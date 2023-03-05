/*
 * pidctl.c
 *
 *  Created on: 10 de set de 2022
 *      Author: marco
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "pidctl.h"

#include "pid.h"
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static pid_t pid;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void pidctlInitialize(void){

	pidInitialize(&pid, 0);
}
//-----------------------------------------------------------------------------
int32_t pidctlSetParams(void *params, uint32_t n){

	uint32_t *p = (uint32_t *)params;

	pidConfig_t pidparams;

	pidparams.a1 = *((float *)(&p[0]));
	pidparams.a2 = *((float *)(&p[1]));
	pidparams.b0 = *((float *)(&p[2]));
	pidparams.b1 = *((float *)(&p[3]));
	pidparams.b2 = *((float *)(&p[4]));

	pidparams.saturate = *((uint32_t *)(&p[5]));
	pidparams.umax = *((float *)(&p[6]));
	pidparams.umin = *((float *)(&p[7]));

	pidSetParams(&pid, &pidparams);

	return 0;
}
//-----------------------------------------------------------------------------
int32_t pidctlGetParams(void *in, uint32_t insize, void *out, uint32_t maxoutsize){

	int32_t n;
	uint32_t *p;
	uint32_t *pout = (uint32_t *)out;
	pidConfig_t params;

	pidGetParams(&pid, &params);

	/* We assume that the size of params is 32-bit aligned */
	n = sizeof(params) >> 2;
	if( n > maxoutsize ) return 0;

	p = (uint32_t *)&params;
	while(n--){
		*pout++ = *p++;
	}

	return sizeof(params) >> 2;
}
//-----------------------------------------------------------------------------
int32_t pidctlRun(void *inputs, int32_t ninputs, void *outputs, int32_t nmaxoutputs){

	float e, u;
	float *p = (float *)inputs;
	float *o = (float *)outputs;

	e = *p;

	u = pidRun(&pid, e);

	*o = u;

	return 1;
}
//-----------------------------------------------------------------------------
//=============================================================================
