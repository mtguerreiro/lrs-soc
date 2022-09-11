/*
 * control.h
 *
 *  Created on: 02.09.2022
 *      Author: mguerreiro
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
/* On entry hook */
typedef void(*controlOnEntry_t)(void);

/* Gets measurements */
typedef int32_t(*controlGetMeas_t)(void *meas);

/* Processes measurements */
typedef void(*controlProcMeas_t)(void *meas, void *procmeas, int32_t n);

/* Gets inputs */
typedef int32_t(*controlGetInputs_t)(void *inputs);

/* Runs control algorithm */
typedef int32_t(*controlRun_t)(void *inputs, int32_t ninputs, void *meas, int32_t nmeas, void *outputs);

/* Processes outputs */
typedef void(*controlProcOutputs_t)(void *outputs, void *procoutputs, int32_t n);

/* Applies outputs */
typedef void(*controlApplyOutputs_t)(void *outputs, int32_t n);

/* On exit hook */
typedef void(*controlOnExit_t)(void);

/* Sets controller parameters */
typedef int32_t(*controlSetCtlParams_t)(void *params, int32_t n);

/* Gets controller parameters */
typedef int32_t(*controlGetCtlParams_t)(void *in, void *out);

/* Sets hardware parameters */
typedef int32_t(*controlSetHwParams_t)(void *params, int32_t n);

/* Gets hardware parameters */
typedef int32_t(*controlGetHwParams_t)(void *in, void *out);

#define CONTROL_CONFIG_N_MEAS_MAX		20
#define CONTROL_CONFIG_N_INPUTS_MAX		5
#define CONTROL_CONFIG_N_OUTPUTS_MAX	5
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void controlInitialize(void);
//-----------------------------------------------------------------------------
void controlRun(void);
//-----------------------------------------------------------------------------
int32_t controlSetCtlParams(void *params, int32_t n);
//-----------------------------------------------------------------------------
int32_t controlGetCtlParams(void *in, void *out);
//-----------------------------------------------------------------------------
int32_t controlSetHwParams(void *params, int32_t n);
//-----------------------------------------------------------------------------
int32_t controlGetHwParams(void *in, void *out);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* CONTROL_H_ */
