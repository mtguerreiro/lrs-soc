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

#include "../utils/rp.h"

/* Controllers */
#include "pidctl.h"

//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef void(*controllerInit)(void);
typedef int32_t(*controllerSP)(void *in, uint32_t insize);
typedef int32_t(*controllerGP)(void *in, uint32_t insize, void *out, uint32_t maxoutsize);
typedef int32_t(*controllerR)(void *inputs, int32_t ninputs, void *outputs, int32_t nmaxoutputs);

typedef struct{
	rphandle_t handles[CONTROLLER_IF_END];
	rpctx_t rp;
}controllerIF_t;

typedef struct{

	controllerInit initialize[CONTROLLER_END];
	controllerSP setParams[CONTROLLER_END];
	controllerGP getParams[CONTROLLER_END];
	controllerR run[CONTROLLER_END];

	uint32_t active;

	controllerIF_t interface;

}controller_t;

static controller_t controllers = {.active = CONTROLLER_END};

//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static int32_t controllerInterfaceGetController(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t controllerInterfaceSetController(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t controllerInterfaceGetControllerParams(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t controllerInterfaceSetControllerParams(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void controllerInitialize(void){

	uint32_t k;

	/* Initializes the request processor */
	rpInitialize(&controllers.interface.rp, CONTROLLER_IF_END, controllers.interface.handles);
	rpRegisterHandle(&controllers.interface.rp, CONTROLLER_IF_GET, controllerInterfaceGetController);
	rpRegisterHandle(&controllers.interface.rp, CONTROLLER_IF_SET, controllerInterfaceSetController);
	rpRegisterHandle(&controllers.interface.rp, CONTROLLER_IF_GET_PARAMS, controllerInterfaceGetControllerParams);
	rpRegisterHandle(&controllers.interface.rp, CONTROLLER_IF_SET_PARAMS, controllerInterfaceSetControllerParams);

	/* Register the available controllers */
	controllers.initialize[CONTROLLER_PID] = pidctlInitialize;
	controllers.setParams[CONTROLLER_PID] = pidctlSetParams;
	controllers.getParams[CONTROLLER_PID] = pidctlGetParams;
	controllers.run[CONTROLLER_PID] = pidctlRun;

	/* Initializes all registered controllers */
	for(k = 0; k < CONTROLLER_END; k++){
		controllers.initialize[k]();
	}
}
//-----------------------------------------------------------------------------
int32_t controllerInterface(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	int32_t status;

	status = rpRequest(&controllers.interface.rp, in, insize, out, maxoutsize);

	return status;
}
//-----------------------------------------------------------------------------
int32_t controllerRun(void *inputs, int32_t ninputs, void *outputs, int32_t nmaxoutputs){

	int32_t status = CONTROLLER_ERR_INACTIVE_CTL;
	uint32_t ctl = controllers.active;

	if( ctl != CONTROLLER_END ){
		status = controllers.run[ctl](inputs, ninputs, outputs, nmaxoutputs);
	}

	return status;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t controllerInterfaceGetController(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	uint32_t *p;

	p = (uint32_t *)( *out );
	*p = controllers.active;

	return sizeof( controllers.active );
}
//-----------------------------------------------------------------------------
static int32_t controllerInterfaceSetController(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	uint32_t ctl;

	ctl = *( (uint32_t *) in );

	if( ctl >= CONTROLLER_END ) return CONTROLLER_ERR_INVALID_CTL;

	controllers.active = ctl;

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t controllerInterfaceGetControllerParams(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	uint32_t status;
	uint32_t *p;
	uint32_t ctl;

	p = (uint32_t *)in;
	ctl = *p++;

	if( ctl >= CONTROLLER_END ) return CONTROLLER_ERR_INVALID_CTL;

	status = controllers.getParams[ctl]((void *)p, insize, *out, maxoutsize);

	return status;
}
//-----------------------------------------------------------------------------
static int32_t controllerInterfaceSetControllerParams(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	uint32_t status;
	uint32_t *p;
	uint32_t ctl;

	p = (uint32_t *)in;
	ctl = *p++;

	if( ctl >= CONTROLLER_END ) return CONTROLLER_ERR_INVALID_CTL;

	status = controllers.setParams[ctl]((void *)p, insize - sizeof( ctl ) );

	return status;
}
//-----------------------------------------------------------------------------
//=============================================================================
