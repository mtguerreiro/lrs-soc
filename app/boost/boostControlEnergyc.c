/*
 * boostControlEnergyc.c
 *
 *  Created on: 23.02.2023
 *      Author: marco
 */

#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "boostControlEnergyc.h"

#include "ocpConfig.h"
#include "ocpTrace.h"

#include "boostConfig.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
static float boostHwExpMovAvgEnergyc(float sample, float average);
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static float e = 0.0f;
static float e_dot = 0.0f;
static float i_l_ref = 0.0f;
static float e_ref = 0.0f;
static float rho = 0.0f;
static float u = 0.0f;

static float i_l = 0.0f;
static float i_o_filt = 0.0f;
static float i_o = 0.0f;
static float v_i = 0.0f;
static float v_o = 0.0f;


static float L = 0.0f;
static float C = 0.0f;
static float K1 = 0.0f;
static float K2 = 0.0f;
static float v_o_ref = 0.0f;

static float alpha = 0.01f;

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void boostControlEnergycInitialize(void){

}
//-----------------------------------------------------------------------------
int32_t boostControlEnergycSetParams(void *params, uint32_t n){

    float *p = (float *)params;

    L = *p++;
    C = *p++;
    K1 = *p++;
    K2 = *p++;
    //v_o_ref = *p++;

	return 0;
}
//-----------------------------------------------------------------------------
int32_t boostControlEnergycGetParams(void *in, uint32_t insize, void *out, uint32_t maxoutsize){

    float *p = (float *)out;

    *p++ = L;
    *p++ = C;
    *p++ = K1;
    *p++ = K2;
    //*p++ = v_o_ref;


    return 16;
}
//-----------------------------------------------------------------------------
int32_t boostControlEnergycRun(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    boostConfigMeasurements_t *m = (boostConfigMeasurements_t *)meas;
    boostConfigReferences_t *r = (boostConfigReferences_t *)refs;
    boostConfigControl_t *o = (boostConfigControl_t *)outputs;

//values from sensors
    i_l = m->i_l;
    i_o = m->i_o;
    v_i = m->v_dc_in;
    v_o = m->v_out;
//value from reference (set point)
    v_o_ref = r->v_o;

//controller equations
    i_o_filt = boostHwExpMovAvgEnergyc(i_o, i_o_filt);
    e = (i_l*i_l*L/2) + (v_o*v_o*C/2);

    e_dot = (i_l*v_i) - (i_o_filt*v_o);

    i_l_ref = i_o_filt*v_o_ref/v_i;
    //i_l_ref = i_o*v_o/v_i;

    e_ref = (i_l_ref*i_l_ref*L/2) + (v_o_ref*v_o_ref*C/2);

    rho = ((e_ref - e)*K1) - (e_dot*K2);

    u = ((rho*L/v_i) + v_o - v_i)/v_o;


    if( u >= 0.98f ) u = 0.98f;
    if( u <= 0.0f ) u = 0.0f;

    o->u = u;
    o->v_o_reference = r->v_o;//
    o->e = e;
    o->e_reference = e_ref;
    o->i_o_filt = i_o_filt;
    return sizeof(boostConfigControl_t);
}
//-----------------------------------------------------------------------------
void boostControlEnergycReset(void){

    u = 0.0f;
}
//-----------------------------------------------------------------------------
static float boostHwExpMovAvgEnergyc(float sample, float average){

    return alpha * sample + (1.0f - alpha) * average;
}
//=============================================================================
#endif /* SOC_CPU1 */
