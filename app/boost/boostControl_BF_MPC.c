/*
 * boostControl_BF_MPC.c
 *
 *  Created on: 29.03.2023
 *      Author: rodolfo
 */

#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================


#include "ocpConfig.h"
#include "ocpTrace.h"

#include "boostConfig.h"

#include "math.h"
#include "cdmpc/dmpc.h"
#include "cdmpc/dmpc_defs.h"
#include "cdmpc/dmpc_matrices.h"
#include "cdmpc/mvops.h"
#include "cdmpc/qp.h"


//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================

static float L = 0.0f;
static float C = 0.0f;

static float switching_f = 100e3;
static float control_f = 100e3;
static int factor = 1;

static float v_i = 0.0f;
static float v_o = 0.0f;
static float i_l = 0.0f;
static float i_o = 0.0f;
static float v_o_ref = 0.0f;
static float i_l_ref = 0.0f;
static float i_o_filt = 0.0f;
static float i_l_filt = 0.0f;
static float p_o = 0.0f;

static float e = 0.0f;
static float e_dot = 0.0f;
static float e_ref = 0.0f;
static float rho = 0.0f;
static float rho_1 = 0.0f;
static float d = 0.0f;

static float alpha = 1e-2;
static float alpha_l = 1;

static float xm[DMPC_CONFIG_NXM] = {0.0f};
static float xm_1[DMPC_CONFIG_NXM] = {0.0f};
static float dx[DMPC_CONFIG_NXM] = {0.0f};
static float ref_mpc[DMPC_CONFIG_NY];
static float u[DMPC_CONFIG_NU + DMPC_CONFIG_ND] = {0.0f};
static float u_1[DMPC_CONFIG_NU + DMPC_CONFIG_ND] = {0.0f};
static float du[DMPC_CONFIG_NU + DMPC_CONFIG_ND] = {0.0f};
static float aux1[DMPC_CONFIG_NXM], aux2[DMPC_CONFIG_NXM];

static float d_min = 0.0f;
static float d_max = 1.0f;
static float i_l_min = 0.0f;
static float i_l_max = 0.0f;
static float u_min = 0.0f;
static float u_max = 0.0f;
static float e_dot_min = 0.0f;
static float e_dot_max = 0.0f;

static int first_run = 1;
static int first_run_i_l = 1;
static int first_time_factor = 1;
uint32_t n_iters_mpc;

static int counter = 0;
static int mpc_flag = 0;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void boostControl_BF_MPC_Initialize(void){



}
//-----------------------------------------------------------------------------
int32_t boostControl_BF_MPC_SetParams(void *params, uint32_t n){


	 float *p = (float *)params;

	    L = *p++;
	    C = *p++;
	    i_l_min = *p++;
	    i_l_max = *p++;
	    control_f = *p++;
	    alpha = *p++;
	    alpha_l = *p++;

	    first_time_factor = 1;
	    factor = (int)(switching_f/control_f);
		return 0;


}
//-----------------------------------------------------------------------------
int32_t boostControl_BF_MPC_GetParams(void *in, uint32_t insize, void *out, uint32_t maxoutsize){

	 float *p = (float *)out;

	    *p++ = L;
	    *p++ = C;
	    *p++ = i_l_min;
	    *p++ = i_l_max;
	    *p++ = control_f;
	    *p++ = alpha;
	    *p++ = alpha_l;
	    return 28;
}

//-----------------------------------------------------------------------------
int32_t boostControl_BF_MPC_Run(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    boostConfigMeasurements_t *m = (boostConfigMeasurements_t *)meas;
    boostConfigReferences_t *r = (boostConfigReferences_t *)refs;
    boostConfigControl_t *o = (boostConfigControl_t *)outputs;

    //writing factor only once
        if (first_time_factor) {
        	boostHwSetAdcDoneIntFactor(factor);
        	first_time_factor = 0;
        }

    //updating rho according to factor
	//rho_1 = rho;
	if (mpc_flag){
		rho_1 = rho;
		mpc_flag = 0;
	}

//values from sensors
    v_i = m->v_dc_in;
    v_o = m->v_out;
    i_o = m->i_o;
//reference
    v_o_ref = r->v_o;

//controller equations
    d = ((rho_1*L/v_i) + v_o - v_i)/v_o;

//output power
	p_o = i_o*v_o;

    //if( d >= 0.98f ) d = 0.98f;
    //if( d <= 0.0f ) d = 0.0f;






    o->u = d;
    o->v_o_reference = r->v_o;
    o->p_o = p_o;
    return sizeof(boostConfigControl_t);
}
//-----------------------------------------------------------------------------
void boostControl_BF_MPC_Reset(void){

    d = 0.0f;
	rho = 0.0f;
    rho_1 = 0.0f;
    e = 0.0f;
    e_dot = 0.0f;
    i_o_filt = 0.0f;
    i_l_ref = 0.0f;
    e_ref = 0.0f;



    resetArray_BF_MPC(xm, DMPC_CONFIG_NXM);
    resetArray_BF_MPC(xm_1, DMPC_CONFIG_NXM);
    resetArray_BF_MPC(dx, DMPC_CONFIG_NXM);
    resetArray_BF_MPC(u, DMPC_CONFIG_NU + DMPC_CONFIG_ND);
    resetArray_BF_MPC(u_1, DMPC_CONFIG_NU + DMPC_CONFIG_ND);
    resetArray_BF_MPC(du, DMPC_CONFIG_NU + DMPC_CONFIG_ND);

    boostHwSetAdcDoneIntFactor(0);

}

