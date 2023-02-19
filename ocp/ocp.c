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

#include "ocpTrace.h"
#include "ocpCS.h"
#include "ocpIf.h"

#include "stdio.h"
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
static float *trace1Data[3];
static char trace1Names[30];
#define OCP_CONFIG_TRACE1_SIZE		(3 * 4)

/* Trace #2 */
static float trace2Buffer[20];
static float *trace2Data[3];
static char trace2Names[30];
#define OCP_CONFIG_TRACE2_SIZE		(30 * 4)

/* Control system #1 */
static float sys1InBuffer[3];
static float sys1ProcInBuffer[3];

static float sys1OutBuffer[3];
static float sys1ProcOutBuffer[3];

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

	ocpTraceInitialize(OCP_TRACE_1, &config, "trace 1");

	config.mem = trace2Buffer;
	config.size = OCP_CONFIG_TRACE2_SIZE;
	config.data = (void **)trace2Data;
	config.names = trace2Names;

	ocpTraceInitialize(OCP_TRACE_2, &config, "trace 2");


	float x = 0.1f; float y = 0.2f; float z = 212334.3402f;

	ocpTraceAddSignal(OCP_TRACE_1, (void *)&x, "xis");
	ocpTraceAddSignal(OCP_TRACE_1, (void *)&y, "why");
	ocpTraceAddSignal(OCP_TRACE_1, (void *)&z, "zhi");

	printf("z's value: %f\n", z);

	ocpTraceSave(OCP_TRACE_1);

	return 0;
}
//-----------------------------------------------------------------------------
int32_t ocpInitializeControlSystem(void){

	ocpCSConfig_t config;

	config.fgetInputs = 0;
	config.fprocInputs = 0;

	config.frun = 0;

	config.fprocOutputs = 0;
	config.fapplyOutputs = 0;

	config.fonEntry = 0;
	config.fonExit = 0;
	config.fhwInterface = 0;
	config.fcontrollerInterface = 0;

	config.binputs = sys1InBuffer;
	config.bprocInputs = sys1ProcInBuffer;

	config.boutputs = sys1OutBuffer;
	config.bprocOutputs = sys1ProcOutBuffer;

	ocpCSInitialize(OCP_CS_1, &config, "temp control");

	config.fgetInputs = 0;
	config.fprocInputs = 0;

	config.frun = 0;

	config.fprocOutputs = 0;
	config.fapplyOutputs = 0;

	config.fonEntry = 0;
	config.fonExit = 0;
	config.fhwInterface = 0;
	config.fcontrollerInterface = 0;

	config.binputs = sys1InBuffer;
	config.bprocInputs = sys1ProcInBuffer;

	config.boutputs = sys1OutBuffer;
	config.bprocOutputs = sys1ProcOutBuffer;

	ocpCSInitialize(OCP_CS_2, &config, "another control");

	return 0;
}
//-----------------------------------------------------------------------------
int32_t ocpInitializeInterface(void){

	ocpIfInitialize();

	return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
