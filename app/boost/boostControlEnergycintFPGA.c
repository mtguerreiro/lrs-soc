#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "boostControlEnergycintFPGA.h"
#include "ocpConfig.h"
#include "ocpTrace.h"

#include "boostConfig.h"
#include "xboostcontrol.h"
#include "xparameters.h"
#include "boostHw.h"

#include <stdlib.h>
//=============================================================================
/*----------------------------- Definitions ---------------------------------*/
//=============================================================================
#define ts					1.0e-5f

#define FREQ_SPLIT_FACTOR 	1

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

static int controlCounter = 0;
static float controllerOut = 0.0f;

static XBoostcontrol HlsBoostcontrol;
static XBoostcontrol_Config *BoostcontrolPtr;

//=============================================================================
#define XPAR_XBOOSTCONTROL_0_DEVICE_ID 0
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

	//boostHwSetPwmBypass(0);
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

	boostHwSetPwmBypass(0);

	float min_v_in_lval = BOOST_CONFIG_V_DC_IN_OFFS;
	float min_v_out_lval = BOOST_CONFIG_V_DC_OUT_OFFS;
	float adc_gain_v_in_inv_lval = BOOST_CONFIG_V_DC_IN_GAIN;
	float adc_gain_v_out_inv_lval = BOOST_CONFIG_V_DC_OUT_GAIN;
	XBoostcontrol_Set_Li(&HlsBoostcontrol, *((u32*)&Li));
	XBoostcontrol_Set_min_v_in(&HlsBoostcontrol, *((u32*)&min_v_in_lval));
	XBoostcontrol_Set_min_v_out(&HlsBoostcontrol, *((u32*)&min_v_out_lval));
	XBoostcontrol_Set_adc_gain_v_in_inv(&HlsBoostcontrol, *((u32*)&adc_gain_v_in_inv_lval));
	XBoostcontrol_Set_adc_gain_v_out_inv(&HlsBoostcontrol, *((u32*)&adc_gain_v_out_inv_lval));
	XBoostcontrol_Set_pwm_period(&HlsBoostcontrol, 500);

	controlCounter++;

    boostConfigMeasurements_t *m = (boostConfigMeasurements_t *)meas;
    boostConfigReferences_t *r = (boostConfigReferences_t *)refs;
    boostConfigControl_t *o = (boostConfigControl_t *)outputs;

    float i_in_conv = (m->i_l) * BOOST_CONFIG_IL_GAIN + BOOST_CONFIG_IL_OFFS;
	float i_out_conv = (m->i_o) * BOOST_CONFIG_IO_AVG_GAIN + BOOST_CONFIG_IO_AVG_OFFS;
	float v_out_conv = (m->v_dc_out) * BOOST_CONFIG_V_DC_OUT_GAIN + BOOST_CONFIG_V_DC_OUT_OFFS;
	float v_in_conv = (m->v_dc_in) * BOOST_CONFIG_V_DC_IN_GAIN + BOOST_CONFIG_V_DC_IN_OFFS;
	float Po = v_out_conv * i_out_conv;

	float actualEnergy = 0.5f * ( Li * i_in_conv * i_in_conv + Co * v_out_conv * v_out_conv );
	float energySetPoint = 0.5f * ( Li * (Po / v_in_conv) * (Po / v_in_conv) + Co * (r->v_o) * (r->v_o) );
	float energyDerivative = v_in_conv * i_in_conv - Po;

	uint16_t v_in_u16 = (uint16_t)(m->v_dc_in);
	uint16_t v_out_u16 = (uint16_t)(m->v_dc_out);
	XBoostcontrol_Set_v_in(&HlsBoostcontrol, *((u32*)&v_in_u16)); //PiL
	XBoostcontrol_Set_v_out(&HlsBoostcontrol, *((u32*)&v_out_u16)); //PiL

	// Integral approximation using Tustin transform: x[n] = x[n-1] + (T/2)*(e[n] + e[n-1])
	// Simulation of slow controller with f_ctrl = f_s / FREQ_SPLIT_FACTOR
	if (controlCounter == FREQ_SPLIT_FACTOR)
	{
		errorIntegral += (FREQ_SPLIT_FACTOR * ts / 2) * (actualEnergy - energySetPoint + prevError);
		prevError = actualEnergy - energySetPoint;

		controllerOut = - k1 * errorIntegral - k2 * actualEnergy - k3 * energyDerivative;
		updateLinearizationInput(controllerOut);
		controlCounter = 0;
	}

	XBoostcontrol_Start(&HlsBoostcontrol);
	uint32_t D_u32 = XBoostcontrol_Get_D_axi(&HlsBoostcontrol);
	o->u = *((float*)&D_u32); //PiL
	o->v_o_reference = r->v_o;  //PiL

    return sizeof(boostConfigControl_t);
}

void updateLinearizationInput(float controllerOut)
{
	XBoostcontrol_Set_controllerIn(&HlsBoostcontrol, *((u32*)&controllerOut));
}

//-----------------------------------------------------------------------------
void boostControlEnergycintFPGAReset(void){
	errorIntegral = 0.0f;
	prevError = 0.0f;
	controllerOut = 0.0f;
	controlCounter = 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* SOC_CPU1 */
