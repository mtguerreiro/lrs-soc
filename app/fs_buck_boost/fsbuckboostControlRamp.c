
//#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "fsbuckboostControlRamp.h"

#include "ocpConfig.h"

#include "fsbuckboostConfig.h"

/* Controllers */
#include "controller/controller.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static float u = 0.0f;
static float u_ref  = 0.5f;
static float u_step = 0.001;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlRampInitialize(void){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlRampSetParams(void *params, uint32_t n){

    float *p = (float *)params;

    u_step = *p++;
    u_ref  = *p++;

    if( u_step < 0 ) u_step = 0.0f;

    if( u_ref  > 1.0f ) u_ref  = 1.0f;
    else if( u_ref  < 0.0f) u_ref  = 0.0f;

	return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlRampGetParams(void *buffer, uint32_t size){

    float *p = (float *)buffer;

    *p++ = u_step;
    *p++ = u_ref ;

    return 8;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlRampRun(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    fsbuckboostConfigMeasurements_t *m = (fsbuckboostConfigMeasurements_t *)meas;
    fsbuckboostConfigReferences_t *r = (fsbuckboostConfigReferences_t *)refs;
    fsbuckboostConfigControl_t *o = (fsbuckboostConfigControl_t *)outputs;

    if( o->u < u_ref  ){
        o->u = o->u + u_step;
        if(o->u > u_ref ) o->u = u_ref ;
    }

    else{
        o->u = o->u - u_step;
        if(o->u < u_ref ) o->u = u_ref ;
    }

    return sizeof(fsbuckboostConfigControl_t);
}
//-----------------------------------------------------------------------------
void fsbuckboostControlRampReset(void){

    u = 0.0f;
}
//-----------------------------------------------------------------------------
void fsbuckboostControlRampGetCallbacks(void *callbacksBuffer){

    controllerCallbacks_t *cbs = (controllerCallbacks_t * )callbacksBuffer;

    cbs->init = fsbuckboostControlRampInitialize;
    cbs->run = fsbuckboostControlRampRun;
    cbs->setParams = fsbuckboostControlRampSetParams;
    cbs->getParams = fsbuckboostControlRampGetParams;
    cbs->reset = fsbuckboostControlRampReset;
    cbs->firstEntry = 0;
    cbs->lastExit = 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
//#endif /* SOC_CPU1 */
