/*
 * boostControlLinearization.c
 *
 *  Created on: 29.03.2023
 *      Author: rodolfo
 */

#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "boostControlLinearization.h"

#include "ocpConfig.h"
#include "ocpTrace.h"

#include "boostConfig.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================

static float L = 0.0f;
static float C = 0.0f;
static float KI = 0.0f;
static float K1 = 0.0f;
static float K2 = 0.0f;
static float alpha = 1.0f;
static float control_f = 0.0f;
static float control_s = 0.0f;

static float v_i = 0.0f;
static float v_o = 0.0f;
static float i_l = 0.0f;
static float i_o = 0.0f;
static float v_o_ref = 0.0f;
static float i_l_ref = 0.0f;
static float i_o_filt = 0.0f;

static float e = 0.0f;
static float e_dot = 0.0f;
static float e_ref = 0.0f;
static float error_dot = 0.0f;
static float error = 0.0f;
static float error_1 = 0.0f;
static float period = 0.0f;

static float rho = 0.0f;
static float u = 0.0f;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void boostControlLinearizationInitialize(void){



}
//-----------------------------------------------------------------------------
int32_t boostControlLinearizationSetParams(void *params, uint32_t n){

	 float *p = (float *)params;

	    L = *p++;
	    C = *p++;
	    KI = *p++;
	    K1 = *p++;
	    K2 = *p++;
	    alpha = *p++;
		control_f = *p++;
		control_s = *p++;
		return 0;

		uint32_t factor= 100e3/control_f;
		uint8_t f = factor;
		boostHwSetAdcDoneIntFactor(f);

}
//-----------------------------------------------------------------------------
int32_t boostControlLinearizationGetParams(void *in, uint32_t insize, void *out, uint32_t maxoutsize){

	 float *p = (float *)out;

	    *p++ = L;
	    *p++ = C;
	    *p++ = KI;
	    *p++ = K1;
	    *p++ = K2;
	    *p++ = alpha;
		*p++ = control_f;
		*p++ = control_s;
	    return 32;
}

//-----------------------------------------------------------------------------
int32_t boostControlLinearizationRun(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    boostConfigMeasurements_t *m = (boostConfigMeasurements_t *)meas;
    boostConfigReferences_t *r = (boostConfigReferences_t *)refs;
    boostConfigControl_t *o = (boostConfigControl_t *)outputs;

//values from sensors
    v_i = m->v_dc_in;
    v_o = m->v_out;

//controller equations
    u = ((rho*L/v_i) + v_o - v_i)/v_o;

    if( u >= 0.98f ) u = 0.98f;
    if( u <= 0.0f ) u = 0.0f;

    o->u = u;
    o->v_o_reference = r->v_o;
    o-> control_s = control_s;

    return sizeof(boostConfigControl_t);
}
//-----------------------------------------------------------------------------
void boostControlLinearizationReset(void){
    u = 0.0f;
    rho = 0.0f;

    e = 0.0f;
    e_dot = 0.0f;
    i_o_filt = 0.0f;
    i_l_ref = 0.0f;
    e_ref = 0.0f;
    error_dot = 0.0f;
    error = 0.0f;
    error_1 = 0.0f;


}
//-----------------------------------------------------------------------------
//=============================================================================
//Energycint_rho functions
//-----------------------------------------------------------------------------
static float boostHwExpMovAvg(float sample, float average);
//-----------------------------------------------------------------------------
int32_t boostControlEnergycint_rhoRun(void *meas, void *outputs){

    boostConfigMeasurements_t *m = (boostConfigMeasurements_t *)meas;
    boostConfigControl_t *o = (boostConfigControl_t *)outputs;

//values from sensors
    i_l = m->i_l;
    i_o = m->i_o;
    v_i = m->v_dc_in;
    v_o = m->v_out;
//value from reference (set point)
    v_o_ref = o->v_o_reference;
//controller equations

    e = (i_l*i_l*L/2) + (v_o*v_o*C/2);
    e_dot = (i_l*v_i) - (i_o_filt*v_o);

    i_o_filt = boostHwExpMovAvg(i_o, i_o_filt);
    i_l_ref = i_o_filt*v_o/v_i;
    e_ref = (i_l_ref*i_l_ref*L/2) + (v_o_ref*v_o_ref*C/2);

    error_dot = e_ref - e;
    period = 1/control_f;
    error = integrator_euler_rho(error_dot, error_1, period);
    error_1 = error;

    rho = (error*KI) - (e*K1) - (e_dot*K2);

    o->rho = rho;
    o->e = e;
	o->e_dot = e_dot;
    o->e_reference = e_ref;
    o->i_o_filt = i_o_filt;
    return sizeof(boostConfigControl_t);
}
//-----------------------------------------------------------------------------
float integrator_euler_rho(float input, float output_1, float ts){

	float output = output_1 + ts*input;

	return output;

}
//------------------------------------------------------------------------------
static float boostHwExpMovAvg(float sample, float average){

    return alpha * sample + (1.0f - alpha) * average;
}
//-----------------------------------------------------------------------------

#endif /* SOC_CPU1 */
