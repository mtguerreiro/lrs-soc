/*
 * itm3903cHwIf.c
 *
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "itm3903cHwIf.h"
#include <stdio.h>

#include "itm3903cConfig.h"

#include "ocp/utils/rp.h"

#include "itm3903cHw.h"
#include "string.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

typedef struct{
    rphandle_t handles[ITM3903C_HW_IF_END];
    rpctx_t rp;
}itm3903cHwIf_t;

typedef struct{

    itm3903cHwIf_t interface;
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


//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
int32_t itm3903cHwIfInitialize(void){

    /* Initializes the request processor */
    rpInitialize(&hwControl.interface.rp, ITM3903C_HW_IF_END, hwControl.interface.handles);

    rpRegisterHandle(&hwControl.interface.rp, ITM3903C_HW_IF_SET_SLOPE, itm3903cHwIfSetSlope);
    rpRegisterHandle(&hwControl.interface.rp, ITM3903C_HW_IF_GET_SLOPE, itm3903cHwIfGetSlope);
    rpRegisterHandle(&hwControl.interface.rp, ITM3903C_HW_IF_SET_OFFSET, itm3903cHwIfSetOffset);
    rpRegisterHandle(&hwControl.interface.rp, ITM3903C_HW_IF_GET_OFFSET, itm3903cHwIfGetOffset);
    rpRegisterHandle(&hwControl.interface.rp, ITM3903C_HW_IF_GET_VERSION, itm3903cHwIfGetVersion);
    rpRegisterHandle(&hwControl.interface.rp, ITM3903C_HW_IF_GET_ERROR, itm3903cHwIfGetError);
    rpRegisterHandle(&hwControl.interface.rp, ITM3903C_HW_IF_CLEAR_ERROR, itm3903cHwIfClearError);
    rpRegisterHandle(&hwControl.interface.rp, ITM3903C_HW_IF_SET_OUTPUT_STATUS, itm3903cHwIfSetOutputStatus);
    rpRegisterHandle(&hwControl.interface.rp, ITM3903C_HW_IF_GET_OUTPUT_STATUS, itm3903cHwIfGetOutputStatus);
    rpRegisterHandle(&hwControl.interface.rp, ITM3903C_HW_IF_SET_ANALOG_EXTERNAL_STATUS, itm3903cHwIfSetAnalogExternalStatus);
    rpRegisterHandle(&hwControl.interface.rp, ITM3903C_HW_IF_GET_ANALOG_EXTERNAL_STATUS, itm3903cHwIfGetAnalogExternalStatus);

    return 0;
}
//-----------------------------------------------------------------------------
int32_t itm3903cHwIf(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    int32_t status;

    status = rpRequest(&hwControl.interface.rp, in, insize, out, maxoutsize);

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

static int32_t itm3903cHwIfGetVersion(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    char *o = (char *)*out;
    
    return itm3903cHwGetVersion(o);
}

static int32_t itm3903cHwIfGetError(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    char *o = (char *)*out;
    
    return itm3903cHwGetError(o);
}

static int32_t itm3903cHwIfClearError(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    itm3903cHwClearError();
    return 0;
}

static int32_t itm3903cHwIfSetOutputStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    uint32_t *p = (uint32_t *)in;
    uint32_t setStatus = *p;
    
    itm3903cHwSetOutputStatus(setStatus);

    return 0;
}

static int32_t itm3903cHwIfSetAnalogExternalStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    uint32_t *p = (uint32_t *)in;
    uint32_t setStatus = *p;
    
    itm3903cHwSetAnalogExternalStatus(setStatus);

    return 0;
}

static int32_t itm3903cHwIfGetOutputStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    uint32_t *o = (u_int32_t *) *out;

    uint32_t output_status = itm3903cHwGetOutputStatus();

    *o = output_status;

    return 4;
}

static int32_t itm3903cHwIfGetAnalogExternalStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    uint32_t *o = (u_int32_t *) *out;

    uint32_t output_status = itm3903cHwGetAnalogExternalStatus();

    *o = output_status;

    return 4;
}
//-----------------------------------------------------------------------------
//=============================================================================

