/*
 * cukOpil.c
 *
 *  Created on: 11.09.2023
 *      Author: LRS
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "cukOpil.h"

#include "config/stypesCuk.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static stypesMeasurements_t xtMeasurements;
static stypesSimData_t xtSimData;
static stypesControl_t xtControl;
static stypesControllerData_t xtControllerData;

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t cukOpilUpdateMeasurements(void *meas, int32_t size){

	uint8_t *src, *dst;

	dst = (uint8_t *)( &xtMeasurements );
	src = (uint8_t *)( meas );

	while(size--) *dst++ = *src++;

	return 0;
}
//-----------------------------------------------------------------------------
int32_t cukOpilGetMeasurements(void *meas){

    float *src;
    cukConfigMeasurements_t *dst;

    src = (float *)(&xtMeasurements);
    dst = (cukConfigMeasurements_t *)meas;

    dst->i_i =  ( (CUK_CONFIG_ADC_GAIN_INV * (*src++)) - CUK_CONFIG_ISENS_ACS712_OFFS ) * CUK_CONFIG_ISENS_ACS712_GAIN_INV;
    dst->i_1 =  ( (CUK_CONFIG_ADC_GAIN_INV * (*src++)) - CUK_CONFIG_ISENS_ACS730_OFFS ) * CUK_CONFIG_ISENS_ACS730_GAIN_INV;

    dst->v_in = ( CUK_CONFIG_ADC_GAIN_INV * (*src++) ) * CUK_CONFIG_VSENS_GAIN_INV;
    dst->v_dc = ( CUK_CONFIG_ADC_GAIN_INV * (*src++) ) * CUK_CONFIG_VSENS_GAIN_INV;
    dst->v_1  = ( CUK_CONFIG_ADC_GAIN_INV * (*src++) ) * CUK_CONFIG_VSENS_GAIN_INV;

    dst->i_o =  ( (CUK_CONFIG_ADC_GAIN_INV * (*src++)) - CUK_CONFIG_ISENS_ACS712_OFFS ) * CUK_CONFIG_ISENS_ACS712_GAIN_INV;
    dst->i_2 =  ( (CUK_CONFIG_ADC_GAIN_INV * (*src++)) - CUK_CONFIG_ISENS_ACS730_OFFS ) * CUK_CONFIG_ISENS_ACS730_GAIN_INV;

    dst->v_out =    ( CUK_CONFIG_ADC_GAIN_INV * (*src++) ) * CUK_CONFIG_VSENS_GAIN_INV;
    dst->v_dc_out = ( CUK_CONFIG_ADC_GAIN_INV * (*src++) ) * CUK_CONFIG_VSENS_GAIN_INV;
    dst->v_2 =      ( CUK_CONFIG_ADC_GAIN_INV * (*src++) ) * CUK_CONFIG_VSENS_GAIN_INV;

    dst->i_i_filt = 0.0f;
    dst->i_1_filt = 0.0f;

    dst->v_in_filt = 0.0f;
    dst->v_dc_filt = 0.0f;
    dst->v_1_filt  = 0.0f;

    dst->i_o_filt = 0.0f;
    dst->i_2_filt = 0.0f;

    dst->v_out_filt =    0.0f;
    dst->v_dc_out_filt = 0.0f;
    dst->v_2_filt =      0.0f;

//    int32_t size;
//
//    uint8_t *src, *dst;
//
//    dst = (uint8_t *)( meas );
//    src = (uint8_t *)( &xtMeasurements );
//    size = sizeof(stypesMeasurements_t);
//
//    while(size--) *dst++ = *src++;

    return sizeof(cukConfigMeasurements_t);
}
//-----------------------------------------------------------------------------
int32_t cukOpilUpdateSimData(void *simData, int32_t size){

	uint8_t *src, *dst;

	dst = (uint8_t *)( &xtSimData );
	src = (uint8_t *)( simData );

	while(size--) *dst++ = *src++;

	return 0;
}
//-----------------------------------------------------------------------------
int32_t cukOpilGetSimData(void **simData, int32_t size){

    *simData = (void *)( &xtSimData );

    return sizeof(stypesSimData_t);
}
//-----------------------------------------------------------------------------
int32_t cukOpilUpdateControl(void *control, int32_t size){

    uint8_t *src, *dst;
    int32_t n;

    dst = (uint8_t *)( &xtControl );
    src = (uint8_t *)( control );

    n = size;
    while(n--) *dst++ = *src++;

    return size;
}
//-----------------------------------------------------------------------------
int32_t cukOpilGetControl(void **control){

	*control = (void *)( &xtControl );

	return sizeof(stypesControl_t);
}
//-----------------------------------------------------------------------------
void cukOpilUpdateControllerData(void *data, int32_t size){

    uint8_t *src, *dst;

    dst = (uint8_t *)( &xtControllerData );
    src = (uint8_t *)( data );

    while(size--) *dst++ = *src++;
}
//-----------------------------------------------------------------------------
int32_t cukOpilGetControllerData(void **controllerData){

	*controllerData = (void *)( &xtControllerData );

	return sizeof(stypesControllerData_t);
}
//-----------------------------------------------------------------------------
int32_t cukOpilProcInputs(void *inputs, void *procinputs, int32_t size){

    uint8_t *src, *dst;
    int32_t n;

    dst = (uint8_t *)( procinputs );
    src = (uint8_t *)( inputs );
    n = size;

    while(n--) *dst++ = *src++;

    return size;
}
//-----------------------------------------------------------------------------
int32_t cukOpilProcOutputs(void *outputs, void *procoutputs, int32_t size){

    uint8_t *src, *dst;
    int32_t n;

    dst = (uint8_t *)( procoutputs );
    src = (uint8_t *)( outputs );
    n = size;

    while(n--) *dst++ = *src++;

    return size;
}
//-----------------------------------------------------------------------------
void cukOpilSetPwmDuty(float duty){

    xtControl.u = duty;
}
//-----------------------------------------------------------------------------
float cukOpilGetPwmDuty(void){

    return xtControl.u;
}
//-----------------------------------------------------------------------------
void cukOpilDisable(void){

    xtControl.u = 0.0f;
    xtControllerData.sw_o = 0;
    xtControllerData.sw_l = 0;
}
//-----------------------------------------------------------------------------
void cukOpilSetLoadSwitch(float state){

    xtControllerData.sw_l = state;
}
//-----------------------------------------------------------------------------
float cukOpilGetLoadSwitch(void){

    return xtControllerData.sw_l;
}
//-----------------------------------------------------------------------------
void cukOpilSetOutputSwitch(float state){

    xtControllerData.sw_o = state;
}
//-----------------------------------------------------------------------------
float cukOpilGetOutputSwitch(void){

    return xtControllerData.sw_o;
}
//-----------------------------------------------------------------------------
//=============================================================================
