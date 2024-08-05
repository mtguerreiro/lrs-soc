#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "buckControlSfbIntFPGA.h"
#include "ocpConfig.h"
#include "ocpTrace.h"

#include "buckConfig.h"
#include "xbucksfb.h"
#include "xparameters.h"
#include "buckHw.h"

#include <stdlib.h>
//=============================================================================
/*----------------------------- Definitions ---------------------------------*/
//=============================================================================
#define ts								1.0e-5f

// Shifting factors determined by calculating a-b in Fixed Point format ap_fixed<a,b> for Vitis HLS
// Must be adjusted if precision is changed in Vitis HLS !!!
#define FIXED_MATH_FTOI_GAIN1		((float)(1 << 0))
#define FIXED_MATH_FTOI_GAIN2		((float)(1 << 16))
#define FIXED_MATH_FTOI_GAIN3		((float)(1 << 16))
#define FIXED_MATH_FTOI_ADC_GAIN_V  ((float)(1 << 20))
#define FIXED_MATH_FTOI_ADC_GAIN_I	((float)(1 << 20))
#define FIXED_MATH_FTOI_ADC_MIN_V	((float)(1 << 20))
#define FIXED_MATH_FTOI_ADC_MIN_I	((float)(1 << 16))
#define FIXED_MATH_FTOI_VREF		((float)(1 << 16))
#define FIXED_MATH_FTOI_TS			((uint32_t)(1 << 31))

#define ftoiGain1( a )				((int32_t) (a * FIXED_MATH_FTOI_GAIN1))
#define ftoiGain2( a )				((int32_t) (a * FIXED_MATH_FTOI_GAIN2))
#define ftoiGain3( a )				((int32_t) (a * FIXED_MATH_FTOI_GAIN3))
#define ftoiAdcGainV( a )			((int32_t) (a * FIXED_MATH_FTOI_ADC_GAIN_V))
#define ftoiAdcGainI( a )			((int32_t) (a * FIXED_MATH_FTOI_ADC_GAIN_I))
#define ftoiAdcMinV( a )			((int32_t) (a * FIXED_MATH_FTOI_ADC_MIN_V))
#define ftoiAdcMinI( a )			((int32_t) (a * FIXED_MATH_FTOI_ADC_MIN_I))
#define ftoiVref( a )				((int32_t) (a * FIXED_MATH_FTOI_VREF))
#define ftoiTs( a )					((int32_t) (a * FIXED_MATH_FTOI_TS))

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================

static float k1 = 0.0f;
static float k2 = 0.0f;
static float k3 = 0.0f;
static float Li = 0.0f;
static float Co = 0.0f;

static float min_v_in = BUCK_CONFIG_V_DC_IN_OFFS;
static float min_v_out = BUCK_CONFIG_V_DC_OUT_OFFS;
static float adc_gain_v_in_inv = BUCK_CONFIG_V_DC_IN_GAIN;
static float adc_gain_v_out_inv = BUCK_CONFIG_V_DC_OUT_GAIN;

static float min_i_l = BUCK_CONFIG_IL_OFFS;
static float min_i_o = BUCK_CONFIG_IO_AVG_OFFS;
static float adc_gain_i_l_inv = BUCK_CONFIG_IL_GAIN;
static float adc_gain_i_o_inv = BUCK_CONFIG_IO_AVG_GAIN;

static float tsample = ts;

static XBucksfb HlsBucksfb;
static XBucksfb_Config *BucksfbPtr = NULL;

//=============================================================================
//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void buckControlSfbIntFPGAInitialize(void){

	BucksfbPtr = XBucksfb_LookupConfig(XPAR_XBUCKSFB_0_DEVICE_ID);
	if (!BucksfbPtr) {
		print("ERROR: Lookup of accelerator configuration failed.\n\r");
		return XST_FAILURE;
	}
    int status = XBucksfb_CfgInitialize(&HlsBucksfb, BucksfbPtr);
		if (status != XST_SUCCESS) {
		print("ERROR: Could not initialize accelerator.\n\r");
		exit(-1);
	}
}
//-----------------------------------------------------------------------------
int32_t buckControlSfbIntFPGASetParams(void *params, uint32_t n){

    float *p = (float *)params;

    Li = *p++;
    Co = *p++;
    k1 = *p++;
    k2 = *p++;
    k3 = *p++;
	return 0;
}
//-----------------------------------------------------------------------------
int32_t buckControlSfbIntFPGAGetParams(void *in, uint32_t insize, void *out, uint32_t maxoutsize){

    float *p = (float *)out;

    *p++ = Li;
    *p++ = Co;
    *p++ = k1;
    *p++ = k2;
    *p++ = k3;


    return 20;
}

