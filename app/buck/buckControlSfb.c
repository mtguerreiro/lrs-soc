/*
 * buckSfbControl.c
 *
 *  Created on: 30 de ago. de 2023
 *      Author: marco
 */

#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "buckControlSfb.h"
#include "buckConfig.h"

#include "ocpConfig.h"
#include "ocpTrace.h"

//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define k1_raw           0.24279100529100536f
#define k2_raw           0.22154174015285136f
#define kr_raw           0.33185185185185195f
#define max_i            8.0f
#define min_i            0.0f
#define range_i          (max_i - min_i)
#define adc_res_i        4095.0f
#define adc_gain_i       (adc_res_i/range_i)
#define max_v_out        35.0f
#define min_v_out        0.0f
#define range_v_out      (max_v_out - min_v_out)
#define adc_res_v_out    4095.0f
#define adc_gain_v_out   (adc_res_v_out/range_v_out)
//#define k1               (k1_raw/adc_gain_i)
//#define k2               (k2_raw/adc_gain_v_out)
//#define kr               kr_raw
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
buckControlSfbParams_t ctl = {
        .kr = kr_raw,
        .k1 = (k1_raw/adc_gain_i),
        .k2 = (k2_raw/adc_gain_v_out),
        .r = 5
};
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void buckControlSfbInitialize(void){

	//ocpTraceAddSignal(OCP_TRACE_1, (void *)&i_ac, "Grid current");
}
//-----------------------------------------------------------------------------
int32_t buckControlSfbSetParams(void *params, uint32_t n){

    float *p = (float *)params;

    ctl.kr = *p++;
    ctl.k1 = *p++;
    ctl.k2 = *p++;

    ctl.r = *p;

	return 0;
}
//-----------------------------------------------------------------------------
int32_t buckControlSfbGetParams(void *in, uint32_t insize, void *out, uint32_t maxoutsize){

    float *p = (float *)out;

    *p++ = ctl.kr;
    *p++ = ctl.k1;
    *p++ = ctl.k2;

    *p = ctl.r;

	return sizeof(buckControlSfbParams_t);
}
//-----------------------------------------------------------------------------
int32_t buckControlSfbRun(void *inputs, int32_t ninputs, void *outputs, int32_t nmaxoutputs){

    buckConfigMeasurements_t *m = (buckConfigMeasurements_t *)inputs;
    buckConfigControlSignals_t *o = (buckConfigControlSignals_t *)outputs;

    float u;

    u = ctl.r * ctl.kr - m->i * ctl.k1 - m->v_out * ctl.k2;

    if( u < 0.0f )
        o->u = 0.0f;
    else if( u > 1.0f )
        o->u = 1.0f;
    else
        o->u = u;

    return sizeof(buckConfigControlSignals_t);
}
//-----------------------------------------------------------------------------
//=============================================================================
#endif /* SOC_CPU1 */
