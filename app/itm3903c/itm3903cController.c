/*
 * itm3903c.c
 *
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "itm3903cController.h"

#include "ocp/utils/rp.h"

#include "itm3903cConfig.h"

/* Controllers */
#include "itm3903cControlDisabled.h"
#include "itm3903cControlOL.h"

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
	rphandle_t handles[ITM3903C_CONTROLLER_IF_END];
	rpctx_t rp;
}controllerIF_t;

typedef struct{

	controllerInit initialize[ITM3903C_CONTROLLER_END];
	controllerSP setParams[ITM3903C_CONTROLLER_END];
	controllerGP getParams[ITM3903C_CONTROLLER_END];
	controllerR run[ITM3903C_CONTROLLER_END];
	controllerReset reset[ITM3903C_CONTROLLER_END];

	uint32_t active;

	controllerIF_t interface;

	itm3903cControllerEnable_t enable;
	itm3903cControllerDisable_t disable;

	itm3903cConfigReferences_t refs;

}controller_t;

static controller_t controllers = {.active = ITM3903C_CONTROLLER_DISABLED};

//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static void itm3903cControllerInitializeInterface(void);
static void itm3903cControllerInitializeControllers(void);
static void itm3903cControllerInitializeReferences(void);

static int32_t itm3903cControllerInterfaceGetController(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cControllerInterfaceSetController(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cControllerInterfaceGetControllerParams(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cControllerInterfaceSetControllerParams(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cControllerInterfaceReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cControllerInterfaceSetReferences(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cControllerInterfaceGetReferences(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void itm3903cControllerInitialize(itm3903cControllerConfig_t *config){

	controllers.enable = config->enable;
    controllers.disable = config->disable;

    if( controllers.disable != 0 ) controllers.disable();

    itm3903cControllerInitializeInterface();
    itm3903cControllerInitializeControllers();
    itm3903cControllerInitializeReferences();
}
//-----------------------------------------------------------------------------
int32_t itm3903cControllerInterface(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	int32_t status;

	status = rpRequest(&controllers.interface.rp, in, insize, out, maxoutsize);

	return status;
}
//-----------------------------------------------------------------------------
int32_t itm3903cControllerRun(void *meas, int32_t nmeas, void *outputs, int32_t nmaxoutputs){

	int32_t status = ITM3903C_CONTROLLER_ERR_INACTIVE_CTL;
	uint32_t ctl = controllers.active;

	if( ctl >= ITM3903C_CONTROLLER_END )
	    return ITM3903C_CONTROLLER_ERR_INACTIVE_CTL;

    status = controllers.run[ctl](meas, nmeas, &controllers.refs, sizeof(controllers.refs), outputs, nmaxoutputs);

	return status;
}
//-----------------------------------------------------------------------------
int32_t itm3903cControllerStatus(void){

    return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static void itm3903cControllerInitializeInterface(void){

    /* Initializes the request processor */
    rpInitialize(&controllers.interface.rp, ITM3903C_CONTROLLER_IF_END, controllers.interface.handles);
    rpRegisterHandle(&controllers.interface.rp, ITM3903C_CONTROLLER_IF_GET, itm3903cControllerInterfaceGetController);
    rpRegisterHandle(&controllers.interface.rp, ITM3903C_CONTROLLER_IF_SET, itm3903cControllerInterfaceSetController);
    rpRegisterHandle(&controllers.interface.rp, ITM3903C_CONTROLLER_IF_GET_PARAMS, itm3903cControllerInterfaceGetControllerParams);
    rpRegisterHandle(&controllers.interface.rp, ITM3903C_CONTROLLER_IF_SET_PARAMS, itm3903cControllerInterfaceSetControllerParams);
    rpRegisterHandle(&controllers.interface.rp, ITM3903C_CONTROLLER_IF_RESET, itm3903cControllerInterfaceReset);
    rpRegisterHandle(&controllers.interface.rp, ITM3903C_CONTROLLER_IF_SET_REF, itm3903cControllerInterfaceSetReferences);
    rpRegisterHandle(&controllers.interface.rp, ITM3903C_CONTROLLER_IF_GET_REF, itm3903cControllerInterfaceGetReferences);
}
//-----------------------------------------------------------------------------
static void itm3903cControllerInitializeControllers(void){

    uint32_t k;

    /* Register the available controllers */
    controllers.initialize[ITM3903C_CONTROLLER_DISABLED] = itm3903cControlDisabledInitialize;
    controllers.setParams[ITM3903C_CONTROLLER_DISABLED] = itm3903cControlDisabledSetParams;
    controllers.getParams[ITM3903C_CONTROLLER_DISABLED] = itm3903cControlDisabledGetParams;
    controllers.run[ITM3903C_CONTROLLER_DISABLED] = itm3903cControlDisabledRun;
    controllers.reset[ITM3903C_CONTROLLER_DISABLED] = itm3903cControlDisabledReset;

    controllers.initialize[ITM3903C_CONTROLLER_OL] = itm3903cControlOLInitialize;
    controllers.setParams[ITM3903C_CONTROLLER_OL] = itm3903cControlOLSetParams;
    controllers.getParams[ITM3903C_CONTROLLER_OL] = itm3903cControlOLGetParams;
    controllers.run[ITM3903C_CONTROLLER_OL] = itm3903cControlOLRun;
    controllers.reset[ITM3903C_CONTROLLER_OL] = itm3903cControlOLReset;

    /* Initializes all registered controllers */
    for(k = 0; k < ITM3903C_CONTROLLER_END; k++){
        controllers.initialize[k]();
    }
}
//-----------------------------------------------------------------------------
static void itm3903cControllerInitializeReferences(void){

}
//-----------------------------------------------------------------------------
static int32_t itm3903cControllerInterfaceGetController(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	uint32_t *p;

	p = (uint32_t *)( *out );
	*p = controllers.active;

	return sizeof( controllers.active );
}
//-----------------------------------------------------------------------------
static int32_t itm3903cControllerInterfaceSetController(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	uint32_t ctl;

	ctl = *( (uint32_t *) in );

	if( ctl >= ITM3903C_CONTROLLER_END ) return ITM3903C_CONTROLLER_ERR_INVALID_CTL;

	controllers.active = ctl;

	if( ctl == ITM3903C_CONTROLLER_DISABLED ){
	    if( controllers.disable != 0 ) controllers.disable();
	}
	else{
	    if( controllers.enable != 0 ) controllers.enable();
	}


	return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cControllerInterfaceGetControllerParams(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	uint32_t status;
	uint32_t *p;
	uint32_t ctl;

	p = (uint32_t *)in;
	ctl = *p++;

	if( ctl >= ITM3903C_CONTROLLER_END ) return ITM3903C_CONTROLLER_ERR_INVALID_CTL;

	status = controllers.getParams[ctl]((void *)p, insize, *out, maxoutsize);

	return status;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cControllerInterfaceSetControllerParams(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	uint32_t status;
	uint32_t *p;
	uint32_t ctl;

	p = (uint32_t *)in;
	ctl = *p++;

	if( ctl >= ITM3903C_CONTROLLER_END ) return ITM3903C_CONTROLLER_ERR_INVALID_CTL;

	status = controllers.setParams[ctl]((void *)p, insize - sizeof( ctl ) );

	return status;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cControllerInterfaceReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *p;
    uint32_t ctl;

    p = (uint32_t *)in;
    ctl = *p++;

    if( ctl >= ITM3903C_CONTROLLER_END ) return ITM3903C_CONTROLLER_ERR_INVALID_CTL;

    controllers.reset[ctl]();

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cControllerInterfaceSetReferences(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    itm3903cConfigReferences_t *p;

    p = (itm3903cConfigReferences_t *)in;

    controllers.refs = *p;

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cControllerInterfaceGetReferences(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    itm3903cConfigReferences_t *p;

    p = (itm3903cConfigReferences_t *)*out;

    *p = controllers.refs;

    return sizeof(itm3903cConfigReferences_t);
}
//-----------------------------------------------------------------------------
//=============================================================================
