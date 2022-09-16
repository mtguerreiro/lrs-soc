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

}control_t;

control_t control = {.fonEntry = 0, .fonExit = 0};

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void controlSetGetMeasFun(controlGetMeas_t getmeas){

	control.fgetMeasurements = getmeas;
}
//-----------------------------------------------------------------------------
void controlSetProcMeasFun(controlProcMeas_t procmeas){

	control.fprocMeasurements = procmeas;
}
//-----------------------------------------------------------------------------
void controlSetGetInputsFun(controlGetInputs_t getinputs){

	control.fgetInputs = getinputs;
}
//-----------------------------------------------------------------------------
void controlSetRunFun(controlRun_t run){

	control.frun = run;
}
//-----------------------------------------------------------------------------
void controlSetProcOutputsFun(controlProcOutputs_t procOutputs){

	control.fprocOutputs = procOutputs;
}
//-----------------------------------------------------------------------------
void controlSetApplyOutputsFun(controlApplyOutputs_t applyOutputs){

	control.fapplyOutputs = applyOutputs;
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
//=============================================================================
