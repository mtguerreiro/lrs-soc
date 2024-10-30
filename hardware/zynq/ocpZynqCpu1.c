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

#include "boostOpil.h"
#include "boostController.h"

#include "boostHwIf.h"
#include "boostHw.h"
#include "boostConfig.h"

#include "buckController.h"

#include "buckHwIf.h"
#include "buckHw.h"
#include "buckConfig.h"

#include "zynqConfig.h"
#include "zynqAxiAdc.h"

#include "benchmarking_zynq.h"
#include "sleep.h"

int irq4_flag = 0;
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
static int32_t ocpZynqCpu1InitializeTracesMeasBuck(void);
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeControlSystem(void);
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeControlSystemBoost(void);
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeControlSystemBuck(void);
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeInterface(void);
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeInterfaceBoost(void);
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeInterfaceBuck(void);
//-----------------------------------------------------------------------------
void ocpZynqCpu1AdcIrq0(void *callbackRef);
//-----------------------------------------------------------------------------
void ocpZynqCpu1AdcIrq(void *callbackRef);
//-----------------------------------------------------------------------------
void ocpZynqCpu1AdcIrq2(void *callbackRef);
//-----------------------------------------------------------------------------
void ocpZynqCpu1AdcIrq3(void *callbackRef);
//-----------------------------------------------------------------------------
void ocpZynqCpu1AdcIrq4(void *callbackRef);
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
#define OCP_ZYNQ_C1_CONFIG_OUTPUT_BUF_SIZE          20
#define OCP_ZYNQ_C1_CONFIG_REFERENCE_BUF_SIZE       20
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static char trace0Names[OCP_ZYNQ_C1_CONFIG_TRACE_0_NAME_LEN];
static size_t trace0Data[OCP_ZYNQ_C1_CONFIG_TRACE_0_MAX_SIGNALS];

static char trace1Names[OCP_ZYNQ_C1_CONFIG_TRACE_0_NAME_LEN];
static size_t trace1Data[OCP_ZYNQ_C1_CONFIG_TRACE_0_MAX_SIGNALS];

static float bInputs1[OCP_ZYNQ_C1_CONFIG_INPUT_BUF_SIZE];
static float bOutputs1[OCP_ZYNQ_C1_CONFIG_OUTPUT_BUF_SIZE];

static float bInputs2[OCP_ZYNQ_C1_CONFIG_INPUT_BUF_SIZE];
static float bOutputs2[OCP_ZYNQ_C1_CONFIG_OUTPUT_BUF_SIZE];

