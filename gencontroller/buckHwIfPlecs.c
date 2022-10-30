/*
 * buckHwIfPlecs.c
 *
 *  Created on: 30.10.2022
 *      Author: mguerreiro
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "buckHwIfPlecs.h"

#include "rp.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{
	rphandle_t handles[BUCK_HW_IF_PLECS_END];
	rpctx_t rp;
}buckHwIfPlecsIf_t;

typedef struct{

	int *measbuffer;
	int *outputbuffer;

	buckHwIfPlecsIf_t interface;

}buckHwIfPlecsControl_t;

buckHwIfPlecsControl_t buckPlecs;

//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static int32_t buckHwIfPlecsSetMeasBuffer(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckHwIfPlecsSetOutBuffer(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t buckHwIfPlecsInitialize(void){

	/* Initializes the request processor */
	rpInitialize(&buckPlecs.interface.rp, BUCK_HW_IF_PLECS_END, buckPlecs.interface.handles);
	rpRegisterHandle(&buckPlecs.interface.rp, BUCK_HW_IF_PLECS_SET_MEAS_BUFFER, buckHwIfPlecsSetMeasBuffer);
	rpRegisterHandle(&buckPlecs.interface.rp, BUCK_HW_IF_PLECS_SET_OUT_BUFFER, buckHwIfPlecsSetOutBuffer);

	return 0;
}
//-----------------------------------------------------------------------------
int32_t buckHwIfPlecsGetMeas(void *meas){

	int *p = (int *)meas;

	if( buckPlecs.measbuffer ){
		*p = *buckPlecs.measbuffer;

		return 1;
	}

	return 0;
}
//-----------------------------------------------------------------------------
void buckHwIfPlecsProcMeas(void *meas, void *procmeas, int32_t n){

	int *s = (int *)meas;
	int *d = (int *)procmeas;

	while(n--){
		*d++ = *s++;
	}
}
//-----------------------------------------------------------------------------
void buckHwIfPlecsProcOutputs(void *outputs, void *procoutputs, int32_t n){

	int *s = (int *)outputs;
	int *d = (int *)procoutputs;

	while(n--){
		*d++ = *s++;
	}
}
//-----------------------------------------------------------------------------
void buckHwIfPlecsApplyOutputs(void *outputs, int32_t n){

	int *p = (int *)outputs;

	if( buckPlecs.outputbuffer ){
		*buckPlecs.outputbuffer = *p;
	}
}
//-----------------------------------------------------------------------------
int32_t buckHwIfPlecsInterface(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	int32_t status;

	status = rpRequest(&buckPlecs.interface.rp, in, insize, out, maxoutsize);

	return status;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t buckHwIfPlecsSetMeasBuffer(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	//uint64_t *p;

	//p = (uint64_t *)( in );

	buckPlecs.measbuffer = (int *)( *( (uint64_t *)( in ) ) );

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfPlecsSetOutBuffer(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	buckPlecs.outputbuffer = (int *)( *( (uint64_t *)( in ) ) );

	return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
