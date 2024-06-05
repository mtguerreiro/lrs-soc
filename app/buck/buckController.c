/*
 * buckController.c
 *
 *  Created on: 11.09.2023
 *      Author: LRS
 */

#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "buckController.h"

#include "../utils/rp.h"

#include "buckConfig.h"

/* Controllers */
#include "buckControlDisabled.h"
#include "buckControlStartup.h"
#include "buckControlSfbInt.h"

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

	buckControllerEnable_t enable;
	buckControllerDisable_t disable;

	buckConfigReferences_t refs;

}controller_t;

static controller_t controllers = {.active = BUCK_CONTROLLER_DISABLED};

//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static void buckControllerInitializeInterface(void);
static void buckControllerInitializeControllers(void);
static void buckControllerInitializeReferences(void);

static int32_t buckControllerInterfaceGetController(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckControllerInterfaceSetController(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckControllerInterfaceGetControllerParams(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckControllerInterfaceSetControllerParams(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckControllerInterfaceReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckControllerInterfaceSetReferences(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckControllerInterfaceGetReferences(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void buckControllerInitialize(buckControllerConfig_t *config){

	controllers.enable = config->enable;
    controllers.disable = config->disable;

    if( controllers.disable != 0 ) controllers.disable();

    buckControllerInitializeInterface();
    buckControllerInitializeControllers();
    buckControllerInitializeReferences();
}
//-----------------------------------------------------------------------------
int32_t buckControllerInterface(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	int32_t status;

	status = rpRequest(&controllers.interface.rp, in, insize, out, maxoutsize);

	return status;
}
//-----------------------------------------------------------------------------
int32_t buckControllerRun(void *meas, int32_t nmeas, void *outputs, int32_t nmaxoutputs){

	int32_t status = BUCK_CONTROLLER_ERR_INACTIVE_CTL;
	uint32_t ctl = controllers.active;

	if( ctl >= BUCK_CONTROLLER_END )
	    return BUCK_CONTROLLER_ERR_INACTIVE_CTL;

    status = controllers.run[ctl](meas, nmeas, &controllers.refs, sizeof(controllers.refs), outputs, nmaxoutputs);

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
static void buckControllerInitializeInterface(void){

    /* Initializes the request processor */
    rpInitialize(&controllers.interface.rp, BUCK_CONTROLLER_IF_END, controllers.interface.handles);
    rpRegisterHandle(&controllers.interface.rp, BUCK_CONTROLLER_IF_GET, buckControllerInterfaceGetController);
    rpRegisterHandle(&controllers.interface.rp, BUCK_CONTROLLER_IF_SET, buckControllerInterfaceSetController);
    rpRegisterHandle(&controllers.interface.rp, BUCK_CONTROLLER_IF_GET_PARAMS, buckControllerInterfaceGetControllerParams);
    rpRegisterHandle(&controllers.interface.rp, BUCK_CONTROLLER_IF_SET_PARAMS, buckControllerInterfaceSetControllerParams);
    rpRegisterHandle(&controllers.interface.rp, BUCK_CONTROLLER_IF_RESET, buckControllerInterfaceReset);
    rpRegisterHandle(&controllers.interface.rp, BUCK_CONTROLLER_IF_SET_REF, buckControllerInterfaceSetReferences);
    rpRegisterHandle(&controllers.interface.rp, BUCK_CONTROLLER_IF_GET_REF, buckControllerInterfaceGetReferences);
}
//-----------------------------------------------------------------------------
static void buckControllerInitializeControllers(void){

    uint32_t k;

    /* Register the available controllers */
    controllers.initialize[BUCK_CONTROLLER_DISABLED] = buckControlDisabledInitialize;
    controllers.setParams[BUCK_CONTROLLER_DISABLED] = buckControlDisabledSetParams;
    controllers.getParams[BUCK_CONTROLLER_DISABLED] = buckControlDisabledGetParams;
    controllers.run[BUCK_CONTROLLER_DISABLED] = buckControlDisabledRun;
    controllers.reset[BUCK_CONTROLLER_DISABLED] = buckControlDisabledReset;

    controllers.initialize[BUCK_CONTROLLER_STARTUP] = buckControlStartupInitialize;
    controllers.setParams[BUCK_CONTROLLER_STARTUP] = buckControlStartupSetParams;
    controllers.getParams[BUCK_CONTROLLER_STARTUP] = buckControlStartupGetParams;
    controllers.run[BUCK_CONTROLLER_STARTUP] = buckControlStartupRun;
    controllers.reset[BUCK_CONTROLLER_STARTUP] = buckControlStartupReset;

    controllers.initialize[BUCK_CONTROLLER_SFB_INT] = buckControlSfbIntInitialize;
    controllers.setParams[BUCK_CONTROLLER_SFB_INT] = buckControlSfbIntSetParams;
    controllers.getParams[BUCK_CONTROLLER_SFB_INT] = buckControlSfbIntGetParams;
    controllers.run[BUCK_CONTROLLER_SFB_INT] = buckControlSfbIntRun;
    controllers.reset[BUCK_CONTROLLER_SFB_INT] = buckControlSfbIntReset;

    /* Initializes all registered controllers */
    for(k = 0; k < BUCK_CONTROLLER_END; k++){
        controllers.initialize[k]();
    }
}
//-----------------------------------------------------------------------------
static void buckControllerInitializeReferences(void){

    controllers.refs.v_o = 0.0f;
}
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

	if( ctl == BUCK_CONTROLLER_DISABLED ){
	    if( controllers.disable != 0 ) controllers.disable();
	}
	else{
	    if( controllers.enable != 0 ) controllers.enable();
	}


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
static int32_t buckControllerInterfaceSetReferences(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    buckConfigReferences_t *p;

    p = (buckConfigReferences_t *)in;

    controllers.refs = *p;

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t buckControllerInterfaceGetReferences(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    buckConfigReferences_t *p;

    p = (buckConfigReferences_t *)*out;

    *p = controllers.refs;

    return sizeof(buckConfigReferences_t);
}
//-----------------------------------------------------------------------------
//=============================================================================
#endif /* SOC_CPU1 */
