/*
 * ocp.c
 *
 *  Created on: 19 de fev de 2023
 *      Author: marco
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "ocp.h"

/* Open controller project */
#include "ocpTrace.h"
#include "ocpCS.h"
#include "ocpIf.h"

/* Inter-processor communication */
#include "../ipc/ipcServer.h"
#include "../ipc/ipcClient.h"

/* Controller lib */
#include "../controller/controller.h"

/* Hardware lib */
#include "../hardware/plecs/plecs.h"
//#include "plecs.h"

#include "stddef.h"
//=============================================================================

//=============================================================================
/*-------------------------------- Definitions ------------------------------*/
//=============================================================================


//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t ocpInitializeTraces(void);
//-----------------------------------------------------------------------------
int32_t ocpInitializeControlSystem(void);
//-----------------------------------------------------------------------------
int32_t ocpInitializeInterface(void);
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================

/* Trace #1 */
static float trace1Buffer[20];
static float *trace1Data[8];
static char trace1Names[30];
#define OCP_CONFIG_TRACE1_SIZE		(1024)

/* Control system #1 */
static float sys1InBuffer[10];
static float sys1ProcInBuffer[10];

static float sys1OutBuffer[10];
static float sys1ProcOutBuffer[10];

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t ocpInitialize(void){


	ocpInitializeTraces();
	ocpInitializeControlSystem();
	ocpInitializeInterface();

	return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t ocpInitializeTraces(void){

	ocpTraceConfig_t config;

	config.mem = trace1Buffer;
	config.size = OCP_CONFIG_TRACE1_SIZE;
	config.data = (void **)trace1Data;
	config.names = trace1Names;

	ocpTraceInitialize(OCP_TRACE_1, &config, "Trace 1");

	return 0;
}
//-----------------------------------------------------------------------------
int32_t ocpInitializeControlSystem(void){

	ocpCSConfig_t config;

	/* Initializes controller lib */
	controllerInitialize();

	/* Initializes control sys lib */
	config.fgetInputs = plecsGetInputs;
	config.fprocInputs = plecsProcInputs;

	config.frun = controllerRun;

	config.fprocOutputs = plecsProcOutputs;
	config.fapplyOutputs = plecsApplyOutputs;

	config.fonEntry = 0;
	config.fonExit = 0;
	config.fhwInterface = 0;
	config.fcontrollerInterface = controllerInterface;

	config.binputs = sys1InBuffer;
	config.bprocInputs = sys1ProcInBuffer;

	config.boutputs = sys1OutBuffer;
	config.bprocOutputs = sys1ProcOutBuffer;

	config.fenable = 0;
	config.fdisable = 0;

	ocpCSInitialize(OCP_CS_1, &config, "Converter control");

	return 0;
}
//-----------------------------------------------------------------------------
int32_t ocpInitializeInterface(void){

	ocpIfInitialize();

	return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
