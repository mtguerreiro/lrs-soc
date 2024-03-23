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
const int32_t MAX_SIZE = 100;
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
static void itm3903HwFillResponseBuffer(char *buffer, int32_t *size);
static void itm3903cHwInitializeAdc(void *intc, itm3903cHwAdcIrqHandle_t irqhandle);
static void itm3903cHwInitializePwm(void);
static void itm3903cHwInitializeGpio(void);
static void itm3903cHwInitializeMeasGains(void);
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
itm3903cHwControl_t hwControl = {.status = 0};

static float supplySlope = 0.0f;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t itm3903cHwInitialize(itm3903cHwInitConfig_t *config){

    itm3903cHwInitializeAdc(config->intc, config->irqhandle);
    itm3903cHwInitializePwm();
    itm3903cHwInitializeGpio();
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
    
   //chan_0 = (float) temp_chan_0 * conversion_factor;
   //chan_1 = (float) temp_chan_1 * conversion_factor;

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
    // TODO: Change mallocs to set buffer (sprintfn doesnt overflow)
    char command[50];

    size_t size = sprintf(command, "EXT:PROG:CHAN:MX %d,%f\r\n", channel, slope);

    uart_write_blocking(OCP_PICO_CONFIG_UART_RS232_ID, (u_int8_t*) command, (size_t) size);

    // sends slope to supply
}
//-----------------------------------------------------------------------------
float itm3903cHwGetSlope(uint32_t channel){

    float slope; 

    int32_t size_response = 0;

    char command[50];

    size_t size = sprintf(command, "EXT:PROG:CHAN:MX? %d\r\n", channel);

    uart_write_blocking(OCP_PICO_CONFIG_UART_RS232_ID, (u_int8_t*) command, (size_t) size);
    
    char o[10]; 

    itm3903HwFillResponseBuffer(o, &size_response);
    slope = (float) strtod(o, NULL);
    // gets slope to supply
    // slope = supplySlope;
    return slope;
}
//-----------------------------------------------------------------------------
void itm3903cHwSetOffset(uint32_t channel, float offset){

    // supplyOffset = offset;
    char command[50];

    size_t size = sprintf(command, "EXT:PROG:CHAN:MB %d,%f\r\n", channel, offset);

    uart_write_blocking(OCP_PICO_CONFIG_UART_RS232_ID, (u_int8_t*) command, (size_t) size);
    
}
//-----------------------------------------------------------------------------
float itm3903cHwGetOffset(uint32_t channel){

    float offset; 

    int32_t size_response = 0;

    char command[50]; 

    size_t size = sprintf(command, "EXT:PROG:CHAN:MB? %d\r\n", channel);

    uart_write_blocking(OCP_PICO_CONFIG_UART_RS232_ID, (u_int8_t*) command, (size_t) size);

    char o[10]; 

    itm3903HwFillResponseBuffer(o, &size_response);

    offset = (float) strtod(o, NULL);

    // gets offset to supply
    // offset = supplyOffset;
    return offset;
}

int32_t itm3903cHwGetVersion(char * o){
    int32_t size = 0;

    char command[] = "SYST:VERS?\r\n";

    size_t command_size = sizeof(command);

    uart_write_blocking(OCP_PICO_CONFIG_UART_RS232_ID, (u_int8_t*) command, (size_t) command_size);

    itm3903HwFillResponseBuffer(o, &size);

    return size;

}

int32_t itm3903cHwGetFuncMode(char * o){
    int32_t size = 0;

    char command[] = "FUNC?\r\n";

    size_t command_size = sizeof(command);

    uart_write_blocking(OCP_PICO_CONFIG_UART_RS232_ID, (u_int8_t*) command, (size_t) command_size);

    itm3903HwFillResponseBuffer(o, &size);

    return size;

}

int32_t itm3903cHwGetError(char * o){
    int32_t size = 0;

    char command[] = "SYST:ERR?\r\n";

    size_t command_size = sizeof(command);

    uart_write_blocking(OCP_PICO_CONFIG_UART_RS232_ID, (u_int8_t*) command, (size_t) command_size);

    itm3903HwFillResponseBuffer(o, &size);

    return size;

}

