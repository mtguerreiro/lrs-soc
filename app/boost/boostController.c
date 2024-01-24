/*
 * boostController.c
 *
 *  Created on: 11.09.2023
 *      Author: LRS
 */

#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "boostController.h"

#include "../utils/rp.h"

#include "boostConfig.h"

/* Controllers */
#include "boostControlDisabled.h"
#include "boostControlStartup.h"

//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef void(*controllerInit)(void);
typedef void(*controllerReset)(void);
typedef int32_t(*controllerSP)(void *in, uint32_t insize);
typedef int32_t(*controllerGP)(void *in, uint32_t insize, void *out, uint32_t maxoutsize);
typedef int32_t(*controllerR)(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs);

typedef struct{
	rphandle_t handles[BOOST_CONTROLLER_IF_END];
	rpctx_t rp;
}controllerIF_t;

typedef struct{

	controllerInit initialize[BOOST_CONTROLLER_END];
	controllerSP setParams[BOOST_CONTROLLER_END];
	controllerGP getParams[BOOST_CONTROLLER_END];
	controllerR run[BOOST_CONTROLLER_END];
	controllerReset reset[BOOST_CONTROLLER_END];

	uint32_t active;

	controllerIF_t interface;

	boostControllerEnable_t enable;
	boostControllerDisable_t disable;

	boostConfigReferences_t refs;

}controller_t;

static controller_t controllers = {.active = BOOST_CONTROLLER_DISABLED};

//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static void boostControllerInitializeInterface(void);
static void boostControllerInitializeControllers(void);
static void boostControllerInitializeReferences(void);