//-----------------------------------------------------------------------------
static float boostHwExpMovAvg(float sample, float average);
static float boostHwExpMovAvg_l(float sample, float average);
//------------------------------------------------------------------------------

static float boostHwExpMovAvg(float sample, float average){

    return alpha * sample + (1.0f - alpha) * average;
}

static float boostHwExpMovAvg_l(float sample, float average){

    return alpha_l * sample + (1.0f - alpha_l) * average;
}
//-----------------------------------------------------------------------------
// mpc_rho functions

int32_t boostControl_BF_MPC_rho_Run(void *meas, void *outputs){

    boostConfigMeasurements_t *m = (boostConfigMeasurements_t *)meas;
    boostConfigControl_t *o = (boostConfigControl_t *)outputs;

//values from sensors
    i_l = m->i_l;
    i_o = m->i_o;
    v_i = m->v_dc_in;
    v_o = m->v_out;



// Initialize i_l_filt
    if (first_run_i_l){
    	i_l_filt = i_l;
    	i_o_filt = i_o;
    	first_run_i_l = 0;
    }
    else{
    	i_l_filt = boostHwExpMovAvg_l(i_l, i_l_filt);
    	i_o_filt = boostHwExpMovAvg(i_o, i_o_filt);
    }

//Reference translation from v_o_ref to e_ref
    e = (i_l_filt*i_l_filt*L/2) + (v_o*v_o*C/2); //(i_l*i_l*L/2) + (v_o*v_o*C/2); //
    e_dot = (i_l_filt*v_i) - (i_o_filt*v_o); //(i_l*v_i) - (i_o_filt*v_o); //
    i_l_ref =  i_o_filt*v_o/v_i; //i_o*v_o/v_i;
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
    e_dot_min = (v_i*i_l_min) - (v_o*i_o_filt);
    e_dot_max = (v_i*i_l_max) - (v_o*i_o_filt);

    DMPC_CONFIG_U_MIN[0] = u_min;
    DMPC_CONFIG_U_MAX[0] = u_max;
    DMPC_CONFIG_XM_MIN[0] = e_dot_min;
    DMPC_CONFIG_XM_MAX[0] = e_dot_max;

    // Initialize xm_1 with the value of xm for the first time the controller is run
    if (first_run) {
       for (int i = 0; i < DMPC_CONFIG_NXM; i++) {
           xm_1[i] = xm[i];
       }
       first_run = 0;
    }


/* Optimization */
    dmpcOpt(xm, xm_1, ref_mpc, u, &n_iters_mpc, du);

/* Computes u = du + u_1 */
    sumv(u, du, DMPC_CONFIG_NU, u);

/* Saves variables */
    for(int i = 0; i < DMPC_CONFIG_NXM; i++){
    	xm_1[i] = xm[i];
    }

    rho = 1e6*u[0]; //gain required of 1e6

// Outputs
    o->e = e;
    o->e_dot = e_dot;
    o->i_o_filt = i_o_filt;
    o->i_l_filt = i_l_filt;
    o->e_reference = e_ref;
    o->rho = rho;

    return sizeof(boostConfigControl_t);
}


//=============================================================================
void resetArray_BF_MPC(float *array, int size) {
    for (int i = 0; i < size; i++) {
        array[i] = 0.0f;
    }
}
//-----------------------------------------------------------------------------
void set_mpc_flag() {
    mpc_flag = 1;
}



//-----------------------------------------------------------------------------


#endif /* SOC_CPU1 */
