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
#include "itm3903cCommands.h"

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

#define UART_ID uart1
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

    // sends slope to supply
}
//-----------------------------------------------------------------------------
float itm3903cHwGetSlope(uint32_t channel){

    float slope; 

    // gets slope to supply
    slope = supplySlope;

    return slope;
}

int32_t itm3903cHwGetVersion(char * o){
    int32_t size = 0;
    char response[MAX_SIZE];
    char *ptr = response;
    
    uart_write_blocking(UART_ID, (u_int8_t*) COMMAND_GETVERSION, (size_t) SIZE_COMMAND_GETVERSION);

    itm3903HwFillResponseBuffer(response, &size);

    if(size < 0){
        return -1;
    }

    memcpy(o, ptr, size);
    return MAX_SIZE;

}

int32_t itm3903cHwGetError(char * o){
    int32_t size = 0;
    char response[MAX_SIZE];
    char *ptr = response;
    
    uart_write_blocking(UART_ID, (u_int8_t*) COMMAND_GETERROR, (size_t) SIZE_COMMAND_GETERROR);

    itm3903HwFillResponseBuffer(response, &size);

    if(size < 0){
        return -1;
    }
    
    memcpy(o, ptr, size);
    return MAX_SIZE;

}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static void itm3903HwFillResponseBuffer(char *buffer, int32_t *size){
    uint8_t curr_char = (u_int8_t) '\r';
    while(curr_char != (u_int8_t) '\n'){
        curr_char = uart_getc(UART_ID);
        *buffer++ = (char) curr_char;
        (*size)++;
    }

    if(curr_char != '\n'){
        *size = -1;
        return;
    }

    *buffer++ = '\0';
    (*size)++;
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

