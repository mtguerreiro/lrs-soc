/*
 * itm3903cHw.c
 *
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "itm3903cHw.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include <math.h>
#include "time.h"

#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/uart.h"
#include "hardware/spi.h"
#include "hardware/pwm.h"

#include "ocp/ocp/ocpTrace.h"
#include "ocp/ocp/ocpCS.h"
#include "ocp/ocp/ocpConfig.h"
#include "ocp/hardware/pico/ocpPicoConfig.h"

#include "ocp/hardware/pico/mcp49x2.h"

#include "itm3903cPicoConfig.h"

#include "pico_ocp_freertos/tasks/task_ntpsync.h"

//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{

    uint32_t status;

    itm3903cConfigMeasurements_t meas;

    itm3903cConfigControl_t control;

    itm3903cConfigMeasGains_t gains;

    itm3903cConfigDacGains_t dacGains;

    itm3903cConfigAlphaValues_t alphas;

    itm3903cConfigIPAddresses_t ips;

    /* Sampling period in us */
    uint32_t ts;

    /* A timer is used to trigger the ADC and run the control routine */
    struct repeating_timer samplingTimer;
    bool samplingEnabled;

}itm3903cHwControl_t;
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static int32_t itm3903HwGetSupplyResponse(char *buffer, uint32_t maxsize);
static void itm3903cHwInitializeGpio(void);
static void itm3903cHwInitializeAdc(void);
static void itm3903cHwInitializeUart(void);
static void itm3903cHwInitializeSpi(void);
static void itm3903cHwInitializeTimer(void);
static void itm3903cHwInitializeMeasGains(void);

static void itm3903cHwInitializeDac(void);
static void itm3903cHwDacSpiWrite(uint8_t *data, uint32_t size);
static void itm3903cHwDac1SpiCsSet(void);
static void itm3903cHwDac1SpiCsClear(void);
static void itm3903cHwDac23SpiCsSet(void);
static void itm3903cHwDac23SpiCsClear(void);

static void itm3903cHwAdcIrq(void);
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static itm3903cHwControl_t hwControl = {
    .status = 0,
    .ts = 1000,
    .samplingEnabled = false,
    .dacGains.a1_adj_gain = 1343.33223761f,
    .dacGains.a1_offset_gain = 576.91514275f,
    .dacGains.a1_offset_offset = -3.54150406f,
    .dacGains.a1_adj_offset = -9.3829024f,
    .dacGains.a2_gain = 404.40256795f,
    .dacGains.a2_offset = -9.42171167f,
    .dacGains.a3_gain = 403.96189302f,
    .dacGains.a3_offset = -8.33781641f,
    .gains.i_gain = 246,
    .gains.i_ofs = 58,
    .gains.v_gain = 48,
    .gains.v_ofs = 33,
    .alphas.v = 0.5,
    .alphas.i = 0.5,
    .ips.ipone = 131,67,239,466,
    .ips.iptwo = 131,67,239,466,
    .ips.ipthree = 131,67,239,466,
    .ips.ipfour = 131,67,239,466,
    .ips.ipfive = 131,67,239,466
    };

static float texec = 0.0f;

mcp49x2_t dac_1, dac_23;

//=============================================================================

//=============================================================================
/*--------------------------------- TO-DOs ----------------------------------*/
//=============================================================================
/*
 * - At the moment, a timer triggers execution of the ADC, which is read with
 *   blocking calls. Better would be if the ADC reading was non-blocking. 
 *   However, the pico adc hardware is a bit weird in this regard.
 */
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t itm3903cHwInitializeC0(void){

    itm3903cHwInitializeGpio();
    itm3903cHwInitializeAdc();
    itm3903cHwInitializeUart();
    itm3903cHwInitializeSpi();
    itm3903cHwInitializeMeasGains();

    return 0;
}
//-----------------------------------------------------------------------------
int32_t itm3903cHwInitializeC1(void){

    itm3903cHwInitializeTimer();
    itm3903cHwInitializeDac();

    return 0;
}
//-----------------------------------------------------------------------------
uint32_t itm3903cHwGetC1ControlExecTimeAddr(void){

    return ((uint32_t) &texec); 
}
//-----------------------------------------------------------------------------
int32_t itm3903cHwStatus(void){

    return hwControl.status;
}
//-----------------------------------------------------------------------------
void itm3903cHwStatusClear(void){

    hwControl.status = 0;
}
//-----------------------------------------------------------------------------
int32_t itm3903cHwGetMeasurements(void *meas){

    itm3903cConfigMeasurements_t *dst;

    uint16_t temp_chan_0, temp_chan_1;
    adc_select_input(0);
    adc_run(true);
    temp_chan_0 = adc_fifo_get_blocking();
    temp_chan_1 = adc_fifo_get_blocking();
    adc_run(false);
    adc_fifo_drain(); 
    
    dst = (itm3903cConfigMeasurements_t *)meas;

    dst->v = hwControl.gains.v_gain * ((float)(temp_chan_1)) + hwControl.gains.v_ofs;
    dst->i = hwControl.gains.i_gain * ((float)(temp_chan_0)) + hwControl.gains.i_ofs;
    dst->t = returnL();

    static float previousv; 
    static float previousi;

    static float alphav; 
    static float alphai;

    alphav = hwControl.alphas.v;
    alphai = hwControl.alphas.i;

    dst->v = previousv * (1 - alphav) + (dst->v * alphav);
    dst->i = previousi * (1 - alphai) + (dst->i * alphai);

    previousv = dst->v;
    previousi = dst->i;

    return sizeof(itm3903cConfigMeasurements_t);
}

