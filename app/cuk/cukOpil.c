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

    int32_t size;

    uint8_t *src, *dst;

    dst = (uint8_t *)( meas );
    src = (uint8_t *)( &xtMeasurements );
    size = sizeof(stypesMeasurements_t);

    while(size--) *dst++ = *src++;

    return sizeof(stypesMeasurements_t);
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
void cukOpilDisable(void){

    xtControl.d = 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
