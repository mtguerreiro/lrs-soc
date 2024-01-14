/*
 * @file ocpPicoCpu0.c
 *
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "ocpPicoCpu0.h"

/* Open controller project */
#include "ocp/ocp/ocpTrace.h"
#include "ocp/ocp/ocpCS.h"
#include "ocp/ocp/ocpIf.h"

#include "ocp/hardware/pico/wz_pico_init.h"

/* Controller lib */
// #include "../controller/controller.h"

//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t ocpPicoCpu0InitializeHw(void);
//-----------------------------------------------------------------------------
static int32_t ocpPicoCpu0InitializeIpc(void);
//-----------------------------------------------------------------------------
static int32_t ocpPicoCpu0InitializeTraces(void);
//-----------------------------------------------------------------------------
static int32_t ocpPicoCpu0InitializeControlSystem(void);
//-----------------------------------------------------------------------------
static int32_t ocpPicoCpu0InitializeInterface(void);
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
// #define OCP_ZYNQ_C0_CONFIG_CPU0_TO_CPU1_ADDR		ZYNQ_CONFIG_MEM_CPU0_TO_CPU1_ADR
// #define OCP_ZYNQ_C0_CONFIG_CPU0_TO_CPU1_SIZE		ZYNQ_CONFIG_MEM_CPU0_TO_CPU1_SIZE

// #define OCP_ZYNQ_C0_CONFIG_CPU1_TO_CPU0_ADDR		ZYNQ_CONFIG_MEM_CPU1_TO_CPU0_ADR
// #define OCP_ZYNQ_C0_CONFIG_CPU1_TO_CPU0_SIZE		ZYNQ_CONFIG_MEM_CPU1_TO_CPU0_SIZE
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void ocpPicoCpu0Initialize(void *params){

	ocpPicoCpu0InitializeHw();
	ocpPicoCpu0InitializeIpc();
	ocpPicoCpu0InitializeTraces();
	ocpPicoCpu0InitializeControlSystem();
	ocpPicoCpu0InitializeInterface();
}
//-----------------------------------------------------------------------------
//=============================================================================


//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t ocpPicoCpu0InitializeHw(void){
	
	return wzPicoInit();
}
//-----------------------------------------------------------------------------
static int32_t ocpPicoCpu0InitializeIpc(void){

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpPicoCpu0InitializeTraces(void){

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
static int32_t ocpPicoCpu0InitializeControlSystem(void){

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
static int32_t ocpPicoCpu0InitializeInterface(void){

	ocpIfInitialize();

	return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
