/*
 * controller.c
 *
 *  Created on: 10 de set de 2022
 *      Author: marco
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "controller.h"

/* Controllers */
#include "pidctl.h"

//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef void(*controllerInit)(void);
typedef int32_t(*controllerSP)(void *params, int32_t n);
typedef int32_t(*controllerGP)(void *in, void *out);
typedef int32_t(*controllerR)(void *inputs, int32_t ninputs, void *meas, int32_t nmeas, void *outputs);

typedef struct{

	controllerInit initialize[CONTROLLER_END];
	controllerSP setParams[CONTROLLER_END];
	controllerGP getParams[CONTROLLER_END];
	controllerR run[CONTROLLER_END];

	uint32_t active;

}controller_t;

controller_t controllers = {.active = CONTROLLER_END};
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void controllerInitialize(void){

	uint32_t k;

	controllers.initialize[CONTROLLER_PID] = pidctlInitialize;
	controllers.setParams[CONTROLLER_PID] = pidctlSetParams;
	controllers.getParams[CONTROLLER_PID] = pidctlGetParams;
	controllers.run[CONTROLLER_PID] = pidctlRun;

	for(k = 0; k < CONTROLLER_END; k++){
		controllers.initialize[k]();
	}
}
//-----------------------------------------------------------------------------
int32_t controllerSetParams(void *params, int32_t n){

	uint32_t *p = (uint32_t *)params;
	uint32_t cmd, ctl;
	int32_t status = 0;

	cmd = *p++;
	ctl = *p++;

	if( ctl >= CONTROLLER_END ) return CONTROLLER_ERR_INVALID_CTL;

	switch( cmd ){

	case CONTROLLER_SET:
		controllers.active = ctl;
		break;

	case CONTROLLER_SET_PARAMS:
		status = controllers.setParams[ctl]((void *)p, n);
		break;

	default:
		return CONTROLLER_ERR_INVALID_CMD;
	}

	return status;
}
//-----------------------------------------------------------------------------
int32_t controllerGetParams(void *in, void *out){

	uint32_t *p = (uint32_t *)in;
	uint32_t cmd, ctl;
	int32_t status = 0;

	cmd = *p++;

	switch( cmd ){

	case CONTROLLER_GET:
		p = (uint32_t *)out;
		*p = controllers.active;
		break;

	case CONTROLLER_GET_PARAMS:
		ctl = *p++;
		if( ctl >= CONTROLLER_END ) return CONTROLLER_ERR_INVALID_CTL;
		status = controllers.getParams[ctl]((void *)p, out);
		break;

	default:
		return CONTROLLER_ERR_INVALID_CMD;
	}

	return status;
}
//-----------------------------------------------------------------------------
int32_t controllerRun(void *inputs, int32_t ninputs, void *meas, int32_t nmeas, void *outputs){

	int32_t status = CONTROLLER_ERR_INACTIVE_CTL;
	uint32_t ctl = controllers.active;

	if( ctl != CONTROLLER_END ){
		status = controllers.run[ctl](inputs, ninputs, meas, nmeas, outputs);
	}

	return status;
}
//-----------------------------------------------------------------------------
//=============================================================================
