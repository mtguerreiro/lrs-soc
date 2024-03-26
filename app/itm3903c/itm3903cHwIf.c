/*
 * itm3903cHwIf.c
 *
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "itm3903cHwIf.h"
#include <stdio.h>
#include <stdbool.h>

#include "itm3903cConfig.h"

#include "ocp/utils/rp.h"

#include "itm3903cHw.h"
#include "string.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

typedef struct{
    rphandle_t handles[ITM3903C_HW_DIGITAL_IF_END];
    rpctx_t rp;
}itm3903cHwDigitalIf_t;

typedef struct{
    rphandle_t handles[ITM3903C_HW_ANALOG_IF_END];
    rpctx_t rp;
}itm3903cHwAnalogIf_t;

typedef struct{

    /*
     * The digital interface uses RS232 to communicate with the source, and is
     * used to configure the supply.
     */
    itm3903cHwDigitalIf_t digitalIf;

    /*
     * The analog interface has access to the hardware that generates the 
     * ADC/DAC system that acquire analog measurements and drive the analog
     * inputs of the supply.
     */
    itm3903cHwAnalogIf_t analogIf;
}itm3903cHwIfControl_t;

static itm3903cHwIfControl_t hwControl;
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static int32_t itm3903cHwIfSetSlope(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetSlope(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfSetOffset(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetOffset(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetVersion(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetError(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfClearError(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfSetOutputStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetOutputStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfSetAnalogExternalStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetAnalogExternalStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfSetFuncMode(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetFuncMode(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfSetVoltValue(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfSetCurrValue(void *in, uint32_t insize, void **out, uint32_t maxoutsize);


//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
int32_t itm3903cHwIfInitialize(void){

    /* Initializes the request processor of the digital interface */
    rpInitialize(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_END, hwControl.digitalIf.handles);

    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_SET_SLOPE, itm3903cHwIfSetSlope);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_SLOPE, itm3903cHwIfGetSlope);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_SET_OFFSET, itm3903cHwIfSetOffset);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_OFFSET, itm3903cHwIfGetOffset);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_VERSION, itm3903cHwIfGetVersion);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_ERROR, itm3903cHwIfGetError);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_CLEAR_ERROR, itm3903cHwIfClearError);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_SET_OUTPUT_STATUS, itm3903cHwIfSetOutputStatus);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_SET_FUNC_MODE, itm3903cHwIfSetFuncMode);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_FUNC_MODE, itm3903cHwIfGetFuncMode);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_OUTPUT_STATUS, itm3903cHwIfGetOutputStatus);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_SET_ANALOG_EXTERNAL_STATUS, itm3903cHwIfSetAnalogExternalStatus);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_ANALOG_EXTERNAL_STATUS, itm3903cHwIfGetAnalogExternalStatus);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_SET_VOLT_VALUE, itm3903cHwIfSetVoltValue);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_SET_CURR_VALUE, itm3903cHwIfSetCurrValue);

    /* Initializes the request processor of the analog interface */
    rpInitialize(&hwControl.analogIf.rp, ITM3903C_HW_ANALOG_IF_END, hwControl.analogIf.handles);

    return 0;
}
//-----------------------------------------------------------------------------
int32_t itm3903cHwDigitalIf(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    int32_t status;

    status = rpRequest(&hwControl.digitalIf.rp, in, insize, out, maxoutsize);

    return status;
}
//-----------------------------------------------------------------------------
int32_t itm3903cHwAnalogIf(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    int32_t status;

    status = rpRequest(&hwControl.analogIf.rp, in, insize, out, maxoutsize);

    return status;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetSlope(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t channel;
    float slope;

    uint32_t *p = (uint32_t *)in;

    channel = *p++;

    slope = *( (float *)p );

    itm3903cHwSetSlope(channel, slope);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetSlope(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t channel;
    float slope;
    float *o = (float *)*out;
    uint32_t *p = (uint32_t *)in;

    channel = *p++;

    slope = itm3903cHwGetSlope(channel);

    *o = slope;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetOffset(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t channel;
    float offset;

    uint32_t *p = (uint32_t *)in;

    channel = *p++;

    offset = *( (float *)p );

    itm3903cHwSetOffset(channel, offset);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetOffset(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t channel;
    float offset;
    float *o = (float *)*out;
    uint32_t *p = (uint32_t *)in;

    channel = *p++;

    offset = itm3903cHwGetOffset(channel);

    *o = offset;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetVersion(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    char *o = (char *)*out;
    
    return itm3903cHwGetVersion(o, maxoutsize);
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetFuncMode(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    char *o = (char *)*out;
    
    return itm3903cHwGetFuncMode(o, maxoutsize);
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetError(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    char *o = (char *)*out;
    
    return itm3903cHwGetError(o, maxoutsize);
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfClearError(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    itm3903cHwClearError();
    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetOutputStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    uint32_t *p = (uint32_t *)in;
    uint32_t setStatus = *p;
    
    itm3903cHwSetOutputStatus(setStatus);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetFuncMode(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    uint32_t *p = (uint32_t *)in;
    uint32_t funcMode = *p;
    
    itm3903cHwSetFuncMode(funcMode);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetAnalogExternalStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    uint32_t *p = (uint32_t *)in;
    uint32_t setStatus = *p;
    
    itm3903cHwSetAnalogExternalStatus(setStatus);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetOutputStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    uint32_t *o = (u_int32_t *) *out;

    uint32_t output_status = itm3903cHwGetOutputStatus();

    *o = output_status;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetAnalogExternalStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    uint32_t *o = (u_int32_t *) *out;

    uint32_t output_status = itm3903cHwGetAnalogExternalStatus();

    *o = output_status;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetVoltValue(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    float volt_value;

    uint32_t *p = (uint32_t *)in;

    volt_value = *( (float *)p );

    itm3903cHwSetValue(volt_value, true);
   
    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetCurrValue(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    float curr_value;

    uint32_t *p = (uint32_t *)in;

    curr_value = *( (float *)p );

    itm3903cHwSetValue(curr_value, false);
   
    return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

