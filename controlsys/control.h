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
 * This library provides a state machine that executes these steps such steps
 * sequentially. In an attempt to make this library as flexible as possible,
 * the following steps are executed:
 *
 * 1. Get measurements
 * 2. Process measurements
 * 3. Read inputs
 * 4. Execute control algorithm
 * 5. Process outputs
 * 6. Apply outputs
 *
 * The user is completely responsible for the proper actions taken at each
 * step; the state machine simply calls the functions assigned for each step.
 *
 * The main idea for such approach is to completely decouple each step, making
 * the state machine a very flexible controller. For instance, if the same
 * control system is to be used on different hardware platforms, only the
 * functions related to getting measurements and applying outputs need to be
 * modified. If the same hardware platform is to be used for a different
 * application, only the control algorithm needs to be changed (and perhaps
 * decoding the measurements and outputs). However, by clearly separating
 * each step, new hardware platforms or systems can be readily supported.
 *
 * A detailed description of each step is given below.
 *
 * 1. Get measurements
 * 	  In this step, the physical measurements should be taken. This usually
 * corresponds to reading ADCs.
 *
 * 2. Process measurements
 *    Here, the measurements taken in the previous step are processed. This
 * usually means converting the readings to actual physical values. This step
 * could be merged with the previous step; however, we separate them for more
 * flexibility.
 * 	  For instance, consider the same embedded hardware being used for
 * different applications: the same routines would be used to read the ADCs,
 * but since the sensors might be different, the conversion routine could be
 * different. If different embedded hardware is used for the same  application,
 * conversion of the measurements might be the same but reading the ADCs could
 * be different. Thus, by breaking reading and conversion apart, it is possible
 * to switch between applications or hardware while reusing common modules.
 *
 * 3. Read inputs
 * 	  Here, user inputs are read. We assume that user inputs are always
 * available; we do not need to "get" them. If they need to be updated, another
 * module will be responsible for that, and we consider this process to be
 * asynchronous to the main control process. This is usually true: we do not
 * want to spend time figuring the user inputs when executing our main control
 * routine.
 *
 * 4. Execute control algorithm
 * 	  In this step, the control algorithm is executed. Measurements and user
 * inputs are passed to the control algorithm, which computes the proper
 * control actions.
 *
 * 5. Process outputs
 * 	  Here, the control actions produced by the control algorithm are
 * processed. In short, the control actions are converter to proper hardware
 * values. For instance, a duty-cycle is converted to the proper timer value
 * used to generate a PWM signal.
 *
 * 6. Apply outputs
 *    In this step, the control signals are actually applied. The rationale to
 * break processing and applying outputs apart is the same used for getting and
 * processing measurements.
 *
 * TODO: update description
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

/* Process inputs */
typedef int32_t(*controlProcInputs_t)(void *inputs, void *procinputs, int32_t n);

/* Runs control algorithm */
typedef int32_t(*controlRun_t)(void *inputs, int32_t ninputs, void *outputs, int32_t nmaxoutputs);

/* Processes outputs */
typedef int32_t(*controlProcOutputs_t)(void *outputs, void *procoutputs, int32_t n);

/* Applies outputs */
typedef int32_t(*controlApplyOutputs_t)(void *outputs, int32_t n);

/* On exit hook */
typedef void(*controlOnExit_t)(void);

/* Config struct */
typedef struct controlConfig_t{
	void *binputs;
	void *bprocInputs;
	void *boutputs;
	void *bprocOutputs;

	controlOnEntry_t fonEntry;
	controlGetInputs_t fgetInputs;
	controlProcInputs_t fprocInputs;
	controlRun_t frun;
	controlProcOutputs_t fprocOutputs;
	controlApplyOutputs_t fapplyOutputs;
	controlOnEntry_t fonExit;

}controlConfig_t;

/* Controller context */
typedef struct control_t{
	void *binputs;
	void *bprocInputs;
	void *boutputs;
	void *bprocOutputs;

	controlOnEntry_t fonEntry;
	controlGetInputs_t fgetInputs;
	controlProcInputs_t fprocInputs;
	controlRun_t frun;
	controlProcOutputs_t fprocOutputs;
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
void controlSetProcMeasFun(control_t *control, controlProcInputs_t procinputs);
//-----------------------------------------------------------------------------
void controlSetRunFun(control_t *control, controlRun_t run);
//-----------------------------------------------------------------------------
void controlSetProcOutputsFun(control_t *control, controlProcOutputs_t procOutputs);
//-----------------------------------------------------------------------------
void controlSetApplyOutputsFun(control_t *control, controlApplyOutputs_t applyOutputs);
//-----------------------------------------------------------------------------
int32_t controlRun(control_t *control);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* CONTROL_H_ */
