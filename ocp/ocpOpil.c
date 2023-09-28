/*
 * ocpOpil.c
 *
 *  Created on: 11.09.2023
 *      Author: LRS
 */

#ifndef OCP_CONFIG_MASTER_CORE
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "ocpOpil.h"

#include "ocpTrace.h"
#include "ocpCS.h"
#include "ocpConfig.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================


typedef struct ocpOpilControl_t{

    ocpOpilUpdateMeas_t updateMeas;
    ocpOpilUpdateSimData_t updateSimData;
    ocpOpilInitControl_t initControl;
    ocpOpilRunControl_t runControl;
    ocpOpilGetControl_t getControl;
    ocpOpilGetControllerData_t getControllerData;
}ocpOpilControl_t;
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================

static ocpOpilControl_t xocontrol;

//=============================================================================


//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t ocpOpilInitialize(ocpOpilConfig_t *config){

    xocontrol.updateMeas = config->updateMeas;
    xocontrol.updateSimData = config->updateSimData;

    xocontrol.initControl = config->initControl;
    xocontrol.runControl = config->runControl;

    xocontrol.getControl = config->getControl;
    xocontrol.getControllerData = config->getControllerData;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t ocpOpilUpdateMeas(void *meas, int32_t size){

    return xocontrol.updateMeas(meas, size);
}
//-----------------------------------------------------------------------------
int32_t ocpOpilUpdateSimData(void *simdata, int32_t size){

    return xocontrol.updateSimData(simdata, size);
}
//-----------------------------------------------------------------------------
void ocpOpilInitControl(void){

    if( xocontrol.initControl != 0 ) xocontrol.initControl(0);
}
//-----------------------------------------------------------------------------
void ocpOpilRunControl(void){

    xocontrol.runControl(0);
}
//-----------------------------------------------------------------------------
int32_t ocpOpilGetControl(void **control){

    return xocontrol.getControl(control);
}
//-----------------------------------------------------------------------------
int32_t ocpOpilGetControllerData(void **controllerdata){

    return xocontrol.getControllerData(controllerdata);
}
//-----------------------------------------------------------------------------
//=============================================================================
#endif /* #ifndef OCP_CONFIG_MASTER_CORE */
