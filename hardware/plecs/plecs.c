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

	void *inputs;
	int32_t ninputs;

	void *outputs;
}plecsControl_t;

static plecsControl_t xplcontrol;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t plecsInitialize(void *inputs, int32_t ninputs, void *outputs){

	xplcontrol.inputs = inputs;
	xplcontrol.ninputs = ninputs;
	xplcontrol.outputs = outputs;

	return 0;
}
//-----------------------------------------------------------------------------
int32_t plecsGetInputs(void *inputs){

	int32_t k;
	float *src;
	float *dst;

	src = (float *)xplcontrol.inputs;
	dst = (float *)inputs;
	for( k = 0; k < xplcontrol.ninputs; k++ ) *dst++ = *src++;

	return xplcontrol.ninputs;
}
//-----------------------------------------------------------------------------
int32_t plecsProcInputs(void *inputs, void *procinputs, int32_t n){

	int32_t k;
	float *src;
	float *dst;

	src = (float *)inputs;
	dst = (float *)procinputs;
	for(k = 0; k < n; k++) *dst++ = *src++;

	return n;
}
//-----------------------------------------------------------------------------
int32_t plecsProcOutputs(void *outputs, void *procoutputs, int32_t n){

	int32_t k;
	float *src;
	float *dst;

	src = (float *)outputs;
	dst = (float *)procoutputs;
	for(k = 0; k < n; k++) *dst++ = *src++;

	return n;
}
//-----------------------------------------------------------------------------
int32_t plecsApplyOutputs(void *outputs, int32_t n){

	int32_t k;
	float *src;
	float *dst;

	src = (float *)outputs;
	dst = (float *)xplcontrol.outputs;
	for( k = 0; k < n; k++ ) *dst++ = *src++;

	return n;
}
//-----------------------------------------------------------------------------
//=============================================================================
