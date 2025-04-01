
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "fsbuckboostController.h"

#include "fsbuckboostConfig.h"

/* OCP */
#include "ocpConfig.h"
#include "ocp/ocpTrace.h"
#include "rp/rp.h"

/* Controller lib */
#include "controller/controller.h"
#include "controller/controllerIf.h"

/* Controllers */
#include "fsbuckboostControlIdle.h"
#include "fsbuckboostControlRamp.h"
#include "fsbuckboostControlBuckSfb.h"
#include "fsbuckboostControlBoostEnergy.h"

//#include "appControllerCascaded.h"
//============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef enum{
    FS_BUCK_BOOST_CONTROLLER_IDLE,
    FS_BUCK_BOOST_CONTROLLER_RAMP,
    FS_BUCK_BOOST_CONTROLLER_BUCK_SFB,
    FS_BUCK_BOOST_CONTROLLER_BOOST_ENERGY,
    FS_BUCK_BOOST_CONTROLLER_END
}appControllersEnum_t;

typedef struct{
    fsbuckboostConfigReferences_t refs;
    controller_t controller;
    controllerCallbacks_t cbs[FS_BUCK_BOOST_CONTROLLER_END];
}fsbuckboostController_t;

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
fsbuckboostController_t xfsbuckboostControler;
//============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t fsbuckboostControllerInit(void){

    ocpTraceAddSignal(
            FS_BUCK_BOOST_CONFIG_TRACE_ID,
            (void *)&xfsbuckboostControler.refs.v_out,
            "Voltage reference"
    );

    controllerConfig_t config;
    controllerGetCbs_t ctlGetCbs[FS_BUCK_BOOST_CONTROLLER_END] = {0};
    ctlGetCbs[FS_BUCK_BOOST_CONTROLLER_IDLE] = fsbuckboostControlIdleGetCallbacks;
    ctlGetCbs[FS_BUCK_BOOST_CONTROLLER_RAMP] = fsbuckboostControlRampGetCallbacks;
    ctlGetCbs[FS_BUCK_BOOST_CONTROLLER_BUCK_SFB] = fsbuckboostControlBuckSfbGetCallbacks;
    ctlGetCbs[FS_BUCK_BOOST_CONTROLLER_BOOST_ENERGY] = fsbuckboostControlBoostEnergyGetCallbacks;

    config.refBuffer = (void *)&xfsbuckboostControler.refs;
    config.refSize = sizeof(xfsbuckboostControler.refs);
    
    config.getCbs = ctlGetCbs;
    config.cbsBuffer = xfsbuckboostControler.cbs;
    config.nControllers = FS_BUCK_BOOST_CONTROLLER_END;

    controllerInit(&xfsbuckboostControler.controller, &config);

    controllerIfInit();
    controllerIfRegister(
            &xfsbuckboostControler.controller,
            FS_BUCK_BOOST_CONFIG_CS_ID
    );

    return 0;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControllerRun(void *inputs, int32_t ninputs, void *outputs, int32_t nmaxoutputs){
    
    int32_t status;

    status = controllerRun(
        &xfsbuckboostControler.controller,
        inputs, ninputs,
        outputs, nmaxoutputs);

    return status;
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControllerIf(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    return controllerIf(in, insize, out, maxoutsize);
}
//-----------------------------------------------------------------------------
int32_t fsbuckboostControllerStatus(void){

    return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
