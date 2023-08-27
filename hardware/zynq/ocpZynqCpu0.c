/*
 * @file ocpZynqCpu0.c
 *
 */

#ifdef SOC_CPU0
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "ocpZynqCpu0.h"


/* Open controller project */
#include "ocpTrace.h"
#include "ocpCS.h"
#include "ocpIf.h"

/* Inter-processor communication */
#include "../ipc/ipcClient.h"

/* Controller lib */
#include "../controller/controller.h"

/* Zynq-specific stuff */
#include "ipcClientZynq.h"

/* */
//#include "soc_defs.h"
#include "zynqConfig.h"
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu0InitializeIpc(void *intcInst);
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu0InitializeTraces(void);
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu0InitializeControlSystem(void);
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu0InitializeInterface(void);
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define OCP_ZYNQ_C0_CONFIG_CPU0_TO_CPU1_ADDR		ZYNQ_CONFIG_MEM_CPU0_TO_CPU1_ADR
#define OCP_ZYNQ_C0_CONFIG_CPU0_TO_CPU1_SIZE		ZYNQ_CONFIG_MEM_CPU0_TO_CPU1_SIZE

#define OCP_ZYNQ_C0_CONFIG_CPU1_TO_CPU0_ADDR		ZYNQ_CONFIG_MEM_CPU1_TO_CPU0_ADR
#define OCP_ZYNQ_C0_CONFIG_CPU1_TO_CPU0_SIZE		ZYNQ_CONFIG_MEM_CPU1_TO_CPU0_SIZE
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void ocpZynqCpu0Initialize(void *intcInst){

	ocpZynqCpu0InitializeIpc(intcInst);
	ocpZynqCpu0InitializeTraces();
	ocpZynqCpu0InitializeControlSystem();
	ocpZynqCpu0InitializeInterface();
}
//-----------------------------------------------------------------------------
//=============================================================================


//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu0InitializeIpc(void *intcInst){

	ipcClientZynqInitialize(intcInst);

	ipcClientInitialize( ipcClientZynqIrqSend, ipcClientZynqIrqReceive,
	        ipcClientZynqLock, ipcClientZynqUnlock,
			OCP_ZYNQ_C0_CONFIG_CPU1_TO_CPU0_ADDR, OCP_ZYNQ_C0_CONFIG_CPU1_TO_CPU0_SIZE,
			OCP_ZYNQ_C0_CONFIG_CPU0_TO_CPU1_ADDR, OCP_ZYNQ_C0_CONFIG_CPU0_TO_CPU1_SIZE);

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu0InitializeTraces(void){

//	ocpTraceConfig_t config;
//
//	config.mem = trace1Buffer;
//	config.size = OCP_CONFIG_TRACE1_SIZE;
//	config.data = (void **)trace1Data;
//	config.names = trace1Names;
//
//	ocpTraceInitialize(OCP_TRACE_1, &config, "Trace 1");
//
	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu0InitializeControlSystem(void){

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
static int32_t ocpZynqCpu0InitializeInterface(void){

	ocpIfInitialize();

	return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
#endif /* SOC_CPU0 */
