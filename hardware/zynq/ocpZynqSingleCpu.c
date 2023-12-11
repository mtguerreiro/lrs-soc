/*
 * @file ocpZynqCpu0.c
 *
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "ocpZynqCpu0.h"


/* Open controller project */
#include "ocpConfig.h"
#include "ocpTrace.h"
#include "ocpCS.h"
#include "ocpIf.h"

/* Inter-processor communication */
//#include "../ipc/ipcClient.h"

/* Controller lib */
//#include "../controller/controller.h"

/* Zynq-specific stuff */
//#include "ipcClientZynq.h"

/* */
//#include "soc_defs.h"
#include "zynqConfig.h"
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t ocpZynqSingleCpuInitializeIpc(void *intcInst);
//-----------------------------------------------------------------------------
static int32_t ocpZynqSingleCpuInitializeTraces(void);
//-----------------------------------------------------------------------------
static int32_t ocpZynqSingleCpuInitializeControlSystem(void);
//-----------------------------------------------------------------------------
static int32_t ocpZynqSingleCpuInitializeInterface(void);
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
//#define OCP_ZYNQ_SC_CONFIG_CPU0_TO_CPU1_ADDR		ZYNQ_CONFIG_MEM_CPU0_TO_CPU1_ADR
//#define OCP_ZYNQ_SC_CONFIG_CPU0_TO_CPU1_SIZE		ZYNQ_CONFIG_MEM_CPU0_TO_CPU1_SIZE
//
//#define OCP_ZYNQ_SC_CONFIG_CPU1_TO_CPU0_ADDR		ZYNQ_CONFIG_MEM_CPU1_TO_CPU0_ADR
//#define OCP_ZYNQ_SC_CONFIG_CPU1_TO_CPU0_SIZE		ZYNQ_CONFIG_MEM_CPU1_TO_CPU0_SIZE

#define OCP_ZYNQ_SC_CONFIG_TRACE_0_NAME_LEN         500
#define OCP_ZYNQ_SC_CONFIG_TRACE_0_MAX_SIGNALS      40

#define OCP_ZYNQ_SC_CONFIG_TRACE_0_ADDR             ZYNQ_CONFIG_MEM_TRACE_ADR
#define OCP_ZYNQ_SC_CONFIG_TRACE_0_SIZE             ZYNQ_CONFIG_MEM_TRACE_SIZE_MAX
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static char trace0Names[OCP_ZYNQ_SC_CONFIG_TRACE_0_NAME_LEN];
static size_t trace0Data[OCP_ZYNQ_SC_CONFIG_TRACE_0_MAX_SIGNALS];

static float texec = 0.0f;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void ocpZynqSingleCpuInitialize(void *intcInst){

	ocpZynqSingleCpuInitializeIpc(intcInst);
	ocpZynqSingleCpuInitializeTraces();
	ocpZynqSingleCpuInitializeControlSystem();
	ocpZynqSingleCpuInitializeInterface();
}
//-----------------------------------------------------------------------------
//=============================================================================


//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t ocpZynqSingleCpuInitializeIpc(void *intcInst){

//	ipcClientZynqInitialize(intcInst);
//
//	ipcClientInitialize( ipcClientZynqIrqSend, ipcClientZynqIrqReceive,
//	        ipcClientZynqLock, ipcClientZynqUnlock,
//			OCP_ZYNQ_C0_CONFIG_CPU1_TO_CPU0_ADDR, OCP_ZYNQ_C0_CONFIG_CPU1_TO_CPU0_SIZE,
//			OCP_ZYNQ_C0_CONFIG_CPU0_TO_CPU1_ADDR, OCP_ZYNQ_C0_CONFIG_CPU0_TO_CPU1_SIZE);

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpZynqSingleCpuInitializeTraces(void){

	ocpTraceConfig_t config;

	config.mem = (void *)OCP_ZYNQ_SC_CONFIG_TRACE_0_ADDR;
	config.size = OCP_ZYNQ_SC_CONFIG_TRACE_0_SIZE;
	config.data = (void **)trace0Data;
	config.names = trace0Names;

	ocpTraceInitialize(OCP_TRACE_1, &config, "Trace 1");
//
	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpZynqSingleCpuInitializeControlSystem(void){

//	ocpCSConfig_t config;
//
//	/* Initializes controller lib */
//	controllerInitialize();
//
//	/* Initializes control sys lib */
//	config.fgetInputs = plecsGetInputs;
//	config.fprocInputs = plecsProcInputs;
//
//	config.frun = controllerRun;
//
//	config.fprocOutputs = plecsProcOutputs;
//	config.fapplyOutputs = plecsApplyOutputs;
//
//	config.fonEntry = 0;
//	config.fonExit = 0;
//	config.fhwInterface = 0;
//	config.fcontrollerInterface = controllerInterface;
//
//	config.binputs = sys1InBuffer;
//	config.bprocInputs = sys1ProcInBuffer;
//
//	config.boutputs = sys1OutBuffer;
//	config.bprocOutputs = sys1ProcOutBuffer;
//
//	config.fenable = 0;
//	config.fdisable = 0;
//
//	ocpCSInitialize(OCP_CS_1, &config, "Converter control");
//
	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpZynqSingleCpuInitializeInterface(void){

	ocpIfInitialize();

	return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
