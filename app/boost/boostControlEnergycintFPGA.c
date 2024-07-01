#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "boostControlEnergycintFPGA.h"
#include "ocpConfig.h"
#include "ocpTrace.h"

#include "boostConfig.h"
#include "xboostcontrol.h"
#include "xboostsfb.h"
#include "xparameters.h"
#include "boostHw.h"

#include <stdlib.h>
//=============================================================================
/*----------------------------- Definitions ---------------------------------*/
//=============================================================================
#define ts								1.0e-5f

#define FREQ_SPLIT_FACTOR 				1

#define XPAR_XBOOSTCONTROL_0_DEVICE_ID 	0

// Shifting factors determined by calculating b-a in Fixed Point format ap_fixed<a,b> for Vitis HLS
#define FIXED_MATH_FTOI_GAINS		((float)(1 << 0))
#define FIXED_MATH_FTOI_STATE1		((float)(1 << 30))
#define FIXED_MATH_FTOI_STATE2		((float)(1 << 18))
#define FIXED_MATH_FTOI_STATE3		((float)(1 << 3))

#define ftoiGain1( a )				((int64_t) (a * FIXED_MATH_FTOI_GAINS))
#define ftoiGain2( a )				((int32_t) (a * FIXED_MATH_FTOI_GAINS))
#define ftoiGain3( a )				((int32_t) (a * FIXED_MATH_FTOI_GAINS))
#define ftoiState1( a )				((int32_t) (a * FIXED_MATH_FTOI_STATE1))
#define ftoiState2( a )				((int32_t) (a * FIXED_MATH_FTOI_STATE2))
#define ftoiState3( a )				((int32_t) (a * FIXED_MATH_FTOI_STATE3))

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================

static float k1 = 0.0f;
static float k2 = 0.0f;
static float k3 = 0.0f;
static float Li = 0.0f;
static float Co = 0.0f;

static float errorIntegral = 0.0f;
static float prevError = 0.0f;

static short controlCounter = 0;
static short init = 1;

static float min_v_in = BOOST_CONFIG_V_DC_IN_OFFS;
static float min_v_out = BOOST_CONFIG_V_DC_OUT_OFFS;
static float adc_gain_v_in_inv = BOOST_CONFIG_V_DC_IN_GAIN;
static float adc_gain_v_out_inv = BOOST_CONFIG_V_DC_OUT_GAIN;

static float min_i_l = BOOST_CONFIG_IL_OFFS;
static float min_i_o = BOOST_CONFIG_IO_AVG_OFFS;
static float adc_gain_i_l_inv = BOOST_CONFIG_IL_GAIN;
static float adc_gain_i_o_inv = BOOST_CONFIG_IO_AVG_GAIN;

static XBoostcontrol HlsBoostcontrol;
static XBoostcontrol_Config *BoostcontrolPtr = NULL;
static XBoostsfb HlsBoostsfb;
static XBoostsfb_Config *BoostsfbPtr = NULL;

//=============================================================================
//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void boostControlEnergycintFPGAInitialize(void){

	BoostcontrolPtr = XBoostcontrol_LookupConfig(XPAR_XBOOSTCONTROL_0_DEVICE_ID);
	if (!BoostcontrolPtr) {
		print("ERROR: Lookup of accelerator configuration failed.\n\r");
		return XST_FAILURE;
	}
	int status = XBoostcontrol_CfgInitialize(&HlsBoostcontrol, BoostcontrolPtr);
		if (status != XST_SUCCESS) {
		print("ERROR: Could not initialize accelerator.\n\r");
		exit(-1);
	}

	BoostsfbPtr = XBoostsfb_LookupConfig(XPAR_XBOOSTSFB_0_DEVICE_ID);
	if (!BoostsfbPtr) {
		print("ERROR: Lookup of accelerator configuration failed.\n\r");
		return XST_FAILURE;
	}
    status = XBoostsfb_CfgInitialize(&HlsBoostsfb, BoostsfbPtr);
		if (status != XST_SUCCESS) {
		print("ERROR: Could not initialize accelerator.\n\r");
		exit(-1);
	}
}
//-----------------------------------------------------------------------------
int32_t boostControlEnergycintFPGASetParams(void *params, uint32_t n){

    float *p = (float *)params;

    Li = *p++;
    Co = *p++;
    k1 = *p++;
    k2 = *p++;
    k3 = *p++;
	return 0;
}
//-----------------------------------------------------------------------------
int32_t boostControlEnergycintFPGAGetParams(void *in, uint32_t insize, void *out, uint32_t maxoutsize){

    float *p = (float *)out;

    *p++ = Li;
    *p++ = Co;
    *p++ = k1;
    *p++ = k2;
    *p++ = k3;


    return 20;
}

