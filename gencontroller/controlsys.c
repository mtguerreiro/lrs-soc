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

#include "controlsysHwIf.h"
#include "controlsysCtlIf.h"
#include "controlsysInputIf.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static void controlsysInitializeControlLib(void);
//-----------------------------------------------------------------------------
static int32_t controlsysGetMeas(void *meas);
//-----------------------------------------------------------------------------
static void controlsysProcMeas(void *meas, void *procmeas, int32_t n);
//-----------------------------------------------------------------------------
static int32_t controlsysGetInputs(void *inputs);
//-----------------------------------------------------------------------------
static int32_t controlsysExecute(void *inputs, int32_t ninputs, void *meas, int32_t nmeas, void *outputs);
//-----------------------------------------------------------------------------
static void controlsysProcOutputs(void *outputs, void *procoutputs, int32_t n);
//-----------------------------------------------------------------------------
static void controlsysApplyOutputs(void *outputs, int32_t n);
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void controlsysInitialize(void){

	controlsysInitializeControlLib();

	controlsysHwIfInitialize();
	controlsysCtlIfInitialize();
	controlsysInputIfInitialize();
}
//-----------------------------------------------------------------------------
int32_t controlsysSetCtlParams(void *params, int32_t n){

	return controlsysCtlIfSetParams( params, n );
}
//-----------------------------------------------------------------------------
int32_t controlsysGetCtlParams(void *in, void *out){

	return controlsysCtlIfGetParams( in, out );
}
//-----------------------------------------------------------------------------
int32_t controlsysSetHwParams(void *params, int32_t n){

	return controlsysHwIfSetParams( params, n );
}
//-----------------------------------------------------------------------------
int32_t controlsysGetHwParams(void *in, void *out){

	return controlsysHwIfGetParams( in, out );
}
//-----------------------------------------------------------------------------
void controlsysRun(void){

	controlRun();
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static void controlsysInitializeControlLib(void){

	controlSetGetMeasFun( controlsysGetMeas );

	controlSetProcMeasFun( controlsysProcMeas );

	controlSetGetInputsFun( controlsysGetInputs );

	controlSetRunFun( controlsysExecute );

	controlSetProcOutputsFun( controlsysProcOutputs );

	controlSetApplyOutputsFun( controlsysApplyOutputs );
}
//-----------------------------------------------------------------------------
static int32_t controlsysGetMeas(void *meas){

	return controlsysHwIfGetMeas( meas );
}
//-----------------------------------------------------------------------------
static void controlsysProcMeas(void *meas, void *procmeas, int32_t n){

	controlsysHwIfProcMeas( meas, procmeas, n );
}
//-----------------------------------------------------------------------------
static int32_t controlsysGetInputs(void *inputs){

	return controlsysInputIfGet( inputs );
}
//-----------------------------------------------------------------------------
static int32_t controlsysExecute(void *inputs, int32_t ninputs, void *meas, int32_t nmeas, void *outputs){

	return controlsysCtlIfRun( inputs, ninputs, meas, nmeas, outputs);
}
//-----------------------------------------------------------------------------
static void controlsysProcOutputs(void *outputs, void *procoutputs, int32_t n){

	controlsysHwIfProcOutputs( outputs, procoutputs, n );
}
//-----------------------------------------------------------------------------
static void controlsysApplyOutputs(void *outputs, int32_t n){

	controlsysHwIfApplyOutputs( outputs, n );
}
//-----------------------------------------------------------------------------
//=============================================================================
