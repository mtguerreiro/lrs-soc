/*
 * controlsys.c
 *
 *  Created on: 04.09.2022
 *      Author: mguerreiro
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "controlsys.h"

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
void controlsysInitialize(controlsys_t *sys, controlsysConfig_t *config){

	controlConfig_t controlConfig;

	controlConfig.binputs = config->binputs;
	controlConfig.bprocInputs = config->bprocInputs;

	controlConfig.boutputs = config->boutputs;
	controlConfig.bprocOutputs = config->bprocOutputs;

	controlConfig.binputs = config->binputs;
	controlConfig.bprocInputs = config->bprocInputs;

	controlConfig.boutputs = config->boutputs;
	controlConfig.bprocOutputs = config->bprocOutputs;

	controlConfig.fgetInputs = config->fgetInputs;
	controlConfig.fprocInputs = config->fprocInputs;
	controlConfig.fprocOutputs = config->fprocOutputs;
	controlConfig.fapplyOutputs = config->fapplyOutputs;

	controlConfig.frun = config->frun;

	controlConfig.fonEntry = config->fonEntry;
	controlConfig.fonExit = config->fonExit;

	controlInitialize(&sys->control, &controlConfig);

	sys->fhwInterface = config->fhwInterface;
	sys->fhwStatus = config->fhwStatus;
	sys->fcontrollerInterface = config->fcontrollerInterface;
	sys->fcontrollerStatus = config->fcontrollerStatus;

	sys->fenable = config->fenable;
	sys->fdisable = config->fdisable;

	sys->status = CONTROLSYS_STATUS_DISABLED;
}
//-----------------------------------------------------------------------------
int32_t controlsysControllerInterface(controlsys_t *sys,
		void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	if( sys->fcontrollerInterface == 0 ) return CONTROLSYS_ERR_UNDEF;

	return sys->fcontrollerInterface(in, insize, out, maxoutsize);
}
//-----------------------------------------------------------------------------
int32_t controlsysHardwareInterface(controlsys_t *sys,
		void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	if( sys->fhwInterface == 0 ) return CONTROLSYS_ERR_UNDEF;

	return sys->fhwInterface(in, insize, out, maxoutsize);
}
//-----------------------------------------------------------------------------
int32_t controlsysRun(controlsys_t *sys){

	int32_t status;

	if( sys->status == CONTROLSYS_STATUS_ENABLED ) status = controlRun(&sys->control);

	if( status != CONTROL_RUN_STATUS_SUCCESS ){
		if( status == CONTROL_RUN_STATUS_HARDWARE_ERROR)
			sys->status = CONTROLSYS_STATUS_HARDWARE_ERROR;
		else if( status == CONTROL_RUN_STATUS_CONTROLLER_ERROR )
			sys->status = CONTROLSYS_STATUS_HARDWARE_ERROR;
		else
		    sys->status = CONTROLSYS_STATUS_UNKNOWN_RUN_ERROR;
	}

	return sys->status;
}
//-----------------------------------------------------------------------------
int32_t controlsysEnable(controlsys_t *sys){

	if( (sys->fhwStatus() != 0) || (sys->fcontrollerStatus() != 0) ) return -1;

	sys->status = CONTROLSYS_STATUS_ENABLED;

	if( sys->fenable != 0 ) sys->fenable();

	return 0;
}
//-----------------------------------------------------------------------------
void controlsysDisable(controlsys_t *sys){

	sys->status = CONTROLSYS_STATUS_DISABLED;

	if( sys->fdisable != 0 ) sys->fdisable();
}
//-----------------------------------------------------------------------------
int32_t controlsysStatus(controlsys_t *sys){

    return sys->status;
}
//-----------------------------------------------------------------------------
//=============================================================================