static int32_t boostControllerInterfaceGetController(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostControllerInterfaceSetController(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostControllerInterfaceGetControllerParams(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostControllerInterfaceSetControllerParams(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostControllerInterfaceReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostControllerInterfaceSetReferences(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostControllerInterfaceGetReferences(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void boostControllerInitialize(boostControllerConfig_t *config){

	controllers.enable = config->enable;
    controllers.disable = config->disable;

    if( controllers.disable != 0 ) controllers.disable();

    boostControllerInitializeInterface();
    boostControllerInitializeControllers();
    boostControllerInitializeReferences();
}
//-----------------------------------------------------------------------------
int32_t boostControllerInterface(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	int32_t status;

	status = rpRequest(&controllers.interface.rp, in, insize, out, maxoutsize);

	return status;
}
//-----------------------------------------------------------------------------
int32_t boostControllerRun(void *meas, int32_t nmeas, void *outputs, int32_t nmaxoutputs){

	int32_t status = BOOST_CONTROLLER_ERR_INACTIVE_CTL;
	uint32_t ctl = controllers.active;

	if( ctl >= BOOST_CONTROLLER_END )
	    return BOOST_CONTROLLER_ERR_INACTIVE_CTL;

    status = controllers.run[ctl](meas, nmeas, &controllers.refs, sizeof(controllers.refs), outputs, nmaxoutputs);

	return status;
}
//-----------------------------------------------------------------------------
int32_t boostControllerStatus(void){

    return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static void boostControllerInitializeInterface(void){

    /* Initializes the request processor */
    rpInitialize(&controllers.interface.rp, BOOST_CONTROLLER_IF_END, controllers.interface.handles);
    rpRegisterHandle(&controllers.interface.rp, BOOST_CONTROLLER_IF_GET, boostControllerInterfaceGetController);
    rpRegisterHandle(&controllers.interface.rp, BOOST_CONTROLLER_IF_SET, boostControllerInterfaceSetController);
    rpRegisterHandle(&controllers.interface.rp, BOOST_CONTROLLER_IF_GET_PARAMS, boostControllerInterfaceGetControllerParams);
    rpRegisterHandle(&controllers.interface.rp, BOOST_CONTROLLER_IF_SET_PARAMS, boostControllerInterfaceSetControllerParams);
    rpRegisterHandle(&controllers.interface.rp, BOOST_CONTROLLER_IF_RESET, boostControllerInterfaceReset);
    rpRegisterHandle(&controllers.interface.rp, BOOST_CONTROLLER_IF_SET_REF, boostControllerInterfaceSetReferences);
    rpRegisterHandle(&controllers.interface.rp, BOOST_CONTROLLER_IF_GET_REF, boostControllerInterfaceGetReferences);
}
//-----------------------------------------------------------------------------
static void boostControllerInitializeControllers(void){

    uint32_t k;

    /* Register the available controllers */
    controllers.initialize[BOOST_CONTROLLER_DISABLED] = boostControlDisabledInitialize;
    controllers.setParams[BOOST_CONTROLLER_DISABLED] = boostControlDisabledSetParams;
    controllers.getParams[BOOST_CONTROLLER_DISABLED] = boostControlDisabledGetParams;
    controllers.run[BOOST_CONTROLLER_DISABLED] = boostControlDisabledRun;
    controllers.reset[BOOST_CONTROLLER_DISABLED] = boostControlDisabledReset;

    controllers.initialize[BOOST_CONTROLLER_STARTUP] = boostControlStartupInitialize;
    controllers.setParams[BOOST_CONTROLLER_STARTUP] = boostControlStartupSetParams;
    controllers.getParams[BOOST_CONTROLLER_STARTUP] = boostControlStartupGetParams;
    controllers.run[BOOST_CONTROLLER_STARTUP] = boostControlStartupRun;
    controllers.reset[BOOST_CONTROLLER_STARTUP] = boostControlStartupReset;

    /* Initializes all registered controllers */
    for(k = 0; k < BOOST_CONTROLLER_END; k++){
        controllers.initialize[k]();
    }
}
//-----------------------------------------------------------------------------
static void boostControllerInitializeReferences(void){

    controllers.refs.v_o = 0.0f;
}
//-----------------------------------------------------------------------------
static int32_t boostControllerInterfaceGetController(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	uint32_t *p;

	p = (uint32_t *)( *out );
	*p = controllers.active;

	return sizeof( controllers.active );
}
//-----------------------------------------------------------------------------
static int32_t boostControllerInterfaceSetController(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	uint32_t ctl;

	ctl = *( (uint32_t *) in );

	if( ctl >= BOOST_CONTROLLER_END ) return BOOST_CONTROLLER_ERR_INVALID_CTL;

	controllers.active = ctl;

	if( ctl == BOOST_CONTROLLER_DISABLED ){
	    if( controllers.disable != 0 ) controllers.disable();
	}
	else{
	    if( controllers.enable != 0 ) controllers.enable();
	}


	return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostControllerInterfaceGetControllerParams(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	uint32_t status;
	uint32_t *p;
	uint32_t ctl;

	p = (uint32_t *)in;
	ctl = *p++;

	if( ctl >= BOOST_CONTROLLER_END ) return BOOST_CONTROLLER_ERR_INVALID_CTL;

	status = controllers.getParams[ctl]((void *)p, insize, *out, maxoutsize);

	return status;
}
//-----------------------------------------------------------------------------
static int32_t boostControllerInterfaceSetControllerParams(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	uint32_t status;
	uint32_t *p;
	uint32_t ctl;

	p = (uint32_t *)in;
	ctl = *p++;

	if( ctl >= BOOST_CONTROLLER_END ) return BOOST_CONTROLLER_ERR_INVALID_CTL;

	status = controllers.setParams[ctl]((void *)p, insize - sizeof( ctl ) );

	return status;
}
//-----------------------------------------------------------------------------
static int32_t boostControllerInterfaceReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *p;
    uint32_t ctl;

    p = (uint32_t *)in;
    ctl = *p++;

    if( ctl >= BOOST_CONTROLLER_END ) return BOOST_CONTROLLER_ERR_INVALID_CTL;

    controllers.reset[ctl]();

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostControllerInterfaceSetReferences(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    boostConfigReferences_t *p;

    p = (boostConfigReferences_t *)in;

    controllers.refs = *p;

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostControllerInterfaceGetReferences(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    boostConfigReferences_t *p;

    p = (boostConfigReferences_t *)*out;

    *p = controllers.refs;

    return sizeof(boostConfigReferences_t);
}
//-----------------------------------------------------------------------------
//=============================================================================
#endif /* SOC_CPU1 */
