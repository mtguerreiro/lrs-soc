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
#include "ocpConfig.h"
#include "ocpTrace.h"
#include "ocpCS.h"
#include "ocpIf.h"
#include "ocpOpil.h"

/* Inter-processor communication */
#include "../ipc/ipcServer.h"

/* Controller lib */
#include "../controller/controller.h"

/* Zynq-specific stuff */
#include "ipcServerZynq.h"

/* Application */
//#include "afe.h"
//#include "afeIf.h"
//#include "afeHwZynq.h"

//#include "buckOpil.h"
//#include "buckController.h"
//#include "buckHw.h"

//#include "cukOpil.h"
//#include "cukController.h"
//
//#include "cukHwIf.h"
//#include "cukHw.h"
//#include "cukConfig.h"

//#include "cukOpil.h"
#include "boostController.h"

#include "boostHwIf.h"
#include "boostHw.h"
#include "boostConfig.h"

#include "zynqConfig.h"
#include "zynqAxiAdc.h"

#include "benchmarking_zynq.h"
#include "sleep.h"
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeHw(void *intcInst);
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeIpc(void *intcInst);
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeTraces(void);
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeTracesMeas(void);
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeTracesMeasBoost(void);
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeControlSystem(void);
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeControlSystemBoost(void);
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeInterface(void);
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeInterfaceBoost(void);
//-----------------------------------------------------------------------------
void ocpZynqCpu1AdcIrq(void *callbackRef);
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define OCP_ZYNQ_C1_CONFIG_CPU0_TO_CPU1_ADDR		ZYNQ_CONFIG_MEM_CPU0_TO_CPU1_ADR
#define OCP_ZYNQ_C1_CONFIG_CPU0_TO_CPU1_SIZE		ZYNQ_CONFIG_MEM_CPU0_TO_CPU1_SIZE

#define OCP_ZYNQ_C1_CONFIG_CPU1_TO_CPU0_ADDR		ZYNQ_CONFIG_MEM_CPU1_TO_CPU0_ADR
#define OCP_ZYNQ_C1_CONFIG_CPU1_TO_CPU0_SIZE		ZYNQ_CONFIG_MEM_CPU1_TO_CPU0_SIZE

#define OCP_ZYNQ_C1_CONFIG_TRACE_0_ADDR				ZYNQ_CONFIG_MEM_TRACE_ADR
#define OCP_ZYNQ_C1_CONFIG_TRACE_0_SIZE				ZYNQ_CONFIG_MEM_TRACE_SIZE_MAX

#define OCP_ZYNQ_C1_CONFIG_TRACE_0_NAME_LEN			500
#define OCP_ZYNQ_C1_CONFIG_TRACE_0_MAX_SIGNALS		40

#define OCP_ZYNQ_C1_CONFIG_INPUT_BUF_SIZE           50
#define OCP_ZYNQ_C1_CONFIG_OUTPUT_BUG_SIZE          20
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static char trace0Names[OCP_ZYNQ_C1_CONFIG_TRACE_0_NAME_LEN];
static size_t trace0Data[OCP_ZYNQ_C1_CONFIG_TRACE_0_MAX_SIGNALS];

static float bInputs[OCP_ZYNQ_C1_CONFIG_INPUT_BUF_SIZE];
static float bOutputs[OCP_ZYNQ_C1_CONFIG_OUTPUT_BUG_SIZE];

static float texec = 0.0f;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void ocpZynqCpu1Initialize(void *intcInst){

	ocpZynqCpu1InitializeTraces();
	ocpZynqCpu1InitializeIpc(intcInst);
	ocpZynqCpu1InitializeHw(intcInst);
//    ocpZynqCpu1InitializeControlSystem();
//    ocpZynqCpu1InitializeInterface();
    ocpZynqCpu1InitializeControlSystemBoost();
    ocpZynqCpu1InitializeInterfaceBoost();
}
//-----------------------------------------------------------------------------
//=============================================================================


