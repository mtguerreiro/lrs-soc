/*
 * control.c
 *
 *  Created on: 02.09.2022
 *      Author: mguerreiro
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "control.h"

//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void controlInitialize(control_t *control, controlConfig_t *config){

	control->binputs = config->binputs;
	control->boutputs = config->boutputs;

	control->fgetInputs = config->fgetInputs;

	control->frun = config->frun;

	control->fapplyOutputs = config->fapplyOutputs;

	control->fonEntry = config->fonEntry;
	control->fonExit = config->fonExit;
}
//-----------------------------------------------------------------------------
void controlSetGetMeasFun(control_t *control, controlGetInputs_t getInputs){

	control->fgetInputs = getInputs;
}
//-----------------------------------------------------------------------------
void controlSetRunFun(control_t *control, controlRun_t run){

	control->frun = run;
}
//-----------------------------------------------------------------------------
void controlSetApplyOutputsFun(control_t *control, controlApplyOutputs_t applyOutputs){

	control->fapplyOutputs = applyOutputs;
}
//-----------------------------------------------------------------------------
void controlSetOnEntry(control_t *control, controlOnEntry_t fonEntry){

	control->fonEntry = fonEntry;
}
//-----------------------------------------------------------------------------
void controlSetOnExit(control_t *control, controlOnExit_t fonExit){

	control->fonExit = fonExit;
}
//-----------------------------------------------------------------------------
int32_t controlRun(control_t *control){

	int32_t ninputs;
	int32_t noutputs;

	if( control->fonEntry ) control->fonEntry();

	ninputs = control->fgetInputs( control->binputs );
	if( ninputs < 0 ) return CONTROL_RUN_STATUS_HARDWARE_ERROR;

	noutputs = control->frun( control->binputs, ninputs, control->boutputs, -1 );
	if( noutputs < 0 ) return CONTROL_RUN_STATUS_CONTROLLER_ERROR;

	control->fapplyOutputs( control->boutputs, noutputs );

	if( control->fonExit ) control->fonExit();

	return CONTROL_RUN_STATUS_SUCCESS;
}
//-----------------------------------------------------------------------------
//=============================================================================
