/*
 * buckOpil.c
 *
 *  Created on: 30 de ago de 2024
 *      Author: LRS
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "buckOpil.h"

#include "config/stypes.h"
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

static buckOpilInit_t xcontrolInit = 0;
static buckOpilControlRun_t xcontrolRun = 0;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void buckOpilInitialize(
        buckOpilInit_t controlInit, buckOpilControlRun_t controlRun
		){

	xcontrolInit = controlInit;
	xcontrolRun = controlRun;
}
//-----------------------------------------------------------------------------
int32_t buckOpilUpdateMeasurements(void *meas, int32_t size){

	uint8_t *src, *dst;

	dst = (uint8_t *)( &xtMeasurements );
	src = (uint8_t *)( meas );

	while(size--) *dst++ = *src++;

	return 0;
}
//-----------------------------------------------------------------------------
int32_t buckOpilGetMeasurements(void *meas){

    int32_t size;

    uint8_t *src, *dst;

    dst = (uint8_t *)( meas );
    src = (uint8_t *)( &xtMeasurements );
    size = sizeof(stypesMeasurements_t);

    while(size--) *dst++ = *src++;

    return sizeof(stypesMeasurements_t);
}
//-----------------------------------------------------------------------------
int32_t buckOpilUpdateSimData(void *simData, int32_t size){

	uint8_t *src, *dst;

	dst = (uint8_t *)( &xtSimData );
	src = (uint8_t *)( simData );

	while(size--) *dst++ = *src++;

	return 0;
}
//-----------------------------------------------------------------------------
int32_t buckOpilGetSimData(void **simData, int32_t size){

    *simData = (void *)( &xtSimData );

    return sizeof(stypesSimData_t);
}
//-----------------------------------------------------------------------------
void buckOpilInitializeControl(void){

	if( xcontrolInit ) xcontrolInit();
}
//-----------------------------------------------------------------------------
void buckOpilRunControl(void){

	if( xcontrolRun )
		xcontrolRun(
				(void *)&xtMeasurements,
				(void *)&xtSimData,
				(void *)&xtControl,
				(void *)&xtControllerData
				);
}
//-----------------------------------------------------------------------------
int32_t buckOpilUpdateControl(void *control, int32_t size){

    uint8_t *src, *dst;
    int32_t n;

    dst = (uint8_t *)( &xtControl );
    src = (uint8_t *)( control );

    n = size;
    while(n--) *dst++ = *src++;

    return size;
}
//-----------------------------------------------------------------------------
int32_t buckOpilGetControl(void **control){

	*control = (void *)( &xtControl );

	return sizeof(stypesControl_t);
}
//-----------------------------------------------------------------------------
void buckOpilUpdateControllerData(void *data, int32_t size){

    uint8_t *src, *dst;

    dst = (uint8_t *)( &xtControllerData );
    src = (uint8_t *)( data );

    while(size--) *dst++ = *src++;
}
//-----------------------------------------------------------------------------
int32_t buckOpilGetControllerData(void **controllerData){

	*controllerData = (void *)( &xtControllerData );

	return sizeof(stypesControllerData_t);
}
//-----------------------------------------------------------------------------
int32_t buckOpilProcInputs(void *inputs, void *procinputs, int32_t size){

    uint8_t *src, *dst;
    int32_t n;

    dst = (uint8_t *)( procinputs );
    src = (uint8_t *)( inputs );
    n = size;

    while(n--) *dst++ = *src++;

    return size;
}
//-----------------------------------------------------------------------------
int32_t buckOpilProcOutputs(void *outputs, void *procoutputs, int32_t size){

    uint8_t *src, *dst;
    int32_t n;

    dst = (uint8_t *)( procoutputs );
    src = (uint8_t *)( outputs );
    n = size;

    while(n--) *dst++ = *src++;

    return size;
}
//-----------------------------------------------------------------------------
//=============================================================================
