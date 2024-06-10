/*
 * cukControlEnergy.c
 *
 *  Created on: 11.09.2023
 *      Author: marco
 */

#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "cukControlEnergyMpc.h"

#include "ocpConfig.h"
#include "ocpTrace.h"

#include "cukConfig.h"
//#include "config/stypesCuk.h"

//#include "mvops.h"
//#include "dmpc.h"
//#include "dmpc_defs.h"
//#include "dmpc_matrices.h"
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static void cukControlEnergyMpcFilterReset(void);
static float cukControlEnergyMpcFilterRun(float x);
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{

    float a0;
    float a1;
    float a2;

    float b1;
    float b2;

    float x_1;
    float x_2;

    float y;
    float y_1;
    float y_2;

}filter_t;

#define CUK_CONFIG_TF_N2N1_SQ   (CUK_CONFIG_TF_N2N1 * CUK_CONFIG_TF_N2N1)
#define L_in_uH ((float)100.f)

#define i1_lim      6
#define i1_min      -i1_lim
#define i1_max      i1_lim
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static float u_out = 0.0f;
static float k1 = 37914191.17733706, k2 = 11200.000048725671, k3 = -98513528579.26212, dt = 1e-05;
static float vc = 0.0f;
static float p_in = 0.0f, p_out = 0.0f, y_dot = 0.0f, y = 0.0f, y_r = 0.0f;

static float y_e = 0.0f, y_e_1 = 0.0f;

//float xm[DMPC_CONFIG_NXM] = {0};
//float xm_1[DMPC_CONFIG_NXM] = {0};
//float dx[DMPC_CONFIG_NXM] = {0};
//
//float ref_mpc[DMPC_CONFIG_NY];
//
//float u[DMPC_CONFIG_NU + DMPC_CONFIG_ND] = {0};
//float u_1[DMPC_CONFIG_NU + DMPC_CONFIG_ND] = {0};
//float du[DMPC_CONFIG_NU + DMPC_CONFIG_ND] = {0};
//
//float aux1[DMPC_CONFIG_NXM], aux2[DMPC_CONFIG_NXM];

uint32_t n_iters;

float u_min;
float u_max;
float y_dot_min;
float y_dot_max;

uint32_t first_time = 1;

static filter_t notch = {.a0 = 1.0f, .a1 = -1.8954516649246216f, .a2 = 0.9887202978134155f,
        .b1 = -1.3892015218734741, .b2 = 0.48247018456459045f,
        .x_1 = 0.0f, .x_2 = 0.0f, .y = 0.0f, .y_1 = 0.0f, .y_2 = 0.0f
};

static float notchEnable = 0.0f;

