
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "cukController.h"
#include "cukConfig.h"

/* OCP */
#include "ocpConfig.h"
#include "ocp/ocpTrace.h"
#include "rp/rp.h"

/* Controller lib */
#include "controller/controller.h"
#include "controller/controllerIf.h"

/* Controllers */
#include "cukControlIdle.h"
#include "cukControlRamp.h"
#include "cukControlSfb.h"
#include "cukControlEnergy.h"
#include "cukControlCascFblin.h"
//============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef enum{
    CUK_CONTROLLER_IDLE,
    CUK_CONTROLLER_RAMP,
    CUK_CONTROLLER_SFB,
    CUK_CONTROLLER_ENERGY,
    CUK_CONTROLLER_CASC_FBLIN,
    CUK_CONTROLLER_END
}appControllersEnum_t;

typedef struct{
    cukConfigReferences_t refs;
    controller_t controller;
    controllerCallbacks_t cbs[CUK_CONTROLLER_END];
}cukController_t;

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
cukController_t xcukControler;
//============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t cukControllerInit(void){

    controllerConfig_t config;

    controllerGetCbs_t ctlGetCbs[CUK_CONTROLLER_END] = {0};
    ctlGetCbs[CUK_CONTROLLER_IDLE] = cukControlIdleGetCallbacks;
    ctlGetCbs[CUK_CONTROLLER_RAMP] = cukControlRampGetCallbacks;
    ctlGetCbs[CUK_CONTROLLER_SFB] = cukControlSfbGetCallbacks;
    ctlGetCbs[CUK_CONTROLLER_ENERGY] = cukControlEnergyGetCallbacks;
    ctlGetCbs[CUK_CONTROLLER_CASC_FBLIN] = cukControlCascFblinGetCallbacks;

    config.refBuffer = (void *)&xcukControler.refs;
    config.refSize = sizeof(xcukControler.refs);
    
    config.getCbs = ctlGetCbs;
    config.cbsBuffer = xcukControler.cbs;
    config.nControllers = CUK_CONTROLLER_END;

    controllerInit(&xcukControler.controller, &config);

    controllerIfInit();
    controllerIfRegister(
            &xcukControler.controller,
            CUK_CONFIG_CS_ID
    );

    ocpTraceAddSignal(
            CUK_CONFIG_TRACE_ID,
            (void *)&xcukControler.refs.vo,
            "Voltage reference"
    );

    return 0;
}
//-----------------------------------------------------------------------------
int32_t cukControllerRun(void *inputs, int32_t ninputs, void *outputs, int32_t nmaxoutputs){
    
    int32_t status;

    status = controllerRun(
        &xcukControler.controller,
        inputs, ninputs,
        outputs, nmaxoutputs);

    return status;
}
//-----------------------------------------------------------------------------
int32_t cukControllerIf(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    return controllerIf(in, insize, out, maxoutsize);
}
//-----------------------------------------------------------------------------
int32_t cukControllerStatus(void){

    return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
