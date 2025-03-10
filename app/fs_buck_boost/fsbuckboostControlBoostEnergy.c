
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "fsbuckboostControlBoostEnergy.h"
#include "fsbuckboostConfig.h"

#include "controller/controller.h"

#include "utils/dfilt.h"
//============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

#define C_SC    12.5f
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static float C = 100e-6;
static float L = 15e-6;

static float vo_r = 0.0f;
static float il_r = 0.0f;

static float yr = 0.0f;
static float y = 0.0f, y_dot = 0.0f;

static float u = 0.0f, v = 0.0f;

static float k1 = 7743839.640425906f, k2 = 5599.999998136721, k3 = -5375358561.238816;

static float dt = 1.0 / 100e3;

static float e = 0.0f, e_dot = 0.0f;

static float io_filt = 0.0f;

static float alpha = 0.78;
static uint32_t filt_en = 0;

static uint32_t first_enter = 0;

static float kd = 5;

static float sc_soe_ref = (1.f / 2.f) * C_SC * (9.0f * 9.0f);
static float sc_soe = 0.0f;
static float sc_soe_e_dot = 0.0f;
static float sc_soe_e = 0.0f;
static float sc_soe_k1 = 16.0f, sc_soe_k2 = -93.78436535876395f;
static float sc_soe_gain = 1.0f / 50.0f;
static uint32_t sc_soe_first_enter = 0;
static uint32_t sc_soe_en = 0;
float sc_soe_dv;
//=============================================================================


//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostEnergyInit(void){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostEnergyRun(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs){

    fsbuckboostConfigMeasurements_t *m = (fsbuckboostConfigMeasurements_t *)meas;
    fsbuckboostConfigControl_t *o = (fsbuckboostConfigControl_t *)outputs;
    fsbuckboostConfigReferences_t *r = (fsbuckboostConfigReferences_t *)refs;

    if(first_enter == 0 ){
        io_filt = m->io;
    }

    if( filt_en != 0 ) io_filt = dfiltExpMovAvg(m->io, io_filt, alpha);
    else io_filt = m->io;

    /* References */
    il_r = m->v_dc_out * io_filt / m->v_in;

    vo_r = r->v_out - kd * io_filt;

    if( sc_soe_en == 1 ){
        sc_soe = (1.f / 2.f) * C_SC * (m->v_in * m->v_in);
        if( sc_soe_first_enter == 0 ){
            sc_soe_first_enter = 1;
            sc_soe_e = -(sc_soe_k1 * sc_soe) / sc_soe_k2;
        }
        sc_soe_e = sc_soe_e + dt * (sc_soe_ref - sc_soe);
        sc_soe_dv = - sc_soe_k1 * sc_soe - sc_soe_k2 * sc_soe_e;
        vo_r = r->v_out - sc_soe_dv * sc_soe_gain;
    }

    yr = (1. / 2.) * C * vo_r * vo_r + (1. / 2.) * L * il_r * il_r;

    /* States */
    y = (1. / 2.) * C * m->v_dc_out * m->v_dc_out + (1. / 2.) * L * m->il * m->il;
    y_dot = m->v_in * m->il - m->v_dc_out * io_filt;

    if( first_enter == 0 ){
        e =  -(k1 * y + k2 * y_dot) / k3;
        first_enter = 1;
    }

    e_dot = yr - y;
    e = e + dt * e_dot;

    /* Control law */
    v = - k1 * y - k2 * y_dot - k3 * e;

    /* Feedback linearization */
    u = 1.0f - 1.0f / m->v_dc_out * (m->v_in - L / m->v_in * v);

    if( u > 1.0f ) u = 1.0f;
    else if( u < 0.0f ) u = 0.0f;

    o->u = u;

    return sizeof(fsbuckboostConfigControl_t);
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostEnergySetParams(void *params, uint32_t size){

    float *p = (float *)params;

    k1 = *p++;
    k2 = *p++;
    k3 = *p++;
    dt = *p++;

    C = *p++;
    L = *p++;

    alpha = *p++;
    filt_en = (uint32_t)*p++;

    kd = *p++;

    sc_soe_en = (uint32_t)*p++;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostEnergyGetParams(void *buffer, uint32_t size){

    float *p = (float *)buffer;

    *p++ = k1;
    *p++ = k2;
    *p++ = k3;
    *p++ = dt;

    *p++ = C;
    *p++ = L;

    *p++ = alpha;
    *p++ = (float)filt_en;

    *p++ = kd;
    *p++ = (float)sc_soe_en;

    return 40;
}
//-----------------------------------------------------------------------------
void fsbuckboostControlBoostEnergyReset(void){

    e = 0.0f;
    first_enter = 0;

    sc_soe_e = 0.0f;
    sc_soe_first_enter = 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostEnergyFirstEntry(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControlBoostEnergyLastExit(void *meas, int32_t nmeas,
    void *refs, int32_t nrefs,
    void *outputs, int32_t nmaxoutputs){

    return 0;
}
//-----------------------------------------------------------------------------
void fsbuckboostControlBoostEnergyGetCallbacks(void *callbacksBuffer){

    controllerCallbacks_t *cbs = (controllerCallbacks_t * )callbacksBuffer;

    cbs->init = fsbuckboostControlBoostEnergyInit;
    cbs->run = fsbuckboostControlBoostEnergyRun;
    cbs->setParams = fsbuckboostControlBoostEnergySetParams;
    cbs->getParams = fsbuckboostControlBoostEnergyGetParams;
    cbs->reset = fsbuckboostControlBoostEnergyReset;
    cbs->firstEntry = fsbuckboostControlBoostEnergyFirstEntry;
    cbs->lastExit = fsbuckboostControlBoostEnergyLastExit;
}
//-----------------------------------------------------------------------------
//=============================================================================
