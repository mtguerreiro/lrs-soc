/*
 * plecs.c
 *
 *  Created on: 4 de mar de 2023
 *      Author: marco
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "plecs.h"

//============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{

	plecsGetInputs_t getInputs;
	plecsProcInputs_t procInputs;
	plecsProcOutputs_t procOutputs;
	plecsApplyOutputs_t applyOutputs;
}plecsControl_t;

static plecsControl_t xcontrol;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t plecsInitialize(plecsGetInputs_t getInputs, plecsProcInputs_t procInputs,
		plecsProcOutputs_t procOutputs, plecsApplyOutputs_t applyOutputs){

	xcontrol.getInputs = getInputs;
	xcontrol.procInputs = procInputs;
	xcontrol.procOutputs = procOutputs;
	xcontrol.applyOutputs = applyOutputs;

	return 0;
}
//-----------------------------------------------------------------------------
int32_t plecsGetInputs(void *inputs){

	return xcontrol.getInputs(inputs);
}
//-----------------------------------------------------------------------------
int32_t plecsProcInputs(void *inputs, void *procinputs, int32_t n){

	return xcontrol.procInputs(inputs, procinputs, n);
}
//-----------------------------------------------------------------------------
int32_t plecsProcOutputs(void *outputs, void *procoutputs, int32_t n){

	return xcontrol.procOutputs(outputs, procoutputs, n);
}
//-----------------------------------------------------------------------------
int32_t plecsApplyOutputs(void *outputs, int32_t n){

	return xcontrol.applyOutputs(outputs, n);
}
//-----------------------------------------------------------------------------
//=============================================================================
