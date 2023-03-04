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
	control->bprocInputs = config->bprocInputs;
	control->boutputs = config->boutputs;
	control->bprocOutputs = config->bprocOutputs;

	control->fgetInputs = config->fgetInputs;
	control->fprocInputs = config->fprocInputs;

	control->frun = config->frun;

	control->fprocOutputs = config->fprocOutputs;
	control->fapplyOutputs = config->fapplyOutputs;

	control->fonEntry = config->fonEntry;
	control->fonExit = config->fonExit;
}
//-----------------------------------------------------------------------------
void controlSetGetMeasFun(control_t *control, controlGetInputs_t getInputs){

	control->fgetInputs = getInputs;
}
//-----------------------------------------------------------------------------
void controlSetProcMeasFun(control_t *control, controlProcInputs_t procInputs){

	control->fprocInputs = procInputs;
}
//-----------------------------------------------------------------------------
void controlSetRunFun(control_t *control, controlRun_t run){

	control->frun = run;
}
//-----------------------------------------------------------------------------
void controlSetProcOutputsFun(control_t *control, controlProcOutputs_t procOutputs){

	control->fprocOutputs = procOutputs;
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
	int32_t nprocInputs;
	int32_t noutputs;
	int32_t nprocOutputs;

	if( control->fonEntry ) control->fonEntry();

	ninputs = control->fgetInputs( control->binputs );

	nprocInputs = control->fprocInputs( control->binputs, control->bprocInputs, ninputs );

	noutputs = control->frun( control->bprocInputs, nprocInputs, control->boutputs, -1 );

	nprocOutputs = control->fprocOutputs( control->boutputs, control->bprocOutputs, noutputs );

	control->fapplyOutputs( control->bprocOutputs, nprocOutputs );

	if( control->fonExit ) control->fonExit();

	return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
