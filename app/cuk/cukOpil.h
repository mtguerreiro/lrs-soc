/*
 * cukOpil.h
 *
 *  Created on: 11.09.2023
 *      Author: LRS
 */

#ifndef CUK_OPIL_H_
#define CUK_OPIL_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
#include "stddef.h"

//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
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
int32_t cukOpilUpdateMeasurements(void *meas, int32_t size);
//-----------------------------------------------------------------------------
int32_t cukOpilGetMeasurements(void *meas);
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
int32_t cukOpilUpdateSimData(void *simData, int32_t size);
//-----------------------------------------------------------------------------
int32_t cukOpilGetSimData(void **simData, int32_t size);
//-----------------------------------------------------------------------------
void cukOpilInitializeControl(void);
//-----------------------------------------------------------------------------
/**
 * @brief Runs the control algorithm.
 */
void cukOpilRunControl(void);
//-----------------------------------------------------------------------------
int32_t cukOpilUpdateControl(void *control, int32_t size);
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
int32_t cukOpilGetControl(void **control);
//-----------------------------------------------------------------------------
void cukOpilUpdateControllerData(void *data, int32_t size);
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
int32_t cukOpilGetControllerData(void **controllerData);
//-----------------------------------------------------------------------------
int32_t cukOpilProcInputs(void *inputs, void *procinputs, int32_t size);
//-----------------------------------------------------------------------------
int32_t cukOpilProcOutputs(void *outputs, void *procoutputs, int32_t size);
//-----------------------------------------------------------------------------
void cukOpilDisable(void);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* CUK_OPIL_H_ */