static uint32_t mpcPeriod = 4;
static uint32_t mpcPeriodCounter = 0;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void cukControlEnergyMpcInitialize(void){

//    ocpTraceAddSignal(OCP_TRACE_1, &y, "Converter energy (MPC)");
//    ocpTraceAddSignal(OCP_TRACE_1, &y_r, "Energy reference (MPC)");

}
//-----------------------------------------------------------------------------
int32_t cukControlEnergyMpcSetParams(void *params, uint32_t n){

    float *p = (float *)params;

    notch.a0 = *p++;
    notch.a1 = *p++;
    notch.a2 = *p++;

    notch.b1 = *p++;
    notch.b2 = *p++;

    notchEnable = *p++;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t cukControlEnergyMpcGetParams(void *in, uint32_t insize, void *out, uint32_t maxoutsize){

    float *p = (float *)out;

    *p++ = notch.a0;
    *p++ = notch.a1;
    *p++ = notch.a2;

    *p++ = notch.b1;
    *p++ = notch.b2;

    *p++ = notchEnable;

    return 24;
}
//-----------------------------------------------------------------------------
int32_t cukControlEnergyMpcRun(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    cukConfigMeasurements_t *m = (cukConfigMeasurements_t *)meas;
    cukConfigReferences_t *r = (cukConfigReferences_t *)refs;
    cukConfigControl_t *o = (cukConfigControl_t *)outputs;

    static float x1, x2, x3, x4;
    static float e_x1, e_x2, e_x3, e_x4;
    static float x1_r, x2_r, x3_r, x4_r;
    static float e_x1_r, e_x2_r, e_x3_r, e_x4_r;
    static float v;

    x1 = m->i_1;
    x2 = m->i_2;
    x3 = m->v_1 + (1.0f / CUK_CONFIG_TF_N2N1) * m->v_2;
    x4 = m->v_dc_out;

    /* Energies */
    e_x1 = (0.5f) * CUK_CONFIG_L_IN * x1 * x1;
    e_x2 = (0.5f) * CUK_CONFIG_L_OUT * x2 * x2;
    e_x3 = (0.5f) * CUK_CONFIG_C_C * x3 * x3 * CUK_CONFIG_TF_N2N1_SQ / (CUK_CONFIG_TF_N2N1_SQ + 1.0f);
    e_x4 = (0.5f) * CUK_CONFIG_C_O * x4 * x4;
    y = e_x1 + e_x2 + e_x3 + e_x4;

    /* Input, output and converter power */
    p_in = m->p_in;
    p_out = m->p_out;
    y_dot = p_in - p_out;

    /* References */
    x4_r = r->v_o;
    x1_r = p_out / m->v_dc;
    x2_r = p_out / x4_r;
    x3_r = m->v_dc + x4_r * (1.0f / CUK_CONFIG_TF_N2N1);

    e_x1_r = (0.5f) * CUK_CONFIG_L_IN * x1_r * x1_r;
    e_x2_r = (0.5f) * CUK_CONFIG_L_OUT * x2_r * x2_r;
    e_x3_r = (0.5f) * CUK_CONFIG_C_C * x3_r * x3_r * CUK_CONFIG_TF_N2N1_SQ / (CUK_CONFIG_TF_N2N1_SQ + 1.0f);
    e_x4_r = (0.5f) * CUK_CONFIG_C_O * x4_r * x4_r;
    y_r = e_x1_r + e_x2_r + e_x3_r + e_x4_r;

//    /* Integrator */
//    y_e = y_e_1 + dt * (y_r - y);
//    y_e_1 = y_e;
//
//    /* Control */
//    v = k1 * y_r - k1 * y - k2 * y_dot - k3 * y_e;


//    if( first_time == 1 ){
//        first_time = 0;
//        xm_1[0] = y_r;
//    }

//    if( mpcPeriodCounter == 0 ){
//        /* Reference */
//        ref_mpc[0] = y_r;
//
//        /* Assembles state vector */
//        xm[0] = y;
//        xm[1] = y_dot;
//
//        /* */
//        u_min = (m->v_in - x3) * m->v_in / L_in_uH;
//        u_max = m->v_in * m->v_in / L_in_uH;
//
//        y_dot_min = m->v_in * i1_min - p_out;
//        y_dot_max = m->v_in * i1_max - p_out;
//
//        DMPC_CONFIG_U_MIN[0] = u_min;
//        DMPC_CONFIG_U_MAX[0] = u_max;
//
//        //DMPC_CONFIG_XM_MIN[0] = y_dot_min;
//        //DMPC_CONFIG_XM_MAX[0] = y_dot_max;
//
//        /* Delay compensation */
//        dmpcDelayComp(xm, xm, u);
//
//        /* Optimization */
//        dmpcOpt(xm, xm_1, ref_mpc, u, &n_iters, du);
//
//        /* Computes u = du + u_1 */
//        sumv(u, du, DMPC_CONFIG_NU, u);
//
//        /* Saves variables */
//        uint32_t i;
//        for(i = 0; i < DMPC_CONFIG_NXM; i++){
//            xm_1[i] = xm[i];
//        }
//    }
   mpcPeriodCounter++;
   if( mpcPeriodCounter >= mpcPeriod ) mpcPeriodCounter = 0;

//    v = u[0] * ((float)1e6);

    if( notchEnable > 0.5f ) v = cukControlEnergyMpcFilterRun(v);

    u_out =  1 - (m->v_dc * m->v_dc / CUK_CONFIG_L_IN - v) * CUK_CONFIG_L_IN / (x3 * m->v_dc);

    if( u_out > 1.0f ) u_out = 1.0f;
    if( u_out < 0.0f ) u_out = 0.0f;

    o->u = u_out;

    return sizeof(cukConfigControl_t);
}
//-----------------------------------------------------------------------------
void cukControlEnergyMpcReset(void){

    cukControlEnergyMpcFilterReset();

//    xm_1[0] = 0.0f;
//    xm_1[1] = 0.0f;

    y_e_1 = 0.0f;
    u_out = 0.0f;
    first_time = 1;

    mpcPeriodCounter = 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static void cukControlEnergyMpcFilterReset(void){

    notch.x_1 = 0.0f;
    notch.x_2 = 0.0f;

    notch.y = 0.0f;
    notch.y_1 = 0.0f;
    notch.y_2 = 0.0f;
}
//-----------------------------------------------------------------------------
static float cukControlEnergyMpcFilterRun(float x){

    notch.y = -notch.b1 * notch.y_1 - notch.b2 * notch.y_2 + \
            notch.a0 * x + notch.a1 * notch.x_1 + notch.a2 * notch.x_2;

    notch.y_2 = notch.y_1;
    notch.y_1 = notch.y;

    notch.x_2 = notch.x_1;
    notch.x_1 = x;

    return notch.y;
}
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* SOC_CPU1 */
