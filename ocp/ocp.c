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

#include "ipcServer.h"
#include "ipcClient.h"
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

/* Control system #1 */
static float sys2InBuffer[3];
static float sys2ProcInBuffer[3];

static float sys2OutBuffer[3];
static float sys2ProcOutBuffer[3];

//=============================================================================

/* Simulating IPC */
static uint32_t icpBufferServerClient[100];
static uint32_t icpBufferClientServer[100];

void ipcTestClientIrqSend(void){

	ipcServerRequest();
}

void ipcTestClientIrqReceive(void){

}


void ipcTestServerIrqSend(void){

	ipcTestClientIrqReceive();
}

int32_t ipcTestServerIrqReceive(void *req, int32_t reqsize, void **resp, int32_t maxrespsize){

	return ocpIf(req, reqsize, resp, maxrespsize);
}
//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t ocpInitialize(void){


	ocpInitializeTraces();
	ocpInitializeControlSystem();
	ocpInitializeInterface();

	ipcClientInitialize(ipcTestClientIrqSend, ipcTestClientIrqReceive, icpBufferServerClient, 400, icpBufferClientServer, 400);

	ipcServerInitialize(ipcTestServerIrqReceive, ipcTestServerIrqSend, icpBufferClientServer, 400, ipcTestClientIrqReceive, 400);

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

	config.fenable = 0;
	config.fdisable = 0;

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

	config.binputs = sys2InBuffer;
	config.bprocInputs = sys2ProcInBuffer;

	config.boutputs = sys2OutBuffer;
	config.bprocOutputs = sys2ProcOutBuffer;

	config.fenable = 0;
	config.fdisable = 0;


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
