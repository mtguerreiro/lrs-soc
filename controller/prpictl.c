/*
 * prpictl.c
 *
 *  Created on: 25 de março de 2023
 *      Author: marco
 */

#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "prpictl.h"

#include "afe.h"

#include "ocpConfig.h"
#include "ocpTrace.h"

#include <math.h>
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
uint32_t precharge = 0;
uint32_t enable = 0;
uint32_t error = 0;

float ts = 1/5e3;
float h = 1.0; // frequency component to control


float e = 0.0, e_1 = 0.0;
float u_pi = 0.0, u_pi_1 = 0.0;

float ig_ref;

float a1, b0, b1;

//float Kp = 0.01, Ki = 1.0; //for sure working
float Kp = 0.4, Ki = 15.0; // for sure working - fast

float v_dc_ref = 20.0;

//float Kp_pr = 0.1, Ki_pr = 10.0; // for sure working
float Kp_pr = 0.4, Ki_pr = 30.0; // for sure working - fast

float w0 = 2.0*3.141592653589793*50.0, wc = 15.0;
float a0_pr, a1_pr, a2_pr;
float b0_pr, b1_pr, b2_pr;

float u_pr = 0.0, u_pr_1 = 0.0, u_pr_2 = 0.0, ei = 0.0, ei_1 = 0.0, ei_2 = 0.0;

float vs_ref;
float vs_ref_norm;


float v_ac_peak;

float v_dc;
float v_ac;
float i_ac;
float i_load;

