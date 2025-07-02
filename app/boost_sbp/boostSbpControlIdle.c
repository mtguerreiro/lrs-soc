
//#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "boostSbpControlIdle.h"
#include "boostSbpConfig.h"

#include "ocpConfig.h"

/* Controllers */
#include "controller/controller.h"

#include "boostSbpHw.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define BOOST_SBP_CONTROL_IDLE_DECREMENT    0.005
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t boostSbpControlIdleInitialize(void){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t boostSbpControlIdleRun(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    boostSbpConfigMeasurements_t *m = (boostSbpConfigMeasurements_t *)meas;
    boostSbpConfigControl_t *o = (boostSbpConfigControl_t *)outputs;

    o->u = 0;

    return sizeof(boostSbpConfigControl_t);
}
//-----------------------------------------------------------------------------
int32_t boostSbpControlIdleSetParams(void *params, uint32_t size){

	return 0;
}
//-----------------------------------------------------------------------------
int32_t boostSbpControlIdleGetParams(void *buffer, uint32_t size){

    return 0;
}
//-----------------------------------------------------------------------------
void boostSbpControlIdleReset(void){

}
//-----------------------------------------------------------------------------
int32_t boostSbpControlIdleFirstEntry(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    boostSbpHwControllerDisable();

    return sizeof(boostSbpConfigControl_t);
}
//-----------------------------------------------------------------------------
int32_t boostSbpControlIdleLastExit(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    boostSbpHwControllerEnable();

    return sizeof(boostSbpConfigControl_t);
}
//-----------------------------------------------------------------------------
void boostSbpControlIdleGetCallbacks(void *callbacksBuffer){

    controllerCallbacks_t *cbs = (controllerCallbacks_t * )callbacksBuffer;

    cbs->init = boostSbpControlIdleInitialize;
    cbs->run = boostSbpControlIdleRun;
    cbs->setParams = boostSbpControlIdleSetParams;
    cbs->getParams = boostSbpControlIdleGetParams;
    cbs->reset = boostSbpControlIdleReset;
    cbs->firstEntry = boostSbpControlIdleFirstEntry;
    cbs->lastExit = boostSbpControlIdleLastExit;
}
//-----------------------------------------------------------------------------
//=============================================================================
//#endif /* SOC_CPU1 */