void itm3903cHwClearError(){
    char command[] = "SYST:CLE\r\n";

    size_t command_size = sizeof(command);

    uart_write_blocking(OCP_PICO_CONFIG_UART_RS232_ID, (u_int8_t*) command, (size_t) command_size);
}

void itm3903cHwSetOutputStatus(uint32_t setStatus){

    char command[] = "OUTP 0\r\n";

    size_t command_size = sizeof(command);

    char* p = command;

    p += 5;
    
    if((bool) setStatus) {
        *p = '1';
    }

    uart_write_blocking(OCP_PICO_CONFIG_UART_RS232_ID, (u_int8_t*) command, (size_t) command_size);

}

void itm3903cHwSetFuncMode(uint32_t funcMode){
    
    char command[] = "FUNC CURR\r\n";

    size_t command_size = sizeof(command);

    char* p = command;

    p += 5;
    
    if((bool) funcMode) {
            *p++ = 'V';
            *p++ = 'O';
            *p++ = 'L';
            *p++ = 'T';
    }

    uart_write_blocking(OCP_PICO_CONFIG_UART_RS232_ID, (u_int8_t*) command, (size_t) command_size);
    
}

uint32_t itm3903cHwGetOutputStatus() {

    uint32_t output_status;

    char command[] = "OUTP?\r\n";
    
    size_t command_size = sizeof(command);

    uart_write_blocking(OCP_PICO_CONFIG_UART_RS232_ID, (u_int8_t*) command, (size_t) command_size);
    
    char o[4];

    int32_t size = 0;

    itm3903HwFillResponseBuffer(o, &size);

    output_status = (uint32_t) atoi(o);

    return output_status;

}

void itm3903cHwSetAnalogExternalStatus(uint32_t setStatus){
    // char * command = strdup("EXT:PROG 0\r\n");
    // TODO: Change malloc to arrays
    char command[] = "EXT:PROG 0\r\n";

    size_t command_size = sizeof(command);
    
    if((bool) setStatus) {

        command[9] = '1';

    }

    uart_write_blocking(OCP_PICO_CONFIG_UART_RS232_ID, (u_int8_t*) command, (size_t) command_size);
    
}

void itm3903cHwSetValue(float value, bool currOrVolt){
    char command[50];
    char pre_comm[5] = "CURR";
    char * p = pre_comm;

    if(currOrVolt) {
        *p++ = 'V';
        *p++ = 'O';
        *p++ = 'L';
        *p++ = 'T';
    };
    
    size_t size = sprintf(command, "%s %f\r\n", pre_comm, value);

    uart_write_blocking(OCP_PICO_CONFIG_UART_RS232_ID, (u_int8_t*) command, (size_t) size);

}

uint32_t itm3903cHwGetAnalogExternalStatus() {
    uint32_t output_status;

    char command[] = "EXT:PROG?\r\n";

    size_t command_size = sizeof(command);

    uart_write_blocking(OCP_PICO_CONFIG_UART_RS232_ID, (u_int8_t*) command, (size_t) command_size);
    
    char o[4];

    int32_t size = 0;

    itm3903HwFillResponseBuffer(o, &size);

    output_status = (uint32_t) atoi(o);

    return output_status;

}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static void itm3903HwFillResponseBuffer(char *buffer, int32_t *size){
    uint8_t curr_char = (u_int8_t) '\r';

    if(!uart_is_readable_within_us(OCP_PICO_CONFIG_UART_RS232_ID, 1e6)) {
        printf("UART timed out\n");
        *size = -1;
        return;
    };

    while(curr_char != (u_int8_t) '\n'){
        
        curr_char = uart_getc(OCP_PICO_CONFIG_UART_RS232_ID);
        *buffer++ = (char) curr_char;
        (*size)++;
    }

    if(curr_char != '\n'){
        *size = -1;
        return;
    }

    // *buffer++ = '\0';
    // (*size)++;
}
//-----------------------------------------------------------------------------
static void itm3903cHwInitializeAdc(void *intc, itm3903cHwAdcIrqHandle_t irqhandle){


}
//-----------------------------------------------------------------------------
static void itm3903cHwInitializePwm(void){


}
//-----------------------------------------------------------------------------
static void itm3903cHwInitializeGpio(void){


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
