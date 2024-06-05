/*
 * boostControlEnergycMPC.c
 *
 *  Created on: 14.05.2024
 *      Author: rodolfo
 */

#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "boostControlEnergycMPC.h"

#include "ocpConfig.h"
#include "ocpTrace.h"

#include "boostConfig.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

//=============================================================================

static float boostHwExpMovAvg(float sample, float average);

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================

#include "stdint.h"
#include "math.h"

#include "cdmpc/dmpc.h"
#include "cdmpc/dmpc_defs.h"
#include "cdmpc/dmpc_matrices.h"
#include "cdmpc/mvops.h"
#include "cdmpc/qp.h"




/* MPC matrixes */

static float xm[DMPC_CONFIG_NXM] = {0.0f};
static float xm_1[DMPC_CONFIG_NXM] = {0.0f};
static float dx[DMPC_CONFIG_NXM] = {0.0f};

static float ref_mpc[DMPC_CONFIG_NY];

static float u[DMPC_CONFIG_NU + DMPC_CONFIG_ND] = {0.0f};
static float u_1[DMPC_CONFIG_NU + DMPC_CONFIG_ND] = {0.0f};
static float du[DMPC_CONFIG_NU + DMPC_CONFIG_ND] = {0.0f};

static float aux1[DMPC_CONFIG_NXM], aux2[DMPC_CONFIG_NXM];




uint32_t n_iters;


//---------------------------------------------------------------------------

static float e = 0.0f;
static float e_dot = 0.0f;

static float e_ref = 0.0f;
static float i_l_ref = 0.0f;
static float v_o_ref = 0.0f;


static float i_l = 0.0f;
static float i_o = 0.0f;
static float v_i = 0.0f;
static float v_o = 0.0f;


static float L = 0.0f;
static float C = 0.0f;


static float d_min = 0.0f;
static float d_max = 0.0f;
static float i_l_min = 0.0f;
static float i_l_max = 0.0f;
static float u_min = 0.0f;
static float u_max = 0.0f;
static float e_dot_min = 0.0f;
static float e_dot_max = 0.0f;

static float rho = 0.0f;
static float d = 0.0f;


uint32_t i;

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void boostControlEnergycMPCInitialize(void){

}
//-----------------------------------------------------------------------------
int32_t boostControlEnergycMPCSetParams(void *params, uint32_t n){

    float *p = (float *)params;

    L = *p++;
    C = *p++;
    d_min = *p++;
    d_max = *p++;
    i_l_min = *p++;
    i_l_max = *p++;
	return 0;
}
//-----------------------------------------------------------------------------
int32_t boostControlEnergycMPCGetParams(void *in, uint32_t insize, void *out, uint32_t maxoutsize){

    float *p = (float *)out;

    *p++ = L;
    *p++ = C;
    *p++ = d_min;
    *p++ = d_max;
    *p++ = i_l_min;
    *p++ = i_l_max;


    return 24;
}
//-----------------------------------------------------------------------------
/*float integrator_euler(float input, float output_1, float ts){

	float output = output_1 + ts*input;

	return output;

}
*/
//-----------------------------------------------------------------------------
int32_t boostControlEnergycMPCRun(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    boostConfigMeasurements_t *m = (boostConfigMeasurements_t *)meas;
    boostConfigReferences_t *r = (boostConfigReferences_t *)refs;
    boostConfigControl_t *o = (boostConfigControl_t *)outputs;

//values from sensors
    i_l = m->i_l_avg;
    i_o = m->i_o;
    v_i = m->v_dc_in;
    v_o = m->v_out;
//value from reference (set point)
    v_o_ref = r->v_o;

//Reference translation from v_o_ref to e_ref

    e = (i_l*i_l*L/2) + (v_o*v_o*C/2);
    e_dot = (i_l*v_i) - (i_o*v_o);  
    i_l_ref = i_o*v_o/v_i; // i_o*v_o_ref/v_i;
    e_ref = (i_l_ref*i_l_ref*L/2) + (v_o_ref*v_o_ref*C/2);

/* Reference */
    ref_mpc[0] = e_ref;


/* Assembles state vector */
    xm[0] = e;
    xm[1] = e_dot;


/* Delay compensation */
    dmpcDelayComp(xm, xm, u);


/* Gamma values introduced */
    u_min = v_i*(v_i - v_o + (v_o*d_min))/(1e6*L);
    u_max = v_i*(v_i - v_o + (v_o*d_max))/(1e6*L);
    e_dot_min = (v_i*i_l_min) - (v_o*i_o);
    e_dot_max = (v_i*i_l_max) - (v_o*i_o);


    DMPC_CONFIG_U_MIN[0] = u_min;
    DMPC_CONFIG_U_MAX[0] = u_max;
    DMPC_CONFIG_XM_MIN[0] = e_dot_min;
    DMPC_CONFIG_XM_MAX[0] = e_dot_max;


/* Optimization */
    dmpcOpt(xm, xm_1, ref_mpc, u, &n_iters, du);

/* Computes u = du + u_1 */
    sumv(u, du, DMPC_CONFIG_NU, u);

/* Saves variables */
    for(i = 0; i < DMPC_CONFIG_NXM; i++){
    	xm_1[i] = xm[i];
    }

//Linearization
    rho = 1e6*u[0]; //gain required of 1e6
    d = ((rho*L/v_i) + v_o - v_i)/v_o;
//-----------------------------------
    o->u = d;
    o->v_o_reference = r->v_o; //bypassing reference value to an output
    o->e = e;
    o->e_reference = e_ref;
    o->i_o_filt = i_o;
    return sizeof(boostConfigControl_t);
}
//-----------------------------------------------------------------------------
void boostControlEnergycMPCReset(void){
	
	resetArray(xm, DMPC_CONFIG_NXM);
	resetArray(xm_1, DMPC_CONFIG_NXM);
	resetArray(dx, DMPC_CONFIG_NXM);

	resetArray(u, DMPC_CONFIG_NU + DMPC_CONFIG_ND);
	resetArray(u_1, DMPC_CONFIG_NU + DMPC_CONFIG_ND);
	resetArray(du, DMPC_CONFIG_NU + DMPC_CONFIG_ND);
    

}
//-----------------------------------------------------------------------------
//=============================================================================
void resetArray(float *array, int size) {
    for (int i = 0; i < size; i++) {
        array[i] = 0.0f;
    }
}
//-----------------------------------------------------------------------------

#endif /* SOC_CPU1 */
