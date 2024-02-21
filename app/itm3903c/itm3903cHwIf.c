/*
 * itm3903cHwIf.c
 *
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "itm3903cHwIf.h"

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
static int32_t itm3903cHwIfGetVersion(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetError(void *in, uint32_t insize, void **out, uint32_t maxoutsize);


//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
int32_t itm3903cHwIfInitialize(void){

    /* Initializes the request processor */
    rpInitialize(&hwControl.interface.rp, ITM3903C_HW_IF_END, hwControl.interface.handles);

    rpRegisterHandle(&hwControl.interface.rp, ITM3903C_HW_IF_SET_SLOPE, itm3903cHwIfSetSlope);
    rpRegisterHandle(&hwControl.interface.rp, ITM3903C_HW_IF_GET_SLOPE, itm3903cHwIfGetSlope);
    rpRegisterHandle(&hwControl.interface.rp, ITM3903C_HW_IF_GET_VERSION, itm3903cHwIfGetVersion);
    rpRegisterHandle(&hwControl.interface.rp, ITM3903C_HW_IF_GET_ERROR, itm3903cHwIfGetError);

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

static int32_t itm3903cHwIfGetVersion(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    char *o = (char *)*out;
    
    return itm3903cHwGetVersion(o);
}

static int32_t itm3903cHwIfGetError(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    char *o = (char *)*out;
    
    return itm3903cHwGetError(o);
}

//-----------------------------------------------------------------------------
//=============================================================================