//-----------------------------------------------------------------------------
int32_t buckControlSfbIntFPGARun(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

	// Controller not working properly on Hardware when setting Bypass in the reset function
	buckHwSetPwmBypass(0);

    buckConfigMeasurements_t *m = (buckConfigMeasurements_t *)meas;
    buckConfigReferences_t *r = (buckConfigReferences_t *)refs;
    buckConfigControl_t *o = (buckConfigControl_t *)outputs;

    int32_t gain1_fixed = ftoiGain1(k1);
    int32_t gain2_fixed = ftoiGain2(k2);
    int32_t gain3_fixed = ftoiGain3(k3);

    int32_t Vref_fixed = ftoiVref(r->v_o);

    XBucksfb_Set_gain1( &HlsBucksfb, *((u32*)&gain1_fixed) );
    XBucksfb_Set_gain2( &HlsBucksfb, *((u32*)&gain2_fixed) );
    XBucksfb_Set_gain3( &HlsBucksfb, *((u32*)&gain3_fixed) );

    XBucksfb_Set_v_ref( &HlsBucksfb, *((u32*)&Vref_fixed) );

	//----------------- PiL ----------------------------------------
	//uint16_t v_in_u16 = (uint16_t)(m->v_dc_in);
	//uint16_t v_out_u16 = (uint16_t)(m->v_dc_out);
	//uint16_t i_l_u16 = (uint16_t)(m->i_l);
	//XBucksfb_Set_v_in(&HlsBucksfb, *((u32*)&v_in_u16));
	//XBucksfb_Set_v_out(&HlsBucksfb, *((u32*)&v_out_u16));
	//XBucksfb_Set_i_l(&HlsBucksfb, *((u32*)&i_l_u16));
	//--------------------------------------------------------------

	XBucksfb_Start(&HlsBucksfb);
	//----------------- PiL ----------------------------------------
	//uint32_t D_u32;
	//D_u32 = XBucksfb_Get_D(&HlsBucksfb);
	//o->u = *((float*)&D_u32);
	//--------------------------------------------------------------
	//o->v_o_reference = r->v_o;

    return sizeof(buckConfigControl_t);
}

//-----------------------------------------------------------------------------
void buckControlSfbIntFPGAReset(void){
	int32_t adcGainVin_fixed = ftoiAdcGainV(adc_gain_v_in_inv);
	int32_t adcMinVin_fixed = ftoiAdcMinV(min_v_in);
	int32_t adcGainVout_fixed = ftoiAdcGainV(adc_gain_v_out_inv);
	int32_t adcMinVout_fixed = ftoiAdcMinV(min_v_out);
	int32_t adcGainIl_fixed = ftoiAdcGainI(adc_gain_i_l_inv);
	int32_t adcMinIl_fixed = ftoiAdcMinI(min_i_l);
	int32_t ts_fixed = ftoiTs(tsample);

	//Reset error integral in IP-Core
	XBucksfb_Set_reset(&HlsBucksfb, 0x1);
	XBucksfb_Start(&HlsBucksfb);
	XBucksfb_Set_reset(&HlsBucksfb, 0x0);

	XBucksfb_Set_min_v_in(&HlsBucksfb, *((u32*)&adcMinVin_fixed));
	XBucksfb_Set_min_v_out(&HlsBucksfb, *((u32*)&adcMinVout_fixed));
	XBucksfb_Set_min_i_l(&HlsBucksfb, *((u32*)&adcMinIl_fixed));
	XBucksfb_Set_adc_gain_v_in_inv(&HlsBucksfb, *((u32*)&adcGainVin_fixed));
	XBucksfb_Set_adc_gain_v_out_inv(&HlsBucksfb, *((u32*)&adcGainVout_fixed));
	XBucksfb_Set_adc_gain_i_l_inv(&HlsBucksfb, *((u32*)&adcGainIl_fixed));
	XBucksfb_Set_ts( &HlsBucksfb, *((u32*)&ts_fixed) );
	//buckHwSetPwmBypass(0);
}
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* SOC_CPU1 */