//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeHw(void *intcInst){

    /* Initialize Cuk's hardware */
    //cukHwInitConfig_t config;
    boostHwInitConfig_t config;

    config.intc = intcInst;
    config.irqhandle = ocpZynqCpu1AdcIrq;

    //cukHwInitialize(&config);
    boostHwInitialize(&config);

    /* Initialize timer for benchmarking */
    InitBenchmarking();

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeIpc(void *intcInst){

    /* Initializes inter-processor communication */
    ipcServerZynqInitialize(intcInst);

    ipcServerInitialize(ocpIf, ipcServerZynqIrqSend,
            ZYNQ_CONFIG_MEM_CPU0_TO_CPU1_ADR, ZYNQ_CONFIG_MEM_CPU0_TO_CPU1_SIZE,
            ZYNQ_CONFIG_MEM_CPU1_TO_CPU0_ADR, ZYNQ_CONFIG_MEM_CPU1_TO_CPU0_SIZE);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeTraces(void){

	ocpTraceConfig_t config;

	config.mem = (void *)OCP_ZYNQ_C1_CONFIG_TRACE_0_ADDR;
	config.size = OCP_ZYNQ_C1_CONFIG_TRACE_0_SIZE;
	config.data = (void **)trace0Data;
	config.names = trace0Names;

	ocpTraceInitialize(OCP_TRACE_1, &config, "Main Trace");

	ocpZynqCpu1InitializeTracesMeasBoost();

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeTracesMeas(void){

//    cukConfigMeasurements_t *meas;
//    cukConfigControl_t *outputs;
//
//    /* Adds measurements to trace */
//    meas = (cukConfigMeasurements_t *)bInputs;
//    ocpTraceAddSignal(OCP_TRACE_1, &meas->i_i, "Input current");
//    ocpTraceAddSignal(OCP_TRACE_1, &meas->i_1, "Primary inductor current");
//    ocpTraceAddSignal(OCP_TRACE_1, &meas->v_in, "Input voltage");
//    ocpTraceAddSignal(OCP_TRACE_1, &meas->v_dc, "DC link voltage");
//    ocpTraceAddSignal(OCP_TRACE_1, &meas->v_1, "Primary coupling cap voltage");
//
//    ocpTraceAddSignal(OCP_TRACE_1, &meas->i_o, "Output current");
//    ocpTraceAddSignal(OCP_TRACE_1, &meas->i_2, "Secondary inductor current");
//    ocpTraceAddSignal(OCP_TRACE_1, &meas->v_out, "Output voltage");
//    ocpTraceAddSignal(OCP_TRACE_1, &meas->v_dc_out, "Output DC link voltage");
//    ocpTraceAddSignal(OCP_TRACE_1, &meas->v_2, "Secondary coupling cap voltage");
//
//    /* Adds control signals to trace */
//    outputs = (cukConfigControl_t *)bOutputs;
//    ocpTraceAddSignal(OCP_TRACE_1, &outputs->u, "Duty-cycle");
//    //ocpTraceAddSignal(OCP_TRACE_1, &outputs->sw_o, "Output switch");
//
//    /* Other signals to add */
//    ocpTraceAddSignal(OCP_TRACE_1, &texec, "Exec. time");
//
//    ocpTraceAddSignal(OCP_TRACE_1, &meas->i_i_filt, "Ii filt");
//    ocpTraceAddSignal(OCP_TRACE_1, &meas->i_1_filt, "I1 filt");
//    ocpTraceAddSignal(OCP_TRACE_1, &meas->i_o_filt, "Io filt");
//    ocpTraceAddSignal(OCP_TRACE_1, &meas->i_2_filt, "I2 filt");
//
//    ocpTraceAddSignal(OCP_TRACE_1, &meas->p_in, "Input power");
//    ocpTraceAddSignal(OCP_TRACE_1, &meas->p_out, "Output power");

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeTracesMeasBoost(void){

    boostConfigMeasurements_t *meas;
    boostConfigControl_t *outputs;

    /* Adds measurements to trace */
    meas = (boostConfigMeasurements_t *)bInputs;
    ocpTraceAddSignal(OCP_TRACE_1, &meas->v_dc_in, "Input DC link");
    ocpTraceAddSignal(OCP_TRACE_1, &meas->v_dc_out, "Output DC link");
    ocpTraceAddSignal(OCP_TRACE_1, &meas->v_out, "Output voltage");

    ocpTraceAddSignal(OCP_TRACE_1, &meas->i_l, "Inductor current");
    ocpTraceAddSignal(OCP_TRACE_1, &meas->i_l_avg, "Inductor current (filt)");
    ocpTraceAddSignal(OCP_TRACE_1, &meas->i_o, "Output current current");

    /* Adds control signals to trace */
    outputs = (boostConfigControl_t *)bOutputs;
    //ocpTraceAddSignal(OCP_TRACE_1, &outputs->u, "Duty-cycle");

    /* Other signals to add */
    ocpTraceAddSignal(OCP_TRACE_1, &texec, "Exec. time");
}
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeControlSystem(void){

//	ocpCSConfig_t config;
//	cukControllerConfig_t cukconfig;
//
//    /* Initializes controller and hardware interface libs */
//	cukconfig.disable = cukHwControllerDisable;
//    cukconfig.enable = cukHwControllerEnable;
//	cukControllerInitialize(&cukconfig);
//    cukHwIfInitialize();
//
//    /* Initializes control sys lib */
//    config.binputs = (void *)bInputs;
//    config.boutputs = (void *)bOutputs;
//
//    config.fhwInterface = cukHwIf;
//    config.fhwStatus = cukHwStatus;
//
//    //config.fgetInputs = cukOpilGetMeasurements;
//    config.fgetInputs = cukHwGetMeasurements;
//
//    //config.fapplyOutputs = cukOpilUpdateControl;
//    config.fapplyOutputs = cukHwApplyOutputs;
//
//    config.frun = cukControllerRun;
//    config.fcontrollerInterface = cukControllerInterface;
//    config.fcontrollerStatus = cukControllerStatus;
//
//    config.fenable = cukHwEnable;
//    //config.fenable = 0;
//    config.fdisable = cukHwDisable;
//    //config.fdisable = cukOpilDisable;
//
//    config.fonEntry = 0;
//    config.fonExit = 0;
//
//	ocpCSInitialize(OCP_CS_1, &config, "Converter control");
//
//	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeControlSystemBoost(void){

    ocpCSConfig_t config;
    boostControllerConfig_t boostconfig;

    /* Initializes controller and hardware interface libs */
    boostconfig.disable = boostHwControllerDisable;
    boostconfig.enable = boostHwControllerEnable;
    boostControllerInitialize(&boostconfig);
    boostHwIfInitialize();

    /* Initializes control sys lib */
    config.binputs = (void *)bInputs;
    config.boutputs = (void *)bOutputs;

    config.fhwInterface = boostHwIf;
    config.fhwStatus = boostHwStatus;

    //config.fgetInputs = boostOpilGetMeasurements;
    config.fgetInputs = boostHwGetMeasurements;

    //config.fapplyOutputs = boostOpilUpdateControl;
    config.fapplyOutputs = boostHwApplyOutputs;

    config.frun = boostControllerRun;
    config.fcontrollerInterface = boostControllerInterface;
    config.fcontrollerStatus = boostControllerStatus;

    config.fenable = boostHwEnable;
    //config.fenable = 0;
    config.fdisable = boostHwDisable;
    //config.fdisable = cukOpilDisable;

    config.fonEntry = 0;
    config.fonExit = 0;

    ocpCSInitialize(OCP_CS_1, &config, "Boost control");

    return 0;
}
//-----------------------------------------------------------------------------
//static int32_t ocpZynqCpu1InitializeInterface(void){
//
//    /* Initializes OPiL interface */
//    ocpOpilConfig_t config;
//
//    config.updateMeas = cukOpilUpdateMeasurements;
//    config.updateSimData = cukOpilUpdateSimData;
//
//    config.initControl = 0;
//    config.runControl = ocpZynqCpu1AdcIrq;
//
//    config.getControl = cukOpilGetControl;
//    config.getControllerData = cukOpilGetControllerData;
//
//    ocpOpilInitialize(&config);
//
//    /* Initializes OCP interface */
//    ocpIfInitialize();
//
//	return 0;
//}
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeInterfaceBoost(void){

    /* Initializes OPiL interface */
    ocpOpilConfig_t config;

    config.updateMeas = 0;
    config.updateSimData = 0;

    config.initControl = 0;
    config.runControl = ocpZynqCpu1AdcIrq;

    config.getControl = 0;
    config.getControllerData = 0;

    ocpOpilInitialize(&config);

    /* Initializes OCP interface */
    ocpIfInitialize();

    return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------------- IRQ -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void ocpZynqCpu1AdcIrq(void *callbackRef){

    uint32_t ticks;

    ticks = GetTicks();

    ocpCSRun(OCP_CS_1);
    ocpTraceSave(OCP_TRACE_1);

    ticks = ticks - GetTicks();
    texec = TicksToS(ticks) / 1e-6;
}
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* SOC_CPU1 */