//-----------------------------------------------------------------------------
int32_t itm3903cHwApplyOutputs(void *outputs, int32_t size){

    itm3903cConfigControl_t *control;

    control = (itm3903cConfigControl_t *)outputs;

    //apply output (write voltage to dac)

    return 0;
}
//-----------------------------------------------------------------------------
void itm3903cHwSetSlope(uint32_t channel, float slope){

    char command[50];
    size_t size;
    
    size = snprintf(command, sizeof(command), "EXT:PROG:CHAN:MX %d,%f\r\n", channel, slope);

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);
}
//-----------------------------------------------------------------------------
float itm3903cHwGetSlope(uint32_t channel){

    float slope; 
    char command[50];
    size_t size;
    
    size = snprintf(command, sizeof(command), "EXT:PROG:CHAN:MX? %d\r\n", channel);

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);
    
    size = itm3903HwGetSupplyResponse(command, sizeof(command));
    if( size < 0 ) slope = NAN;
    else slope = (float) strtod(command, NULL);

    return slope;
}
//-----------------------------------------------------------------------------
void itm3903cHwSetOffset(uint32_t channel, float offset){

    char command[50];
    size_t size;
    
    size = snprintf(command, sizeof(command), "EXT:PROG:CHAN:MB %d,%f\r\n", channel, offset);

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);
}
//-----------------------------------------------------------------------------
float itm3903cHwGetOffset(uint32_t channel){

    float offset; 
    char command[50]; 
    size_t size;
    
    size = snprintf(command, sizeof(command), "EXT:PROG:CHAN:MB? %d\r\n", channel);

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);

    size = itm3903HwGetSupplyResponse(command, sizeof(command));

    if( size < 0 ) offset = NAN;
    else offset = (float) strtod(command, NULL);

    return offset;
}
//-----------------------------------------------------------------------------
int32_t itm3903cHwGetVersion(char *o, uint32_t maxsize){

    int32_t size = 0;
    char command[] = "SYST:VERS?\r\n";
    size_t command_size = sizeof(command) - 1;

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) command_size);
    
    size = itm3903HwGetSupplyResponse(o, maxsize);

    return size;
}
//-----------------------------------------------------------------------------
int32_t itm3903cHwGetFuncMode(char * o, uint32_t maxsize){

    int32_t size = 0;
    char command[] = "FUNC?\r\n";
    size_t command_size = sizeof(command) - 1;

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) command_size);

    size = itm3903HwGetSupplyResponse(o, maxsize);

    return size;
}
//-----------------------------------------------------------------------------
int32_t itm3903cHwGetError(char * o, uint32_t maxsize){

    int32_t size = 0;
    char command[] = "SYST:ERR?\r\n";
    size_t command_size = sizeof(command) - 1;

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) command_size);

    size = itm3903HwGetSupplyResponse(o, maxsize);

    return size;
}
//-----------------------------------------------------------------------------
void itm3903cHwClearError(void){

    char command[] = "SYST:CLE\r\n";
    size_t command_size = sizeof(command) - 1;

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) command_size);
}
//-----------------------------------------------------------------------------
void itm3903cHwSetOutputStatus(uint32_t setStatus){

    char command[] = "OUTP 0\r\n";
    size_t command_size = sizeof(command) - 1;
    char* p = command;

    p += 5;
    if((bool) setStatus) {
        *p = '1';
    }

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) command_size);
}
//-----------------------------------------------------------------------------
void itm3903cHwSetFuncMode(uint32_t funcMode){
    
    char command[] = "FUNC CURR\r\n";
    size_t command_size = sizeof(command) - 1;
    char* p = command;

    p += 5;
    if((bool) funcMode) {
            *p++ = 'V';
            *p++ = 'O';
            *p++ = 'L';
            *p++ = 'T';
    }

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) command_size);
}
//-----------------------------------------------------------------------------
uint32_t itm3903cHwGetOutputStatus(void) {

    uint32_t output_status;
    int32_t size;
    char command[] = "OUTP?\r\n";
    
    size_t command_size = sizeof(command) - 1;

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) command_size);

    size = itm3903HwGetSupplyResponse(command, sizeof(command));

    if( size < 0 ) output_status = 0xFFFFFFFF;
    else output_status = (uint32_t) atoi(command);

    return output_status;
}
//-----------------------------------------------------------------------------
void itm3903cHwSetAnalogExternalStatus(uint32_t setStatus){

    char command[] = "EXT:PROG 0\r\n";

    size_t command_size = sizeof(command) - 1;
    
    if((bool) setStatus) {
        command[9] = '1';
    }

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) command_size); 
}
//-----------------------------------------------------------------------------
void itm3903cHwSetValue(float value, bool currOrVolt){

    char command[50];
    char pre_comm[5] = "CURR";
    char * p = pre_comm;
    size_t size;

    if(currOrVolt) {
        *p++ = 'V';
        *p++ = 'O';
        *p++ = 'L';
        *p++ = 'T';
    };
        
    size = snprintf(command, sizeof(command), "%s %f\r\n", pre_comm, value);

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);
}
//-----------------------------------------------------------------------------
uint32_t itm3903cHwGetAnalogExternalStatus(void) {

    uint32_t output_status;
    int32_t size;
    char command[] = "EXT:PROG?\r\n";

    size_t command_size = sizeof(command) - 1;

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) command_size);
    
    size = itm3903HwGetSupplyResponse(command, sizeof(command));

    if( size < 0 ) output_status = 0xFFFFFFFF;
    else output_status = (uint32_t) atoi(command);

    return output_status;
}
//-----------------------------------------------------------------------------
float itm3903cHwGetVoltageMeasurement(void){

    float voltage_measurement;
    char command[50];
    size_t size;
    
    size = snprintf(command, sizeof(command), "MEAS:VOLT?\r\n");   

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);
    
    size = itm3903HwGetSupplyResponse(command, sizeof(command));

    if( size < 0 ) voltage_measurement = NAN;
    else voltage_measurement = (float) strtod(command, NULL);

    return voltage_measurement;
}
//-----------------------------------------------------------------------------
float itm3903cHwGetCurrentMeasurement(void){

    float current_measurement; 
    char command[50];
    size_t size;
    
    size = snprintf(command, sizeof(command), "MEAS:CURR?\r\n");    

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);
    
    size = itm3903HwGetSupplyResponse(command, sizeof(command));
    if( size < 0 ) current_measurement = NAN;
    else current_measurement = (float) strtod(command, NULL);

    return current_measurement;
}
//-----------------------------------------------------------------------------
float itm3903cHwGetVoltageMax(void){

    float voltage_max; 
    char command[50];
    size_t size;
    
    size = snprintf(command, sizeof(command), "VOLT:LIM? \r\n");

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);
    
    size = itm3903HwGetSupplyResponse(command, sizeof(command));
    if( size < 0 ) voltage_max = NAN;
    else voltage_max = (float) strtod(command, NULL);

    return voltage_max;
}
//-----------------------------------------------------------------------------
float itm3903cHwGetVoltageMin(void){

    float voltage_min; 
    char command[50];
    size_t size;
    
    size = snprintf(command, sizeof(command), "VOLT:LIM:NEG? \r\n");

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);
    
    size = itm3903HwGetSupplyResponse(command, sizeof(command));
    if( size < 0 ) voltage_min = NAN;
    else voltage_min = (float) strtod(command, NULL);

    return voltage_min;
}
//-----------------------------------------------------------------------------
void itm3903cHwSetVoltageMax(float value){

    char command[50];
    size_t size;
        
    size = snprintf(command, sizeof(command), "VOLT:LIM %f\r\n", value);

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);
}
//-----------------------------------------------------------------------------
void itm3903cHwSetVoltageMin(float value){

    char command[50];
    size_t size;
        
    size = snprintf(command, sizeof(command), "VOLT:LIM:NEG %f\r\n", value);

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);
}
//-----------------------------------------------------------------------------
float itm3903cHwGetPowerMax(void){

    float power_max; 
    char command[50];
    size_t size;
    
    size = snprintf(command, sizeof(command), "POW:LIM? \r\n");

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);
    
    size = itm3903HwGetSupplyResponse(command, sizeof(command));
    if( size < 0 ) power_max = NAN;
    else power_max = (float) strtod(command, NULL);

    return power_max;
}
//-----------------------------------------------------------------------------
float itm3903cHwGetPowerMin(void){

    float power_min; 
    char command[50];
    size_t size;
    
    size = snprintf(command, sizeof(command), "POW:LIM:NEG? \r\n");

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);
    
    size = itm3903HwGetSupplyResponse(command, sizeof(command));
    if( size < 0 ) power_min = NAN;
    else power_min = (float) strtod(command, NULL);

    return power_min;
}
//-----------------------------------------------------------------------------
void itm3903cHwSetPowerMax(float value){

    char command[50];
    size_t size;
        
    size = snprintf(command, sizeof(command), "POW:LIM %f\r\n", value);

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);
}
//-----------------------------------------------------------------------------
void itm3903cHwSetPowerMin(float value){

    char command[50];
    size_t size;
        
    size = snprintf(command, sizeof(command), "POW:LIM:NEG %f\r\n", value);

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);
}
//-----------------------------------------------------------------------------
float itm3903cHwGetCurrentMax(void){

    float current_max; 
    char command[50];
    size_t size;
    
    size = snprintf(command, sizeof(command), "CURR:LIM? \r\n");

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);
    
    size = itm3903HwGetSupplyResponse(command, sizeof(command));
    if( size < 0 ) current_max = NAN;
    else current_max = (float) strtod(command, NULL);

    return current_max;
}
//-----------------------------------------------------------------------------
float itm3903cHwGetCurrentMin(void){

    float current_min; 
    char command[50];
    size_t size;
    
    size = snprintf(command, sizeof(command), "CURR:LIM:NEG? \r\n");

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);
    
    size = itm3903HwGetSupplyResponse(command, sizeof(command));
    if( size < 0 ) current_min = NAN;
    else current_min = (float) strtod(command, NULL);

    return current_min;
}
//-----------------------------------------------------------------------------
void itm3903cHwSetCurrentMax(float value){

    char command[50];
    size_t size;
        
    size = snprintf(command, sizeof(command), "CURR:LIM %f\r\n", value);

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);
}
//-----------------------------------------------------------------------------
void itm3903cHwSetCurrentMin(float value){

    char command[50];
    size_t size;
        
    size = snprintf(command, sizeof(command), "CURR:LIM:NEG %f\r\n", value);

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);
}
//-----------------------------------------------------------------------------
uint32_t itm3903cHwGetVoltageProtectionStatus(void) {

    uint32_t volt_prot_status;
    int32_t size;
    char command[] = "VOLT:PROT:STAT?\r\n";

    size_t command_size = sizeof(command) - 1;

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) command_size);
    
    size = itm3903HwGetSupplyResponse(command, sizeof(command));

    if( size < 0 ) volt_prot_status = 0xFFFFFFFF;
    else volt_prot_status = (uint32_t) atoi(command);

    return volt_prot_status;
}
//-----------------------------------------------------------------------------
void itm3903cHwSetVoltageProtectionStatus(uint32_t setStatus){

    char command[] = "VOLT:PROT:STAT 0\r\n";

    size_t command_size = sizeof(command) - 1;
    
    if((bool) setStatus) {
        command[15] = '1';
    }

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) command_size); 
}
//-----------------------------------------------------------------------------
uint32_t itm3903cHwGetVoltageUnderProtectionStatus(void) {

    uint32_t volt_uprot_status;
    int32_t size;
    char command[] = "VOLT:UND:PROT:STAT?\r\n";

    size_t command_size = sizeof(command) - 1;

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) command_size);
    
    size = itm3903HwGetSupplyResponse(command, sizeof(command));

    if( size < 0 ) volt_uprot_status = 0xFFFFFFFF;
    else volt_uprot_status = (uint32_t) atoi(command);

    return volt_uprot_status;
}
//-----------------------------------------------------------------------------
float itm3903cHwGetVoltageOverProtectionValue(void) {

    float volt_oprot_value; 
    char command[50];
    size_t size;
    
    size = snprintf(command, sizeof(command), "VOLT:PROT?\r\n");   

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);
    
    size = itm3903HwGetSupplyResponse(command, sizeof(command));

    if( size < 0 ) volt_oprot_value = NAN;
    else volt_oprot_value = (float) strtod(command, NULL);

    return volt_oprot_value;
}
//-----------------------------------------------------------------------------
float itm3903cHwGetVoltageUnderProtectionValue(void) {

    float volt_uprot_value; 
    char command[50];
    size_t size;
    
    size = snprintf(command, sizeof(command), "VOLT:UND:PROT?\r\n");   

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);
    
    size = itm3903HwGetSupplyResponse(command, sizeof(command));

    if( size < 0 ) volt_uprot_value = NAN;
    else volt_uprot_value = (float) strtod(command, NULL);

    return volt_uprot_value;
}
//-----------------------------------------------------------------------------
uint32_t itm3903cHwGetPowerProtectionStatus(void) {

    uint32_t power_prot_status;
    int32_t size;
    char command[] = "POW:PROT:STAT?\r\n";

    size_t command_size = sizeof(command) - 1;

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) command_size);
    
    size = itm3903HwGetSupplyResponse(command, sizeof(command));

    if( size < 0 ) power_prot_status = 0xFFFFFFFF;
    else power_prot_status = (uint32_t) atoi(command);

    return power_prot_status;
}
//-----------------------------------------------------------------------------
void itm3903cHwSetPowerProtectionStatus(uint32_t setStatus){

    char command[] = "POW:PROT:STAT 0\r\n";

    size_t command_size = sizeof(command) - 1;
    
    if((bool) setStatus) {
        command[14] = '1';
    }

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) command_size); 
}
//-----------------------------------------------------------------------------
float itm3903cHwGetPowerOverProtectionValue(void) {

    float pow_oprot_value; 
    char command[50];
    size_t size;
    
    size = snprintf(command, sizeof(command), "POW:PROT?\r\n");   

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);
    
    size = itm3903HwGetSupplyResponse(command, sizeof(command));

    if( size < 0 ) pow_oprot_value = NAN;
    else pow_oprot_value = (float) strtod(command, NULL);

    return pow_oprot_value;
}
//-----------------------------------------------------------------------------
float itm3903cHwGetPowerUnderProtectionValue(void) {

    float pow_uprot_value; 
    char command[50];
    size_t size;
    
    size = snprintf(command, sizeof(command), "POW:UND:PROT?\r\n");   

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);
    
    size = itm3903HwGetSupplyResponse(command, sizeof(command));

    if( size < 0 ) pow_uprot_value = NAN;
    else pow_uprot_value = (float) strtod(command, NULL);

    return pow_uprot_value;
}
//-----------------------------------------------------------------------------
uint32_t itm3903cHwGetCurrentProtectionStatus(void) {

    uint32_t curr_prot_status;
    int32_t size;
    char command[] = "CURR:PROT:STAT?\r\n";

    size_t command_size = sizeof(command) - 1;

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) command_size);
    
    size = itm3903HwGetSupplyResponse(command, sizeof(command));

    if( size < 0 ) curr_prot_status = 0xFFFFFFFF;
    else curr_prot_status = (uint32_t) atoi(command);

    return curr_prot_status;
}
//-----------------------------------------------------------------------------
void itm3903cHwSetCurrentProtectionStatus(uint32_t setStatus){

    char command[] = "CURR:PROT:STAT 0\r\n";

    size_t command_size = sizeof(command) - 1;
    
    if((bool) setStatus) {
        command[15] = '1';
    }

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) command_size); 
}
//-----------------------------------------------------------------------------
uint32_t itm3903cHwGetCurrentUnderProtectionStatus(void) {

    uint32_t curr_uprot_status;
    int32_t size;
    char command[] = "CURR:UND:PROT:STAT?\r\n";

    size_t command_size = sizeof(command) - 1;

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) command_size);
    
    size = itm3903HwGetSupplyResponse(command, sizeof(command));

    if( size < 0 ) curr_uprot_status = 0xFFFFFFFF;
    else curr_uprot_status = (uint32_t) atoi(command);

    return curr_uprot_status;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float itm3903cHwGetCurrentOverProtectionValue(void) {

    float curr_oprot_value; 
    char command[50];
    size_t size;
    
    size = snprintf(command, sizeof(command), "CURR:PROT?\r\n");   

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);
    
    size = itm3903HwGetSupplyResponse(command, sizeof(command));

    if( size < 0 ) curr_oprot_value = NAN;
    else curr_oprot_value = (float) strtod(command, NULL);

    return curr_oprot_value;
}
//-----------------------------------------------------------------------------
float itm3903cHwGetCurrentUnderProtectionValue(void) {

    float curr_uprot_value; 
    char command[50];
    size_t size;
    
    size = snprintf(command, sizeof(command), "CURR:UND:PROT?\r\n");   

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);
    
    size = itm3903HwGetSupplyResponse(command, sizeof(command));

    if( size < 0 ) curr_uprot_value = NAN;
    else curr_uprot_value = (float) strtod(command, NULL);

    return curr_uprot_value;
}
//-----------------------------------------------------------------------------
float itm3903cHwGetVoltageSlewRate(void){

    float slew_rate; 
    char command[50];
    size_t size;
    
    size = snprintf(command, sizeof(command), "VOLT:SLEW? \r\n");

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);
    
    size = itm3903HwGetSupplyResponse(command, sizeof(command));
    if( size < 0 ) slew_rate = NAN;
    else slew_rate = (float) strtod(command, NULL);

    return slew_rate;
}
//-----------------------------------------------------------------------------
void itm3903cHwSetVoltageSlewRate(float value){

    char command[50];
    size_t size;
        
    size = snprintf(command, sizeof(command), "VOLT:SLEW %f\r\n", value);

    uart_write_blocking(ITM3903C_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);
}
//-----------------------------------------------------------------------------
void itm3903cHwSetSamplingStatus(uint32_t status){

    bool enable;

    if( status ) enable = true;
    else enable = false;

    pwm_set_enabled(ITM3903C_PICO_CONFIG_PWM_SLICE, enable);
    hwControl.samplingEnabled = enable;
}
//-----------------------------------------------------------------------------
uint32_t itm3903cHwGetSamplingStatus(void){

    if( hwControl.samplingEnabled == true ) return 1;

    return 0;
}
//-----------------------------------------------------------------------------
void itm3903cHwSetSamplingFreq(uint32_t freq){

    hwControl.ts = 1000000L / freq;

    pwm_set_wrap(ITM3903C_PICO_CONFIG_PWM_SLICE, hwControl.ts - 1);
}
//-----------------------------------------------------------------------------
uint32_t itm3903cHwGetSamplingFreq(void){

    return (uint32_t)(1000000L / hwControl.ts);
}
//-----------------------------------------------------------------------------
void itm3903cHwDac1WriteOffset(float offset){

    uint16_t flags = MCP49X2_CFG_SET_GA_1 | MCP49X2_CFG_DIS_SHDN | MCP49X2_CFG_WRITE_CH_B;

    uint16_t data = (uint16_t)(hwControl.dacGains.a1_offset_gain * offset + hwControl.dacGains.a1_offset_offset);

    if (data > 4095){
        
        data = 4095;

    }

    mcp49x2Write(&dac_1, ((uint16_t) (data & 0x0FFF)), flags);
}
//-----------------------------------------------------------------------------
void itm3903cHwDac1WriteAdj(float adj){

    uint16_t flags = MCP49X2_CFG_SET_GA_1 | MCP49X2_CFG_DIS_SHDN;

    uint16_t data = (uint16_t)(hwControl.dacGains.a1_adj_gain * adj + hwControl.dacGains.a1_adj_offset);

    if (data > 4095){

        data = 4095;
    }

    mcp49x2Write(&dac_1, ((uint16_t) (data & 0x0FFF)), flags);
}
//-----------------------------------------------------------------------------
void itm3903cHwDac23WriteA2(float value){

    uint16_t flags = MCP49X2_CFG_SET_GA_1 | MCP49X2_CFG_DIS_SHDN;

    uint16_t data = (uint16_t)(hwControl.dacGains.a2_gain * value + hwControl.dacGains.a2_offset);

    if (data > 4095){
        
        data = 4095;

    }

    mcp49x2Write(&dac_23, ((uint16_t) (data & 0x0FFF)), flags);
}
//-----------------------------------------------------------------------------
void itm3903cHwDac23WriteA3(float value){

    uint16_t flags = MCP49X2_CFG_SET_GA_1 | MCP49X2_CFG_DIS_SHDN | MCP49X2_CFG_WRITE_CH_B;

    uint16_t data = (uint16_t)(hwControl.dacGains.a3_gain * value + hwControl.dacGains.a3_offset);

    if (data > 4095){
        
        data = 4095;

    }

    mcp49x2Write(&dac_23, ((uint16_t) (data & 0x0FFF)), flags);
}
//-----------------------------------------------------------------------------
void itm3903cHwSetDacCalData(float *data){

    hwControl.dacGains.a1_adj_gain = *data++;
    hwControl.dacGains.a1_adj_offset = *data++;

    hwControl.dacGains.a1_offset_gain = *data++;
    hwControl.dacGains.a1_offset_offset = *data++;

    hwControl.dacGains.a2_gain = *data++;
    hwControl.dacGains.a2_offset = *data++;

    hwControl.dacGains.a3_gain = *data++;
    hwControl.dacGains.a3_offset = *data++;
}
//-----------------------------------------------------------------------------
void itm3903cHwSetAdcCalData(float *data){

    hwControl.gains.v_gain = *data++;
    hwControl.gains.v_ofs = *data++;

    hwControl.gains.i_gain = *data++;
    hwControl.gains.i_ofs = *data++;
}
//-----------------------------------------------------------------------------
uint32_t itm3903cHwGetDacCalData(float *data){

    *data++ = hwControl.dacGains.a1_adj_gain;
    *data++ = hwControl.dacGains.a1_adj_offset;

    *data++ = hwControl.dacGains.a1_offset_gain;
    *data++ = hwControl.dacGains.a1_offset_offset;

    *data++ = hwControl.dacGains.a2_gain;
    *data++ = hwControl.dacGains.a2_offset;

    *data++ = hwControl.dacGains.a3_gain;
    *data++ = hwControl.dacGains.a3_offset;

    return (8 * 4);
}
//-----------------------------------------------------------------------------
uint32_t itm3903cHwGetAdcCalData(float *data){

    *data++ = hwControl.gains.v_gain;
    *data++ = hwControl.gains.v_ofs;

    *data++ = hwControl.gains.i_gain;
    *data++ = hwControl.gains.i_ofs;

    return (4 * 4);
}
//-----------------------------------------------------------------------------
uint32_t itm3903cHwGetAlphaValues(float *data){

    *data++ = hwControl.alphas.v;
    *data++ = hwControl.alphas.i;

    return (2 * 4);
}
//-----------------------------------------------------------------------------
void itm3903cHwSetAlphaValues(float *data){

    hwControl.alphas.v = *data++;
    hwControl.alphas.i = *data++;
}

