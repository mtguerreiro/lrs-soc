/*
 * controller.c
 *
 *  Created on: 10 de set de 2022
 *      Author: marco
 */

#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "buckController.h"

#include "../utils/rp.h"

/* Controllers */
#include "buckControlSfb.h"
#include "buckControlDisabled.h"

//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef void(*controllerInit)(void);
typedef void(*controllerReset)(void);
typedef int32_t(*controllerSP)(void *in, uint32_t insize);
typedef int32_t(*controllerGP)(void *in, uint32_t insize, void *out, uint32_t maxoutsize);
typedef int32_t(*controllerR)(void *inputs, int32_t ninputs, void *outputs, int32_t nmaxoutputs);

typedef struct{
	rphandle_t handles[BUCK_CONTROLLER_IF_END];
	rpctx_t rp;
}controllerIF_t;

typedef struct{

	controllerInit initialize[BUCK_CONTROLLER_END];
	controllerSP setParams[BUCK_CONTROLLER_END];
	controllerGP getParams[BUCK_CONTROLLER_END];
	controllerR run[BUCK_CONTROLLER_END];
	controllerReset reset[BUCK_CONTROLLER_END];

	uint32_t active;

	controllerIF_t interface;

}controller_t;

static controller_t controllers = {.active = BUCK_CONTROLLER_DISABLED};

//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static int32_t buckControllerInterfaceGetController(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckControllerInterfaceSetController(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckControllerInterfaceGetControllerParams(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckControllerInterfaceSetControllerParams(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckControllerInterfaceReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void buckControllerInitialize(void){

	uint32_t k;

	/* Initializes the request processor */
	rpInitialize(&controllers.interface.rp, BUCK_CONTROLLER_IF_END, controllers.interface.handles);
	rpRegisterHandle(&controllers.interface.rp, BUCK_CONTROLLER_IF_GET, buckControllerInterfaceGetController);
	rpRegisterHandle(&controllers.interface.rp, BUCK_CONTROLLER_IF_SET, buckControllerInterfaceSetController);
	rpRegisterHandle(&controllers.interface.rp, BUCK_CONTROLLER_IF_GET_PARAMS, buckControllerInterfaceGetControllerParams);
	rpRegisterHandle(&controllers.interface.rp, BUCK_CONTROLLER_IF_SET_PARAMS, buckControllerInterfaceSetControllerParams);
    rpRegisterHandle(&controllers.interface.rp, BUCK_CONTROLLER_IF_RESET, buckControllerInterfaceReset);

	/* Register the available controllers */
    controllers.initialize[BUCK_CONTROLLER_DISABLED] = buckControlDisabledInitialize;
    controllers.setParams[BUCK_CONTROLLER_DISABLED] = buckControlDisabledSetParams;
    controllers.getParams[BUCK_CONTROLLER_DISABLED] = buckControlDisabledGetParams;
    controllers.run[BUCK_CONTROLLER_DISABLED] = buckControlDisabledRun;
    controllers.reset[BUCK_CONTROLLER_DISABLED] = buckControlDisabledReset;

    controllers.initialize[BUCK_CONTROLLER_SFB] = buckControlSfbInitialize;
    controllers.setParams[BUCK_CONTROLLER_SFB] = buckControlSfbSetParams;
    controllers.getParams[BUCK_CONTROLLER_SFB] = buckControlSfbGetParams;
    controllers.run[BUCK_CONTROLLER_SFB] = buckControlSfbRun;
    controllers.reset[BUCK_CONTROLLER_SFB] = buckControlSfbReset;

	/* Initializes all registered controllers */
	for(k = 0; k < BUCK_CONTROLLER_END; k++){
		controllers.initialize[k]();
	}
}
//-----------------------------------------------------------------------------
int32_t buckControllerInterface(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	int32_t status;

	status = rpRequest(&controllers.interface.rp, in, insize, out, maxoutsize);

	return status;
}
//-----------------------------------------------------------------------------
int32_t buckControllerRun(void *inputs, int32_t ninputs, void *outputs, int32_t nmaxoutputs){

	int32_t status = BUCK_CONTROLLER_ERR_INACTIVE_CTL;
	uint32_t ctl = controllers.active;

	if( ctl != BUCK_CONTROLLER_END ){
		status = controllers.run[ctl](inputs, ninputs, outputs, nmaxoutputs);
	}

	return status;
}
//-----------------------------------------------------------------------------
int32_t buckControllerStatus(void){

    return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t buckControllerInterfaceGetController(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	uint32_t *p;

	p = (uint32_t *)( *out );
	*p = controllers.active;

	return sizeof( controllers.active );
}
//-----------------------------------------------------------------------------
static int32_t buckControllerInterfaceSetController(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	uint32_t ctl;

	ctl = *( (uint32_t *) in );

	if( ctl >= BUCK_CONTROLLER_END ) return BUCK_CONTROLLER_ERR_INVALID_CTL;

	controllers.active = ctl;

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t buckControllerInterfaceGetControllerParams(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	uint32_t status;
	uint32_t *p;
	uint32_t ctl;

	p = (uint32_t *)in;
	ctl = *p++;

	if( ctl >= BUCK_CONTROLLER_END ) return BUCK_CONTROLLER_ERR_INVALID_CTL;

	status = controllers.getParams[ctl]((void *)p, insize, *out, maxoutsize);

	return status;
}
//-----------------------------------------------------------------------------
static int32_t buckControllerInterfaceSetControllerParams(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	uint32_t status;
	uint32_t *p;
	uint32_t ctl;

	p = (uint32_t *)in;
	ctl = *p++;

	if( ctl >= BUCK_CONTROLLER_END ) return BUCK_CONTROLLER_ERR_INVALID_CTL;

	status = controllers.setParams[ctl]((void *)p, insize - sizeof( ctl ) );

	return status;
}
//-----------------------------------------------------------------------------
static int32_t buckControllerInterfaceReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *p;
    uint32_t ctl;

    p = (uint32_t *)in;
    ctl = *p++;

    if( ctl >= BUCK_CONTROLLER_END ) return BUCK_CONTROLLER_ERR_INVALID_CTL;

    controllers.reset[ctl]();

    return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
#endif /* SOC_CPU1 */