static float texec_buck = 0.0f;
static float texec_boost_sw = 0.0f, texec_boost_ctl = 0.0f;

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
    ocpZynqCpu1InitializeControlSystemBuck();
    //ocpZynqCpu1InitializeInterfaceBuck();

    ocpIfInitialize();
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
    boostHwInitConfig_t boostHwConfig;
    buckHwInitConfig_t buckHwConfig;

    boostHwConfig.intc = intcInst;
    boostHwConfig.irqhandle0 = ocpZynqCpu1AdcIrq0;
    boostHwConfig.irqhandle1 = ocpZynqCpu1AdcIrq;
    boostHwConfig.irqhandle3 = ocpZynqCpu1AdcIrq3;
    boostHwConfig.irqhandle4 = ocpZynqCpu1AdcIrq4;

    //cukHwInitialize(&config);
    boostHwInitialize(&boostHwConfig);

    buckHwConfig.intc = intcInst;
    buckHwConfig.irqhandle2 = ocpZynqCpu1AdcIrq2;
    buckHwInitialize(&buckHwConfig);

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

	ocpTraceConfig_t boostTrConfig, buckTrConfig;

	uint32_t boostAddr, boostSize;
	uint32_t buckAddr, buckSize;

	boostAddr = OCP_ZYNQ_C1_CONFIG_TRACE_0_ADDR;
	boostSize = OCP_ZYNQ_C1_CONFIG_TRACE_0_SIZE >> 1;

	boostTrConfig.mem = (void *)boostAddr;
	boostTrConfig.size = boostSize;
	boostTrConfig.data = (void **)trace0Data;
	boostTrConfig.names = trace0Names;

	ocpTraceInitialize(OCP_TRACE_1, &boostTrConfig, "Boost trace");

    ocpZynqCpu1InitializeTracesMeasBoost();

    buckAddr = boostAddr + boostSize;
    buckSize = OCP_ZYNQ_C1_CONFIG_TRACE_0_SIZE >> 1;

    buckTrConfig.mem = (void *)buckAddr;
    buckTrConfig.size = buckSize;
    buckTrConfig.data = (void **)trace1Data;
    buckTrConfig.names = trace1Names;

    ocpTraceInitialize(OCP_TRACE_2, &buckTrConfig, "Buck trace");

    ocpZynqCpu1InitializeTracesMeasBuck();

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
    //boostConfigReferences_t *references; //added

    /* Adds measurements to trace */
    meas = (boostConfigMeasurements_t *)bInputs1;
    ocpTraceAddSignal(OCP_TRACE_1, &meas->v_in, "Input voltage");
    ocpTraceAddSignal(OCP_TRACE_1, &meas->v_dc_in, "Input DC link");
    ocpTraceAddSignal(OCP_TRACE_1, &meas->v_dc_out, "Output DC link");
    ocpTraceAddSignal(OCP_TRACE_1, &meas->v_out, "Output voltage");

    ocpTraceAddSignal(OCP_TRACE_1, &meas->i_l, "Inductor current");
    ocpTraceAddSignal(OCP_TRACE_1, &meas->i_o, "Output current");

    /* Adds control signals to trace */
    outputs = (boostConfigControl_t *)bOutputs1;
    ocpTraceAddSignal(OCP_TRACE_1, &outputs->u, "Duty-cycle");

    /* Other signals to add */
    ocpTraceAddSignal(OCP_TRACE_1, &texec_boost_sw, "Sw exec. time");
	ocpTraceAddSignal(OCP_TRACE_1, &texec_boost_ctl, "Ctl exec. time");

    /* Other signals to add */
    ocpTraceAddSignal(OCP_TRACE_1, &outputs->v_o_reference, "Output voltage Reference");
    ocpTraceAddSignal(OCP_TRACE_1, &outputs->e, "Energy");
    ocpTraceAddSignal(OCP_TRACE_1, &outputs->e_reference, "Energy Reference");
    ocpTraceAddSignal(OCP_TRACE_1, &outputs->i_o_filt, "Output current filtered");
    ocpTraceAddSignal(OCP_TRACE_1, &outputs->p_o, "Output power");
    ocpTraceAddSignal(OCP_TRACE_1, &outputs->e_dot, "E_dot");
    //ocpTraceAddSignal(OCP_TRACE_1, &outputs->rho_1, "rho_1 (linearization)");
    //ocpTraceAddSignal(OCP_TRACE_1, &outputs->i_l_filt, "i_l_filt");

    /* Other signals to add */
    //references = (boostConfigReferences_t *)bOutputs;
    //ocpTraceAddSignal(OCP_TRACE_1, &references->v_o, "Reference");
    
    return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeTracesMeasBuck(void){

    buckConfigMeasurements_t *meas;
    buckConfigControl_t *outputs;

    /* Adds measurements to trace */
    meas = (buckConfigMeasurements_t *)bInputs2;
    ocpTraceAddSignal(OCP_TRACE_2, &meas->v_in, "Input voltage");
    ocpTraceAddSignal(OCP_TRACE_2, &meas->v_dc_in, "Input DC link");
    ocpTraceAddSignal(OCP_TRACE_2, &meas->v_dc_out, "Output DC link");
    ocpTraceAddSignal(OCP_TRACE_2, &meas->v_out, "Output voltage");

    ocpTraceAddSignal(OCP_TRACE_2, &meas->i_l, "Inductor current");
    ocpTraceAddSignal(OCP_TRACE_2, &meas->i_o, "Output current current");

    /* Adds control signals to trace */
    outputs = (buckConfigControl_t *)bOutputs2;
    ocpTraceAddSignal(OCP_TRACE_2, &outputs->u, "Duty-cycle");

    /* Other signals to add */
    ocpTraceAddSignal(OCP_TRACE_2, &texec_buck, "Exec. time");
    ocpTraceAddSignal(OCP_TRACE_2, &outputs->v_o_reference, "Output voltage Reference");
    return 0;
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
    config.binputs = (void *)bInputs1;
    config.boutputs = (void *)bOutputs1;


    config.fhwInterface = boostHwIf;
    config.fhwStatus = boostHwStatus;
//for HW
    config.fgetInputs = boostHwGetMeasurements;
    config.fapplyOutputs = boostHwApplyOutputs;
    config.fenable = boostHwEnable;
    config.fdisable = boostHwDisable;
//for simulation
    //config.fgetInputs = boostOpilGetMeasurements;
    //config.fapplyOutputs = boostOpilUpdateControl;
    //config.fenable = 0;
    //config.fdisable = boostOpilDisable;

    config.frun = boostControllerRun;
    config.fcontrollerInterface = boostControllerInterface;
    config.fcontrollerStatus = boostControllerStatus;

    config.fonEntry = 0;
    config.fonExit = 0;

    ocpCSInitialize(OCP_CS_1, &config, "Boost control");

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeControlSystemBuck(void){

    ocpCSConfig_t config;
    buckControllerConfig_t buckconfig;

    /* Initializes controller and hardware interface libs */
    buckconfig.disable = buckHwControllerDisable;
    buckconfig.enable = buckHwControllerEnable;
    buckControllerInitialize(&buckconfig);
    buckHwIfInitialize();

    /* Initializes control sys lib */
    config.binputs = (void *)bInputs2;
    config.boutputs = (void *)bOutputs2;

    config.fhwInterface = buckHwIf;
    config.fhwStatus = buckHwStatus;

    //config.fgetInputs = buckOpilGetMeasurements;
    config.fgetInputs = buckHwGetMeasurements;

    //config.fapplyOutputs = buckOpilUpdateControl;
    config.fapplyOutputs = buckHwApplyOutputs;

    config.frun = buckControllerRun;
    config.fcontrollerInterface = buckControllerInterface;
    config.fcontrollerStatus = buckControllerStatus;

    config.fenable = buckHwEnable;
    //config.fenable = 0;
    config.fdisable = buckHwDisable;
    //config.fdisable = buckOpilDisable;

    config.fonEntry = 0;
    config.fonExit = 0;

    ocpCSInitialize(OCP_CS_2, &config, "Buck control");

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

    config.updateMeas = boostOpilUpdateMeasurements;
    config.updateSimData = boostOpilUpdateSimData;

    config.initControl = 0;
    config.runControl = ocpZynqCpu1AdcIrq;

    config.getControl = boostOpilGetControl;
    config.getControllerData = boostOpilGetControllerData;

    ocpOpilInitialize(&config);

    /* Initializes OCP interface */
    //ocpIfInitialize();

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeInterfaceBuck(void){

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
    //ocpIfInitialize();

    return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------------- IRQ -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void ocpZynqCpu1AdcIrq(void *callbackRef){

	static uint32_t ticks = 0;

    ticks = GetTicks();

    ocpCSRun(OCP_CS_1);
    ocpTraceSave(OCP_TRACE_1);

    ticks = ticks - GetTicks();
    texec_boost_sw = TicksToS(ticks) / 1e-6;
}
//-----------------------------------------------------------------------------
void ocpZynqCpu1AdcIrq3(void *callbackRef){

	static uint32_t ticks = 0;
	//texec_boost_ctl = TicksToS(ticks - GetTicks())/ 1e-6; //this measures time between interruptions (control period), disable this and enable last one to measure exec time of energy controller of the MPC (discounting linearization part)
	ticks = GetTicks();

	uint32_t currentController = get_active_controller();

	if (currentController == 6){

		boostConfigMeasurements_t *meas;
		boostConfigControl_t *outputs;
		meas = (boostConfigMeasurements_t *)bInputs1;
		outputs = (boostConfigControl_t *)bOutputs1;

		//boostControlEnergycint_rhoRun(meas, outputs);
		boostControl_BF_MPC_rho_Run(meas, outputs);
	}

	texec_boost_ctl = TicksToS(ticks - GetTicks()) / 1e-6;

}

//-----------------------------------------------------------------------------
void ocpZynqCpu1AdcIrq0(void *callbackRef){

   set_mpc_flag();

}

//-----------------------------------------------------------------------------
void ocpZynqCpu1AdcIrq4(void *callbackRef){ //function for protection trip in boost converter

	irq4_flag++;

}

//-----------------------------------------------------------------------------

void ocpZynqCpu1AdcIrq2(void *callbackRef){

    uint32_t ticks;

    ticks = GetTicks();

    ocpCSRun(OCP_CS_2);
    ocpTraceSave(OCP_TRACE_2);

    ticks = ticks - GetTicks();
    texec_buck = TicksToS(ticks) / 1e-6;
}
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* SOC_CPU1 */
