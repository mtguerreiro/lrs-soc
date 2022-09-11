/*
 * soc_control.c
 *
 *  Created on: 02.09.2022
 *      Author: mguerreiro
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "control.h"

#include "controlsys.h"
//=============================================================================


//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

#define CONTROL_CONFIG_N_MEAS		20
#define CONTROL_CONFIG_N_INPUTS		5
#define CONTROL_CONFIG_N_OUTPUTS	5

typedef struct socControl_t{
	uint32_t bmeas[CONTROL_CONFIG_N_MEAS];
	uint32_t bprocMeas[CONTROL_CONFIG_N_MEAS];
	uint32_t binputs[CONTROL_CONFIG_N_INPUTS];
	uint32_t boutputs[CONTROL_CONFIG_N_OUTPUTS];
	uint32_t bprocOutputs[CONTROL_CONFIG_N_OUTPUTS];

	controlOnEntry_t fonEntry;
	controlGetMeas_t fgetMeasurements;
	controlProcMeas_t fprocMeasurements;
	controlGetInputs_t fgetInputs;
	controlRun_t frun;
	controlProcOutputs_t fprocOutputs;
	controlApplyOutputs_t fapplyOutputs;
	controlOnEntry_t fonExit;

	controlSetCtlParams_t fsetCtlParams;
	controlGetCtlParams_t fgetCtlParams;
	controlSetHwParams_t fsetHwParams;
	controlGetHwParams_t fgetHwParams;

}control_t;

control_t control;

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void controlInitialize(void){

	controlsysInitialize();

	control.fgetMeasurements = controlsysGetMeas;
	control.fprocMeasurements = controlsysProcMeas;

	control.fgetInputs = controlsysGetInputs;

	control.frun = controlsysRun;

	control.fprocOutputs = controlsysProcOutputs;
	control.fapplyOutputs = controlsysApplyOutputs;

	control.fsetCtlParams = controlsysSetCtlParams;
	control.fgetCtlParams = controlsysGetCtlParams;
	control.fsetHwParams = controlsysSetHwParams;
	control.fgetHwParams = controlsysGetHwParams;

	control.fonEntry = 0;
	control.fonExit = 0;
}
//-----------------------------------------------------------------------------
void controlRun(void){

	uint32_t nmeas;
	uint32_t ninputs;
	uint32_t noutputs;

	if( control.fonEntry ) control.fonEntry();

	nmeas = control.fgetMeasurements( control.bmeas );

	control.fprocMeasurements( control.bmeas, control.bprocMeas, nmeas );

	ninputs = control.fgetInputs( control.binputs );

	noutputs = control.frun( control.binputs, ninputs,
							 control.bprocMeas, nmeas,
							 control.boutputs);

	control.fprocOutputs( control.boutputs, control.bprocOutputs, noutputs );

	control.fapplyOutputs( control.bprocOutputs, noutputs );

	if( control.fonExit ) control.fonExit();
}
//-----------------------------------------------------------------------------
int32_t controlSetCtlParams(void *params, int32_t n){

	return control.fsetCtlParams( params, n );
}
//-----------------------------------------------------------------------------
int32_t controlGetCtlParams(void *in, void *out){

	int32_t n;
	n = control.fgetCtlParams( in, out );

	return n;
}
//-----------------------------------------------------------------------------
int32_t controlSetHwParams(void *params, int32_t n){

	return control.fsetHwParams( params, n );
}
//-----------------------------------------------------------------------------
int32_t controlGetHwParams(void *in, void *out){

	int32_t n;
	n = control.fgetHwParams( in, out );

	return n;
}
//-----------------------------------------------------------------------------
//=============================================================================
