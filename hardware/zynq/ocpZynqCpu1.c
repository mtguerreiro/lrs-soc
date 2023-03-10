/*
 * @file ocpZynqCpu1.c
 *
 */

#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "ocpZynqCpu1.h"


/* Open controller project */
#include "ocp.h"
#include "ocpTrace.h"
#include "ocpCS.h"
#include "ocpIf.h"

/* Inter-processor communication */
#include "../ipc/ipcServer.h"

/* Controller lib */
#include "../controller/controller.h"

/* Zynq-specific stuff */
#include "ipcServerZynq.h"

/* */
#include "soc_defs.h"
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeIpc(void *intcInst);
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeTraces(void);
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeControlSystem(void);
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeInterface(void);
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define OCP_ZYNQ_C1_CONFIG_CPU0_TO_CPU1_ADDR		SOC_MEM_CPU0_TO_CPU1_ADR
#define OCP_ZYNQ_C1_CONFIG_CPU0_TO_CPU1_SIZE		SOC_MEM_CPU0_TO_CPU1_SIZE

#define OCP_ZYNQ_C1_CONFIG_CPU1_TO_CPU0_ADDR		SOC_MEM_CPU1_TO_CPU0_ADR
#define OCP_ZYNQ_C1_CONFIG_CPU1_TO_CPU0_SIZE		SOC_MEM_CPU1_TO_CPU0_SIZE

#define OCP_ZYNQ_C1_CONFIG_TRACE_0_ADDR				SOC_MEM_TRACE_ADR
#define OCP_ZYNQ_C1_CONFIG_TRACE_0_SIZE				SOC_MEM_TRACE_SIZE_MAX

#define OCP_ZYNQ_C1_CONFIG_TRACE_0_NAME_LEN			250
#define OCP_ZYNQ_C1_CONFIG_TRACE_0_MAX_SIGNALS		20
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static char trace0Names[OCP_ZYNQ_C1_CONFIG_TRACE_0_NAME_LEN];
static size_t trace0Data[OCP_ZYNQ_C1_CONFIG_TRACE_0_MAX_SIGNALS];
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void ocpZynqCpu1Initialize(void *intcInst){

	ocpZynqCpu1InitializeIpc(intcInst);
	ocpZynqCpu1InitializeTraces();
	ocpZynqCpu1InitializeControlSystem();
	ocpZynqCpu1InitializeInterface();
}
//-----------------------------------------------------------------------------
//=============================================================================


//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeIpc(void *intcInst){


	ipcServerZynqInitialize(intcInst);

	ipcServerInitialize(ocpIf, ipcServerZynqIrqSend,
			SOC_MEM_CPU0_TO_CPU1_ADR, SOC_MEM_CPU0_TO_CPU1_SIZE,
			SOC_MEM_CPU1_TO_CPU0_ADR, SOC_MEM_CPU1_TO_CPU0_SIZE);

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeTraces(void){

	ocpTraceConfig_t config;

	config.mem = OCP_ZYNQ_C1_CONFIG_TRACE_0_ADDR;
	config.size = OCP_ZYNQ_C1_CONFIG_TRACE_0_SIZE;
	config.data = (void **)trace0Data;
	config.names = trace0Names;

	ocpTraceInitialize(OCP_TRACE_1, &config, "Main Trace");
//
	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeControlSystem(void){

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
static int32_t ocpZynqCpu1InitializeInterface(void){

	ocpIfInitialize();

	return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
#endif /* SOC_CPU0 */