float duty;

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void prpictlInitialize(void){

	a1 = 1.0;
	b0 = 1.0 / 2.0 * (2.0 * Kp + ts * Ki);
	b1 = 1.0 / 2.0 * (ts * Ki - 2.0 * Kp);

	//b0_pr = ts*ts*w0*w0 + 4.0*ts*wc + 4.0;
	//
	//a1_pr = 4.0 * Ki_pr * ts * wc / b0_pr;
	//
	//b1_pr = (2.0 * ts*ts * w0*w0 - 8.0) / b0_pr;
	//b2_pr = (ts*ts*w0*w0 - 4.0*ts*wc + 4.0) / b0_pr;

	/* PR with limited bandwidth */
//	b0_pr = ts*ts*w0*w0 + 4.0*ts*wc + 4.0;
//	b1_pr = (2.0 * ts*ts * w0*w0 - 8.0);
//	b2_pr = (ts*ts*w0*w0 - 4.0*ts*wc + 4.0);
//
//
//	a1_pr = 4.0 * Ki_pr * ts * wc;
//	a2_pr = -a1_pr;

	/* PR with infinite gain */
	b0_pr = ts*ts*w0*w0 + 4.0;
	b1_pr = (2.0 * ts*ts * w0*w0 - 8.0);
	b2_pr = (ts*ts*w0*w0 + 4.0);

	a0_pr = 4*Ki_pr*ts;
	a1_pr = 0;
	a2_pr = -a0_pr;

//	b0_pr = Ki_pr*sinf(h*w0*ts)/(2.0*h*w0);
//	b1_pr = Ki_pr*0.0;
//	b2_pr = Ki_pr*(-sinf(h*w0*ts)/(2.0*h*w0));
//
//	a1_pr = -2.0*cosf(h*w0*ts);
//	a2_pr = 1.000000000000000;

	v_ac_peak = 10.0 * sqrtf(2.0);
	//v_ac_peak = 23;

	ocpTraceAddSignal(OCP_TRACE_1, (void *)&i_ac, "Grid current");
	ocpTraceAddSignal(OCP_TRACE_1, (void *)&v_ac, "Grid voltage");
	ocpTraceAddSignal(OCP_TRACE_1, (void *)&v_dc, "DC link");
	ocpTraceAddSignal(OCP_TRACE_1, (void *)&i_load, "Load current");
}
//-----------------------------------------------------------------------------
int32_t prpictlSetParams(void *params, uint32_t n){

	e = 0.0;
	e_1 = 0.0;

	u_pi = 0.0;
	u_pi_1 = 0.0;

	u_pr = 0.0;
	u_pr_1 = 0.0;
	u_pr_2 = 0.0;
	ei = 0.0;
	ei_1 = 0.0;
	ei_2 = 0.0;

	return 0;
}
//-----------------------------------------------------------------------------
int32_t prpictlGetParams(void *in, uint32_t insize, void *out, uint32_t maxoutsize){

	return 0;
}
//-----------------------------------------------------------------------------
int32_t prpictlRun(void *inputs, int32_t ninputs, void *outputs, int32_t nmaxoutputs){

	float *p = (float *)inputs;
	float *o = (float *)outputs;

	/* Get inputs */
	i_ac = *p++;
	v_ac = *p++;
	v_dc = *p++;
	i_load = *p;

	/* Checks if all is well */
	if( precharge == 0 ){
		if( v_dc > AFE_DC_LINK_PRECHARGE ){
			precharge = 1;
			afeInputRelaySet(1);
			afeOutputRelaySet(1);
		}
	}
	else{
		if( v_dc < AFE_DC_LINK_MIN ) error |= AFE_ERR_DCLINK_UNDER;
		if( v_dc > AFE_DC_LINK_MAX ) error |= AFE_ERR_DCLINK_OVER;
	}

	if( i_ac < AFE_HB_CURRENT_MIN ) error |= AFE_ERR_HB_CURRENT_UNDER;
	if( i_ac > AFE_HB_CURRENT_MAX ) error |= AFE_ERR_HB_CURRENT_OVER;

	if( v_ac < AFE_GRID_VOLTAGE_MIN ) error |= AFE_ERR_GRID_VOLTAGE_UNDER;
	if( v_ac  > AFE_GRID_VOLTAGE_MAX ) error |= AFE_ERR_GRID_VOLTAGE_OVER;

	if( i_load < AFE_LOAD_CURRENT_MIN ) error |= AFE_ERR_LOAD_CURRENT_UNDER;
	if( i_load > AFE_LOAD_CURRENT_MAX ) error |= AFE_ERR_LOAD_CURRENT_OVER;

	if( error != 0 ){
		enable = 0;
		afeInputRelaySet(0);
		afeOutputRelaySet(0);
		afePwmEnable(0);
	}
	else{
		if( precharge == 1 ){
			afeInputRelaySet(1);
			afeOutputRelaySet(1);
			afePwmEnable(1);
		}
	}

	//error = 1;
	/* Main control */
	if( (enable == 1) && (error == 0) ){

		e = v_dc_ref - v_dc;

		u_pi = a1 * u_pi_1 + b0 * e + b1 * e_1;
		if(u_pi > 30.0) u_pi = 30.0;
		else if ( u_pi < (-30.0) ) u_pi = -30.0;

		ig_ref = u_pi * (v_ac / v_ac_peak);

		ei = ig_ref - i_ac;

		//u_pr = (-b1_pr * u_pr_1 - b2_pr * u_pr_2 + Kp_pr * b0_pr * ei + (Kp_pr*b1_pr+a1_pr)*ei_1 + (Kp_pr*b2_pr+a2_pr)*ei_2) / b0_pr;
		u_pr = (-b1_pr * u_pr_1 - b2_pr * u_pr_2 + (Kp_pr*b0_pr+a0_pr) * ei + (Kp_pr*b1_pr)*ei_1 + (Kp_pr*b2_pr+a2_pr)*ei_2) / b0_pr;
		//u_pr = b0_pr*ei + b1_pr*ei_1 + b2_pr*ei_2 - a1_pr*u_pr_1 - a2_pr*u_pr_2 + Kp_pr*ts*ei;

		vs_ref = v_ac - u_pr;

		vs_ref_norm = vs_ref / v_ac_peak;
		if(vs_ref_norm >  1.0) vs_ref_norm = 1.0;
		if(vs_ref_norm < -1.0) vs_ref_norm = -1.0;
		duty = (vs_ref_norm + 1.0) / 2.0;

		u_pi_1 = u_pi;
		e_1 = e;

		ei_2 = ei_1;
		ei_1 = ei;

		u_pr_2 = u_pr_1;
		u_pr_1 = u_pr;
	}
	else{
		duty = 0.0f;
	}

	/* Set outputs */
	*o = duty;

	return 1;
}
//-----------------------------------------------------------------------------
//=============================================================================
#endif /* SOC_CPU1 */
