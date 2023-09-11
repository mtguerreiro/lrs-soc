/*
 * ocpOpil.h
 *
 *  Created on: 11.09.2023
 *      Author: LRS
 */

#ifndef OCP_OPIL_H_
#define OCP_OPIL_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"

#include "../controlsys/controlsys.h"
//============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef int32_t (*ocpOpilUpdateMeas_t)(void *meas, int32_t size);
typedef int32_t (*ocpOpilUpdateSimData_t)(void *simdata, int32_t size);
typedef void (*ocpOpilInitControl_t)(void);
typedef int32_t (*ocpOpilGetControl_t)(void **control);
typedef int32_t (*ocpOpilGetControllerData_t)(void **controllerdata);

typedef struct ocpOpilConfig_t{

    ocpOpilUpdateMeas_t updateMeas;
    ocpOpilUpdateSimData_t updateSimData;
    ocpOpilInitControl_t initControl;
    ocpOpilGetControl_t getControl;
    ocpOpilGetControllerData_t getControllerData;
}ocpOpilConfig_t;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t ocpOpilInitialize(ocpOpilConfig_t *config);
//-----------------------------------------------------------------------------
int32_t ocpOpilUpdateMeas(void *meas, int32_t size);
//-----------------------------------------------------------------------------
int32_t ocpOpilUpdateSimData(void *simdata, int32_t size);
//-----------------------------------------------------------------------------
void ocpOpilInitControl(void);
//-----------------------------------------------------------------------------
void ocpOpilRunControl(void);
//-----------------------------------------------------------------------------
int32_t ocpOpilGetControl(void **control);
//-----------------------------------------------------------------------------
int32_t ocpOpilGetControllerData(void **controllerdata);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* OCP_OPIL_H_ */
