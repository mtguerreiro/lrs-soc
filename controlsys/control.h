/*
 * control.h
 *
 *  Created on: 02.09.2022
 *      Author: mguerreiro
 *
 * This is a generic control system written in C.
 *
 * Controlling any system requires the same conceptual steps: take measurements
 * from the system, read set-points, compute the control action and apply it
 * to the system.
 *
 * This library provides a simple state machine that executes these steps
 * sequentially. The following steps are executed:
 *
 * 1. Get measurements
 * 2. Execute control algorithm
 * 3. Apply outputs
 *
 * The user is completely responsible for the proper actions taken at each
 * step; the state machine simply calls the functions assigned for each step.
 *
 * A detailed description of each step is given below.
 *
 * 1. Get measurements
 * 	  In this step, the physical measurements should be taken. This usually
 * corresponds to reading ADCs and converting the samples to their
 * corresponding physical quantities (for instance, voltage, current,
 * temperature, etc).
 *
 * 2. Execute control algorithm
 * 	  In this step, the control algorithm is executed. Measurements taken in
 * the previous step are passed to the control algorithm, which computes the
 * proper control signals.
 *
 * 3. Apply outputs
 *    In this step, the control signals are actually applied. This usually
 * corresponds to setting PWM signals, relays, etc.
 *
 */

#ifndef CONTROL_H_
#define CONTROL_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef enum controlRunStatus_t{
	CONTROL_RUN_STATUS_SUCCESS,
	CONTROL_RUN_STATUS_HARDWARE_ERROR,
	CONTROL_RUN_STATUS_CONTROLLER_ERROR
}controlRunStatus_t;

/* On entry hook */
typedef void(*controlOnEntry_t)(void);

/* Get inputs */
typedef int32_t(*controlGetInputs_t)(void *inputs);

/* Runs control algorithm */
typedef int32_t(*controlRun_t)(void *inputs, int32_t ninputs, void *outputs, int32_t nmaxoutputs);

/* Applies outputs */
typedef int32_t(*controlApplyOutputs_t)(void *outputs, int32_t n);

/* On exit hook */
typedef void(*controlOnExit_t)(void);

/* Config struct */
typedef struct controlConfig_t{
	void *binputs;
	void *boutputs;

	controlOnEntry_t fonEntry;
	controlGetInputs_t fgetInputs;
	controlRun_t frun;
	controlApplyOutputs_t fapplyOutputs;
	controlOnEntry_t fonExit;
}controlConfig_t;

/* Controller context */
typedef struct control_t{
	void *binputs;
	void *boutputs;

	controlOnEntry_t fonEntry;
	controlGetInputs_t fgetInputs;
	controlRun_t frun;
	controlApplyOutputs_t fapplyOutputs;
	controlOnEntry_t fonExit;
}control_t;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void controlInitialize(control_t *control, controlConfig_t *config);
//-----------------------------------------------------------------------------
void controlSetGetMeasFun(control_t *control, controlGetInputs_t getinputs);
//-----------------------------------------------------------------------------
void controlSetRunFun(control_t *control, controlRun_t run);
//-----------------------------------------------------------------------------
void controlSetApplyOutputsFun(control_t *control, controlApplyOutputs_t applyOutputs);
//-----------------------------------------------------------------------------
int32_t controlRun(control_t *control);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* CONTROL_H_ */
