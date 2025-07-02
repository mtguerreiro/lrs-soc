
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "boostSbpController.h"

#include "boostSbpConfig.h"

/* OCP */
#include "ocpConfig.h"
#include "ocp/ocpTrace.h"
#include "rp/rp.h"

/* Controller lib */
#include "controller/controller.h"
#include "controller/controllerIf.h"

/* Controllers */
#include "boostSbpControlIdle.h"
#include "boostSbpControlRamp.h"

//#include "appControllerCascaded.h"
//============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef enum{
    BOOST_SBP_CONTROLLER_IDLE,
    BOOST_SBP_CONTROLLER_RAMP,
    BOOST_SBP_CONTROLLER_END
}appControllersEnum_t;

typedef struct{
    boostSbpConfigReferences_t refs;
    controller_t controller;
    controllerCallbacks_t cbs[BOOST_SBP_CONTROLLER_END];
}boostSbpController_t;

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
boostSbpController_t xboostSbpControler;
//============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t boostSbpControllerInit(void){

    ocpTraceAddSignal(
            BOOST_SBP_CONFIG_TRACE_ID,
            (void *)&xboostSbpControler.refs.v_o,
            "Voltage reference"
    );

    controllerConfig_t config;
    controllerGetCbs_t ctlGetCbs[BOOST_SBP_CONTROLLER_END] = {0};
    ctlGetCbs[BOOST_SBP_CONTROLLER_IDLE] = boostSbpControlIdleGetCallbacks;
    ctlGetCbs[BOOST_SBP_CONTROLLER_RAMP] = boostSbpControlRampGetCallbacks;

    config.refBuffer = (void *)&xboostSbpControler.refs;
    config.refSize = sizeof(xboostSbpControler.refs);
    
    config.getCbs = ctlGetCbs;
    config.cbsBuffer = xboostSbpControler.cbs;
    config.nControllers = BOOST_SBP_CONTROLLER_END;

    controllerInit(&xboostSbpControler.controller, &config);

    controllerIfInit();
    controllerIfRegister(
            &xboostSbpControler.controller,
            BOOST_SBP_CONFIG_CS_ID
    );

    return 0;
}
//-----------------------------------------------------------------------------
int32_t boostSbpControllerRun(void *inputs, int32_t ninputs, void *outputs, int32_t nmaxoutputs){
    
    int32_t status;

    status = controllerRun(
        &xboostSbpControler.controller,
        inputs, ninputs,
        outputs, nmaxoutputs);

    return status;
}
//-----------------------------------------------------------------------------
int32_t boostSbpControllerIf(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    return controllerIf(in, insize, out, maxoutsize);
}
//-----------------------------------------------------------------------------
int32_t boostSbpControllerStatus(void){

    return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
