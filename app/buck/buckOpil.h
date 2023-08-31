/*
 * buckOpil.h
 *
 *  Created on: 30 de ago de 2023
 *      Author: LRS
 */

#ifndef BUCK_OPIL_H_
#define BUCK_OPIL_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
#include "stddef.h"

//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef void (*buckOpilInit_t)(void);
typedef void (*buckOpilControlRun_t)(
		void *meas,
		void *simData,
		void *control,
		void *controllerData
		);
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
/**
 * @brief Initializes the controller interface.
 */
void buckOpilInitialize(
        buckOpilInit_t controlInit, buckOpilControlRun_t controlRun
		);
//-----------------------------------------------------------------------------
/**
 * @brief Updates measurements data.
 *
 * The internal measurements structure is updated with the new data.
 *
 * @param meas Pointer to measurements data.
 * @param size Size of the measurements data, in bytes.
 * @return Always returns 0.
 */
int32_t buckOpilUpdateMeasurements(void *meas, int32_t size);
//-----------------------------------------------------------------------------
int32_t buckOpilGetMeasurements(void *meas);
//-----------------------------------------------------------------------------
/**
 * @brief Updates simulation data.
 *
 * The internal simulation data structure is updated with the new data.
 *
 * @param simData Pointer to simulation data.
 * @param size Size of the simulation data, in bytes.
 * @return Always returns 0.
 */
int32_t buckOpilUpdateSimData(void *simData, int32_t size);
//-----------------------------------------------------------------------------
int32_t buckOpilGetSimData(void **simData, int32_t size);
//-----------------------------------------------------------------------------
void buckOpilInitializeControl(void);
//-----------------------------------------------------------------------------
/**
 * @brief Runs the control algorithm.
 */
void buckOpilRunControl(void);
//-----------------------------------------------------------------------------
int32_t buckOpilUpdateControl(void *control, int32_t size);
//-----------------------------------------------------------------------------
/**
 * @brief Gets control data.
 *
 * The location of the structure holding the control data is stored in control.
 *
 * @param control Address of pointer to hold the address of the
 * 			      control data structure.
 * @return The size of the control data structure, in bytes.
 */
int32_t buckOpilGetControl(void **control);
//-----------------------------------------------------------------------------
void buckOpilUpdateControllerData(void *data, int32_t size);
//-----------------------------------------------------------------------------
/**
 * @brief Gets controller data.
 *
 * The location of the structure holding the controller data is stored in
 * controllerData.
 *
 * @param controllerData Address of pointer to hold the address of the
 * 			             controller data structure.
 * @return The size of the control data structure, in bytes.
 */
int32_t buckOpilGetControllerData(void **controllerData);
//-----------------------------------------------------------------------------
int32_t buckOpilProcInputs(void *inputs, void *procinputs, int32_t size);
//-----------------------------------------------------------------------------
int32_t buckOpilProcOutputs(void *outputs, void *procoutputs, int32_t size);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* BUCK_OPIL_H_ */
