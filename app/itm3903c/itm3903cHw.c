/*
 * itm3903cHw.c
 *
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "itm3903cHw.h"
#include "hardware/uart.h"
#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "string.h"
#include <stdint.h>
#include <stdlib.h>

#include "ocp/hardware/pico/ocpPicoConfig.h"

//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{

    uint32_t status;

    itm3903cConfigMeasurements_t meas;
    itm3903cConfigControl_t control;

    itm3903cConfigMeasGains_t gains;

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
static void itm3903cHwInitializeMeasGains(void);
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
itm3903cHwControl_t hwControl = {.status = 0};

static float supplySlope = 0.0f;
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
int32_t itm3903cHwInitialize(void){

    itm3903cHwInitializeGpio();
    itm3903cHwInitializeAdc();
    itm3903cHwInitializeUart();
    itm3903cHwInitializeSpi();
    itm3903cHwInitializeMeasGains();

    return 0;
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
    
    dst->i =  hwControl.gains.i_gain * ((float)(temp_chan_0)) + hwControl.gains.i_ofs;

    dst->v = hwControl.gains.v_gain * ((float)(temp_chan_1)) + hwControl.gains.v_ofs;

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

    supplySlope = slope;
    char command[50];
    size_t size;
    
    size = snprintf(command, sizeof(command), "EXT:PROG:CHAN:MX %d,%f\r\n", channel, slope);

    uart_write_blocking(OCP_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);
}
//-----------------------------------------------------------------------------
float itm3903cHwGetSlope(uint32_t channel){

    float slope; 
    char command[50];
    size_t size;
    
    size = snprintf(command, sizeof(command), "EXT:PROG:CHAN:MX? %d\r\n", channel);

    uart_write_blocking(OCP_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);
    
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

    uart_write_blocking(OCP_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);
}
//-----------------------------------------------------------------------------
float itm3903cHwGetOffset(uint32_t channel){

    float offset; 
    char command[50]; 
    size_t size;
    
    size = snprintf(command, sizeof(command), "EXT:PROG:CHAN:MB? %d\r\n", channel);

    uart_write_blocking(OCP_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);

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

    uart_write_blocking(OCP_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) command_size);

    size = itm3903HwGetSupplyResponse(o, maxsize);

    return size;
}
//-----------------------------------------------------------------------------
int32_t itm3903cHwGetFuncMode(char * o, uint32_t maxsize){

    int32_t size = 0;
    char command[] = "FUNC?\r\n";
    size_t command_size = sizeof(command) - 1;

    uart_write_blocking(OCP_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) command_size);

    size = itm3903HwGetSupplyResponse(o, maxsize);

    return size;
}
//-----------------------------------------------------------------------------
int32_t itm3903cHwGetError(char * o, uint32_t maxsize){

    int32_t size = 0;
    char command[] = "SYST:ERR?\r\n";
    size_t command_size = sizeof(command) - 1;

    uart_write_blocking(OCP_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) command_size);

    size = itm3903HwGetSupplyResponse(o, maxsize);

    return size;
}
//-----------------------------------------------------------------------------
void itm3903cHwClearError(void){

    char command[] = "SYST:CLE\r\n";
    size_t command_size = sizeof(command) - 1;

    uart_write_blocking(OCP_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) command_size);
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

    uart_write_blocking(OCP_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) command_size);
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

    uart_write_blocking(OCP_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) command_size);
}
//-----------------------------------------------------------------------------
uint32_t itm3903cHwGetOutputStatus(void) {

    uint32_t output_status;
    int32_t size;
    char command[] = "OUTP?\r\n";
    
    size_t command_size = sizeof(command) - 1;

    uart_write_blocking(OCP_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) command_size);

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

    uart_write_blocking(OCP_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) command_size); 
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

    uart_write_blocking(OCP_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) size);
}
//-----------------------------------------------------------------------------
uint32_t itm3903cHwGetAnalogExternalStatus(void) {

    uint32_t output_status;
    int32_t size;
    char command[] = "EXT:PROG?\r\n";

    size_t command_size = sizeof(command) - 1;

    uart_write_blocking(OCP_PICO_CONFIG_RS232_UART, (u_int8_t*) command, (size_t) command_size);
    
    size = itm3903HwGetSupplyResponse(command, sizeof(command));

    if( size < 0 ) output_status = 0xFFFFFFFF;
    else output_status = (uint32_t) atoi(command);

    return output_status;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t itm3903HwGetSupplyResponse(char *buffer, uint32_t maxsize){
    
    uint8_t curr_char = (u_int8_t) '\r';
    bool status;
    int32_t size;

    status = uart_is_readable_within_us(OCP_PICO_CONFIG_RS232_UART, OCP_PICO_CONFIG_RS232_UART_TO_MS * 1000);
    if(status == false) {
        printf("UART timed out\n");
        return -1;
    };

    size = 0;
    while( (curr_char != ((u_int8_t) '\n')) && (size <= maxsize) ){
        
        curr_char = uart_getc(OCP_PICO_CONFIG_RS232_UART);
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

    gpio_set_function(OCP_PICO_CONFIG_RS232_UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(OCP_PICO_CONFIG_RS232_UART_RX_PIN, GPIO_FUNC_UART);

    uart_init(OCP_PICO_CONFIG_RS232_UART, OCP_PICO_CONFIG_RS232_UART_BAUD_RATE);
}
//-----------------------------------------------------------------------------
static void itm3903cHwInitializeSpi(void){


}
//-----------------------------------------------------------------------------
static void itm3903cHwInitializeMeasGains(void){

    hwControl.gains.i_gain = 1.0f;
    hwControl.gains.i_ofs =  0.0f;

    hwControl.gains.v_gain = 1.0f;
    hwControl.gains.v_ofs =  0.0f;
}
//-----------------------------------------------------------------------------
//=============================================================================

