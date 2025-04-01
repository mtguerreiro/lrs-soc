


//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "cukControlCascFblin.h"

#include "ocpConfig.h"
#include "ocp/ocpTrace.h"

#include "cukConfig.h"

#include "controller/controller.h"
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static float u = 0.0f;
static float ki, k_ei, kv, k_ev;
static float dt = 1e-05;
static float vc = 0.0f;

static float ei = 0.0f, ev = 0.0f;

static uint32_t first_enter = 0;

static float kd = 0.5f;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t cukControlCascFblinInitialize(void){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t cukControlCascFblinRun(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    cukConfigMeasurements_t *m = (cukConfigMeasurements_t *)meas;
    cukConfigReferences_t *r = (cukConfigReferences_t *)refs;
    cukConfigControl_t *o = (cukConfigControl_t *)outputs;

    float ir;
    float vi;
    
    vc = m->v1 + (1.0f / CUK_CONFIG_TF_N2N1) * m->v2;

    /* Voltage controller */
    if( first_enter == 0 ){
        ev = -(kv * m->vo_dc) / k_ev;
    }
    else{
        ev = ev + dt * ( (r->vo - kd * m->io_filt) - m->vo_dc );
    }
    ir = (-kv * m->vo_dc - k_ev * ev) * CUK_CONFIG_C_O;

    /* Current controller */
    if( first_enter == 0 ){
        ei = -(ki * m->i1) / k_ei;
        first_enter = 1;
    }
    else{
        ei = ei + dt * (ir - m->i1);
    }
    vi = -ki * m->i1 - k_ei * ei;

    /* Feedback linearization */
    u = 1.0f - (m->vi_dc - CUK_CONFIG_L_IN * vi) / vc;

    if( u > 1.0f ) u = 1.0f;
    if( u < 0.0f ) u = 0.0f;

    o->u = u;

    return sizeof(cukConfigControl_t);
}
//-----------------------------------------------------------------------------
int32_t cukControlCascFblinSetParams(void *params, uint32_t n){

    float *p = (float *)params;

    ki = *p++;
    k_ei = *p++;

    kv = *p++;
    k_ev = *p++;

    dt = *p++;

    kd = *p++;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t cukControlCascFblinGetParams(void *buffer, uint32_t size){

    float *p = (float *)buffer;

    *p++ = ki;
    *p++ = k_ei;

    *p++ = kv;
    *p++ = k_ev;

    *p++ = dt;

    *p++ = kd;

    return 24;
}
//-----------------------------------------------------------------------------
void cukControlCascFblinReset(void){

    ei = 0.0f;
    ev = 0.0f;
    first_enter = 0;
    u = 0.0f;
}
//-----------------------------------------------------------------------------
void cukControlCascFblinGetCallbacks(void *callbacksBuffer){

    controllerCallbacks_t *cbs = (controllerCallbacks_t * )callbacksBuffer;

    cbs->init = cukControlCascFblinInitialize;
    cbs->run = cukControlCascFblinRun;
    cbs->setParams = cukControlCascFblinSetParams;
    cbs->getParams = cukControlCascFblinGetParams;
    cbs->reset = cukControlCascFblinReset;
    cbs->firstEntry = 0;
    cbs->lastExit = 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//=============================================================================