//-----------------------------------------------------------------------------
uint32_t itm3903cHwGetNeighborIps(float *data){

    *data++ = hwControl.ips.ipone;
    *data++ = hwControl.ips.iptwo;
    *data++ = hwControl.ips.ipthree;
    *data++ = hwControl.ips.ipfour;
    *data++ = hwControl.ips.ipfive;

    return (5 * 4);
}
//-----------------------------------------------------------------------------
void itm3903cHwSetNeighborIps(float *data){

    hwControl.ips.ipone = *data++;
    hwControl.ips.iptwo = *data++;
    hwControl.ips.ipthree = *data++;
    hwControl.ips.ipfour = *data++;
    hwControl.ips.ipfive = *data++;
}

//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t itm3903HwGetSupplyResponse(char *buffer, uint32_t maxsize){
    
    uint8_t curr_char = (u_int8_t) '\r';
    bool status;
    int32_t size;

    status = uart_is_readable_within_us(ITM3903C_PICO_CONFIG_RS232_UART, ITM3903C_PICO_CONFIG_RS232_UART_TO_MS * 1000);
    if(status == false) {
        printf("UART timed out\n");
        return -1;
    };

    size = 0;
    while( (curr_char != ((u_int8_t) '\n')) && (size <= maxsize) ){
        
        curr_char = uart_getc(ITM3903C_PICO_CONFIG_RS232_UART);
        *buffer++ = (char) curr_char;
        size++;
    }

    if(curr_char != '\n'){
        return -1;
    }

    return size;
}
//-----------------------------------------------------------------------------
static void itm3903cHwInitializeGpio(void){

    gpio_init(ITM3903C_PICO_LED_1);
	gpio_set_dir(ITM3903C_PICO_LED_1, GPIO_OUT);
	gpio_put(ITM3903C_PICO_LED_1, 1);

    gpio_init(ITM3903C_PICO_LED_2);
	gpio_set_dir(ITM3903C_PICO_LED_2, GPIO_OUT);
	gpio_put(ITM3903C_PICO_LED_2, 1);

    gpio_init(ITM3903C_PICO_RELAY_1);
	gpio_set_dir(ITM3903C_PICO_RELAY_1, GPIO_OUT);
	gpio_put(ITM3903C_PICO_RELAY_1, 0);

    gpio_init(ITM3903C_PICO_RELAY_2);
	gpio_set_dir(ITM3903C_PICO_RELAY_2, GPIO_OUT);
	gpio_put(ITM3903C_PICO_RELAY_2, 0);
}
//-----------------------------------------------------------------------------
static void itm3903cHwInitializeAdc(void){

    adc_init();

    // GPIO 26 and 27 enabled
    adc_gpio_init(26);
    adc_gpio_init(27);

    adc_set_round_robin(_u(0x03));

    adc_fifo_setup(true, false, 0, false, false);
    adc_fifo_drain();
}
//-----------------------------------------------------------------------------
static void itm3903cHwInitializeUart(void){

    gpio_set_function(ITM3903C_PICO_CONFIG_RS232_UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(ITM3903C_PICO_CONFIG_RS232_UART_RX_PIN, GPIO_FUNC_UART);

    uart_init(ITM3903C_PICO_CONFIG_RS232_UART, ITM3903C_PICO_CONFIG_RS232_UART_BAUD_RATE);
}
//-----------------------------------------------------------------------------
static void itm3903cHwInitializeSpi(void){

    spi_init(ITM3903C_PICO_CONFIG_DAC_SPI, ITM3903C_PICO_CONFIG_DAC_SPI_CLK);
	gpio_set_function(ITM3903C_PICO_CONFIG_DAC_SPI_CLK_PIN, GPIO_FUNC_SPI);
	gpio_set_function(ITM3903C_PICO_CONFIG_DAC_SPI_SD_PIN, GPIO_FUNC_SPI);

    gpio_init(ITM3903C_PICO_CONFIG_DAC_A1_CS_PIN);
	gpio_set_dir(ITM3903C_PICO_CONFIG_DAC_A1_CS_PIN, GPIO_OUT);
	gpio_put(ITM3903C_PICO_CONFIG_DAC_A1_CS_PIN, 1);

    gpio_init(ITM3903C_PICO_CONFIG_DAC_A2_A3_CS_PIN);
	gpio_set_dir(ITM3903C_PICO_CONFIG_DAC_A2_A3_CS_PIN, GPIO_OUT);
	gpio_put(ITM3903C_PICO_CONFIG_DAC_A2_A3_CS_PIN, 1);
}
//-----------------------------------------------------------------------------
static void itm3903cHwInitializeTimer(void){

    /* Mask slice's IRQ output into PWM block, register interrupt handler  */
    pwm_clear_irq(ITM3903C_PICO_CONFIG_PWM_SLICE);
    pwm_set_irq_enabled(ITM3903C_PICO_CONFIG_PWM_SLICE, true);
    irq_set_exclusive_handler(PWM_IRQ_WRAP, itm3903cHwAdcIrq);
    irq_set_enabled(PWM_IRQ_WRAP, true);
    irq_set_priority(PWM_IRQ_WRAP, ITM3903C_PICO_CONFIG_PWM_IRQ_PRIO);
    
    pwm_config config = pwm_get_default_config();

    /* Set divider and initial clock div */
    pwm_config_set_clkdiv(&config, (float)ITM3903C_PICO_CONFIG_PWM_CLK_DIV);
    pwm_config_set_wrap(&config, hwControl.ts - 1);

    /* Load configurations */
    pwm_init(ITM3903C_PICO_CONFIG_PWM_SLICE, &config, false);
}
//-----------------------------------------------------------------------------
static void itm3903cHwInitializeMeasGains(void){

    hwControl.gains.i_gain = 1.0f;
    hwControl.gains.i_ofs =  0.0f;

    hwControl.gains.v_gain = 1.0f;
    hwControl.gains.v_ofs =  0.0f;
}
//-----------------------------------------------------------------------------
static void itm3903cHwInitializeDac(void){

    uint16_t flags;

    dac_1.spiWrite = itm3903cHwDacSpiWrite;

    dac_1.csSet = itm3903cHwDac1SpiCsSet;
    dac_1.csClear = itm3903cHwDac1SpiCsClear;

    dac_1.res = MCP49X2_RES_12_BIT;

    dac_1.ldacSet = 0;
    dac_1.ldacClear = 0;

    dac_1.shdnSet = 0;
    dac_1.shdnClear = 0;

    /* Writes 0 to the DAC's outputs so that it is not initially undefined */
    flags = MCP49X2_CFG_SET_GA_1 | MCP49X2_CFG_DIS_SHDN;
    mcp49x2Write(&dac_1, 0, flags);

    flags = MCP49X2_CFG_SET_GA_1 | MCP49X2_CFG_DIS_SHDN | MCP49X2_CFG_WRITE_CH_B;
    mcp49x2Write(&dac_1, 0, flags);

    dac_23.spiWrite = itm3903cHwDacSpiWrite;

    dac_23.csSet = itm3903cHwDac23SpiCsSet;
    dac_23.csClear = itm3903cHwDac23SpiCsClear;

    dac_23.res = MCP49X2_RES_12_BIT;

    dac_23.ldacSet = 0;
    dac_23.ldacClear = 0;

    dac_23.shdnSet = 0;
    dac_23.shdnClear = 0;

    /* Writes 0 to the DAC's outputs so that it is not initially undefined */
    flags = MCP49X2_CFG_SET_GA_1 | MCP49X2_CFG_DIS_SHDN;
    mcp49x2Write(&dac_23, 0, flags);

    flags = MCP49X2_CFG_SET_GA_1 | MCP49X2_CFG_DIS_SHDN | MCP49X2_CFG_WRITE_CH_B;
    mcp49x2Write(&dac_23, 0, flags);
}
//-----------------------------------------------------------------------------
static void itm3903cHwDacSpiWrite(uint8_t *data, uint32_t size){

    spi_write_blocking(ITM3903C_PICO_CONFIG_DAC_SPI, data, size);
}
//-----------------------------------------------------------------------------
static void itm3903cHwDac1SpiCsSet(void){

    asm volatile("nop \n nop \n nop");
    gpio_put(ITM3903C_PICO_CONFIG_DAC_A1_CS_PIN, 1);
    asm volatile("nop \n nop \n nop");
}
//-----------------------------------------------------------------------------
static void itm3903cHwDac1SpiCsClear(void){

    asm volatile("nop \n nop \n nop");
    gpio_put(ITM3903C_PICO_CONFIG_DAC_A1_CS_PIN, 0);
    asm volatile("nop \n nop \n nop");
}
//-----------------------------------------------------------------------------
static void itm3903cHwDac23SpiCsSet(void){

    asm volatile("nop \n nop \n nop");
    gpio_put(ITM3903C_PICO_CONFIG_DAC_A2_A3_CS_PIN, 1);
    asm volatile("nop \n nop \n nop");
}
//-----------------------------------------------------------------------------
static void itm3903cHwDac23SpiCsClear(void){

    asm volatile("nop \n nop \n nop");
    gpio_put(ITM3903C_PICO_CONFIG_DAC_A2_A3_CS_PIN, 0);
    asm volatile("nop \n nop \n nop");
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------------- IRQ -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static void itm3903cHwAdcIrq(void){
    
    uint32_t ticks;

    pwm_clear_irq(ITM3903C_PICO_CONFIG_PWM_SLICE);

	gpio_put(ITM3903C_PICO_LED_1, 1);

    ticks = time_us_32();

    ocpCSRun(OCP_CS_1);
    ocpTraceSave(OCP_TRACE_1);

    ticks = time_us_32() - ticks;
    texec = ((float)ticks);

	gpio_put(ITM3903C_PICO_LED_1, 0);
}
//-----------------------------------------------------------------------------
//=============================================================================


