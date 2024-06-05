/*
 * boostOpil.c
 *
 *  Created on: 11.09.2023
 *      Author: LRS
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "boostOpil.h"

#include "config/stypesBoost.h"
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
int32_t boostOpilUpdateMeasurements(void *meas, int32_t size){

	uint8_t *src, *dst;

	dst = (uint8_t *)( &xtMeasurements );
	src = (uint8_t *)( meas );

	while(size--) *dst++ = *src++;

	return 0;
}
//-----------------------------------------------------------------------------
int32_t boostOpilGetMeasurements(void *meas){

    int32_t size;

    uint8_t *src, *dst;

    dst = (uint8_t *)( meas );
    src = (uint8_t *)( &xtMeasurements );
    size = sizeof(stypesMeasurements_t);

    while(size--) *dst++ = *src++;

    return sizeof(boostConfigMeasurements_t);
}
//-----------------------------------------------------------------------------
int32_t boostOpilUpdateSimData(void *simData, int32_t size){

	uint8_t *src, *dst;

	dst = (uint8_t *)( &xtSimData );
	src = (uint8_t *)( simData );

	while(size--) *dst++ = *src++;

	return 0;
}
//-----------------------------------------------------------------------------
int32_t boostOpilGetSimData(void **simData, int32_t size){

    *simData = (void *)( &xtSimData );

    return sizeof(stypesSimData_t);
}
//-----------------------------------------------------------------------------
int32_t boostOpilUpdateControl(void *control, int32_t size){

    uint8_t *src, *dst;
    int32_t n;

    dst = (uint8_t *)( &xtControl );
    src = (uint8_t *)( control );

    n = size;
    while(n--) *dst++ = *src++;

    return size;
}
//-----------------------------------------------------------------------------
int32_t boostOpilGetControl(void **control){

	*control = (void *)( &xtControl );

	return sizeof(stypesControl_t);
}
//-----------------------------------------------------------------------------
void boostOpilUpdateControllerData(void *data, int32_t size){

    uint8_t *src, *dst;

    dst = (uint8_t *)( &xtControllerData );
    src = (uint8_t *)( data );

    while(size--) *dst++ = *src++;
}
//-----------------------------------------------------------------------------
int32_t boostOpilGetControllerData(void **controllerData){

	*controllerData = (void *)( &xtControllerData );

	return sizeof(stypesControllerData_t);
}
//-----------------------------------------------------------------------------
int32_t boostOpilProcInputs(void *inputs, void *procinputs, int32_t size){

    uint8_t *src, *dst;
    int32_t n;

    dst = (uint8_t *)( procinputs );
    src = (uint8_t *)( inputs );
    n = size;

    while(n--) *dst++ = *src++;

    return size;
}
//-----------------------------------------------------------------------------
int32_t boostOpilProcOutputs(void *outputs, void *procoutputs, int32_t size){

    uint8_t *src, *dst;
    int32_t n;

    dst = (uint8_t *)( procoutputs );
    src = (uint8_t *)( outputs );
    n = size;

    while(n--) *dst++ = *src++;

    return size;
}
//-----------------------------------------------------------------------------
void boostOpilSetPwmDuty(float duty){

    xtControl.u = duty;
}
//-----------------------------------------------------------------------------
float boostOpilGetPwmDuty(void){

    return xtControl.u;
}
//-----------------------------------------------------------------------------
void boostOpilDisable(void){

    xtControl.u = 0.0f;
//    xtControllerData.sw_o = 0;
//    xtControllerData.sw_l = 0;
}
//-----------------------------------------------------------------------------
void boostOpilSetLoadSwitch(float state){

	//xtControllerData.input_relay = state;
//    xtControllerData.sw_l = state;
}
//-----------------------------------------------------------------------------
float boostOpilGetLoadSwitch(void){

//    return xtControllerData.sw_l;
}
//-----------------------------------------------------------------------------
void boostOpilSetOutputSwitch(float state){

//    xtControllerData.sw_o = state;
}
//-----------------------------------------------------------------------------
float boostOpilGetOutputSwitch(void){

//    return xtControllerData.sw_o;
}
//-----------------------------------------------------------------------------
//=============================================================================