//-----------------------------------------------------------------------------
int32_t boostControlEnergycintFPGARun(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

	if ( init == 1 )
	{
		XBoostcontrol_Set_Li(&HlsBoostcontrol, *((u32*)&Li));
		XBoostcontrol_Set_min_v_in(&HlsBoostcontrol, *((u32*)&min_v_in));
		XBoostcontrol_Set_min_v_out(&HlsBoostcontrol, *((u32*)&min_v_out));
		XBoostcontrol_Set_adc_gain_v_in_inv(&HlsBoostcontrol, *((u32*)&adc_gain_v_in_inv));
		XBoostcontrol_Set_adc_gain_v_out_inv(&HlsBoostcontrol, *((u32*)&adc_gain_v_out_inv));
		//boostHwSetPwmBypass(0);
		init = 0;
	}

	controlCounter++;

    boostConfigMeasurements_t *m = (boostConfigMeasurements_t *)meas;
    boostConfigReferences_t *r = (boostConfigReferences_t *)refs;
    boostConfigControl_t *o = (boostConfigControl_t *)outputs;

    // Do not use adc_gain_inv and min values when testing on hardware !!!
    float i_in_conv = (m->i_l) * adc_gain_i_l_inv + min_i_l;
	float i_out_conv = (m->i_o) * adc_gain_i_o_inv + min_i_o;
	float v_out_conv = (m->v_dc_out) * adc_gain_v_out_inv + min_v_out;
	float v_in_conv = (m->v_dc_in) * adc_gain_v_in_inv + min_v_in;
	float Po = v_out_conv * i_out_conv;

	float actualEnergy = 0.5f * ( Li * i_in_conv * i_in_conv + Co * v_out_conv * v_out_conv );
	float energySetPoint = 0.5f * ( Li * (Po / v_in_conv) * (Po / v_in_conv) + Co * (r->v_o) * (r->v_o) );
	float energyDerivative = v_in_conv * i_in_conv - Po;

	//----------------- PiL ----------------------------------------
	uint16_t v_in_u16 = (uint16_t)(m->v_dc_in);
	uint16_t v_out_u16 = (uint16_t)(m->v_dc_out);
	XBoostcontrol_Set_v_in(&HlsBoostcontrol, *((u32*)&v_in_u16));
	XBoostcontrol_Set_v_out(&HlsBoostcontrol, *((u32*)&v_out_u16));
	//--------------------------------------------------------------

	// Integral approximation using Tustin transform: x[n] = x[n-1] + (T/2)*(e[n] + e[n-1])
	// Simulation of slow controller with f_ctrl = f_s / FREQ_SPLIT_FACTOR
	if (controlCounter == FREQ_SPLIT_FACTOR)
	{
		errorIntegral += (FREQ_SPLIT_FACTOR * ts / 2) * (actualEnergy - energySetPoint + prevError);
		prevError = actualEnergy - energySetPoint;

		updateStates( errorIntegral, actualEnergy, energyDerivative );
		controlCounter = 0;
	}

	//----------------- PiL ----------------------------------------
	uint32_t D_u32;
	D_u32 = XBoostcontrol_Get_D(&HlsBoostcontrol);
	o->u = *((float*)&D_u32);
	o->v_o_reference = r->v_o;
	//--------------------------------------------------------------

    return sizeof(boostConfigControl_t);
}

void updateStates( float state1, float state2, float state3 )
{
	int64_t gain1_fixed = ftoiGain1(k1);
	int32_t gain2_fixed = ftoiGain2(k2);
	int32_t gain3_fixed = ftoiGain3(k3);
	int32_t state1_fixed = ftoiState1(state1);
	int32_t state2_fixed = ftoiState2(state2);
	int32_t state3_fixed = ftoiState3(state3);

	XBoostsfb_Set_gain1( &HlsBoostsfb, *((u64*)&gain1_fixed) );
	XBoostsfb_Set_gain2( &HlsBoostsfb, *((u32*)&gain2_fixed) );
	XBoostsfb_Set_gain3( &HlsBoostsfb, *((u32*)&gain3_fixed) );
	XBoostsfb_Set_state1( &HlsBoostsfb, *((u32*)&state1_fixed) );
	XBoostsfb_Set_state2( &HlsBoostsfb, *((u32*)&state2_fixed) );
	XBoostsfb_Set_state3( &HlsBoostsfb, *((u32*)&state3_fixed) );

	XBoostsfb_Start(&HlsBoostsfb);
}

//-----------------------------------------------------------------------------
void boostControlEnergycintFPGAReset(void){
	errorIntegral = 0.0f;
	prevError = 0.0f;
	controlCounter = 0;
	init = 1;
}
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* SOC_CPU1 */
