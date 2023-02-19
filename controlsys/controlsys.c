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
	sys->fcontrollerInterface = config->fcontrollerInterface;

	sys->fenable = config->fenable;
	sys->fdisable = config->fdisable;

	sys->enable = 0;
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

	if( sys->enable ) status = controlRun(&sys->control);
	else status = CONTROLSYS_ERR_DISABLED;

	return status;
}
//-----------------------------------------------------------------------------
void controlsysEnable(controlsys_t *sys){

	sys->enable = 1;

	if( sys->fenable != 0 ) sys->fenable();
}
//-----------------------------------------------------------------------------
void controlsysDisable(controlsys_t *sys){

	sys->enable = 0;

	if( sys->fdisable != 0 ) sys->fdisable();
}
//-----------------------------------------------------------------------------
int32_t controlsysStatus(controlsys_t *sys){

	return sys->enable;
}
//-----------------------------------------------------------------------------
//=============================================================================
