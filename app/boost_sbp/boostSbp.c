

#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "boostSbp.h"

/* Open controller project */
#include "ocpConfig.h"
#include "ocpTrace.h"
#include "ocpCS.h"
#include "ocpIf.h"
#include "ocpOpil.h"

/* Controller lib */
#include "controller/controller.h"

/* Application */
#include "boostSbpConfig.h"

//#include "fsBuckBoostOpil.h"
#include "boostSbpController.h"

#include "boostSbpHwIf.h"
#include "boostSbpHw.h"
#include "boostSbpConfig.h"

/* Zynq */
#include "zynqConfig.h"

/* Benchmarking */
#include "benchmarking_zynq.h"
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t boostSbpInitializeHw(void *intcInst);
//-----------------------------------------------------------------------------
static int32_t boostSbpInitializeTrace(void);
//-----------------------------------------------------------------------------
static int32_t boostSbpInitializeTraceSignals(void);
//-----------------------------------------------------------------------------
static int32_t boostSbpInitializeControlSystem(void);
//-----------------------------------------------------------------------------
void boostSbpAdcIrq(void *callbackRef);
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static char traceNames[BOOST_SBP_CONFIG_TRACE_0_NAME_LEN];
static size_t traceData[BOOST_SBP_CONFIG_TRACE_0_MAX_SIGNALS];

static float bInputs[BOOST_SBP_CONFIG_INPUT_BUF_SIZE];
static float bOutputs[BOOST_SBP_CONFIG_OUTPUT_BUG_SIZE];

static float texec_boost_sbp = 0.0f;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void boostSbpInit(void *intcInst){

    boostSbpInitializeTrace();

    boostSbpInitializeHw(intcInst);
    boostSbpInitializeControlSystem();
}
//-----------------------------------------------------------------------------
//=============================================================================


//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t boostSbpInitializeTrace(void){

    ocpTraceConfig_t config;

    config.mem = (void *)BOOST_SBP_CONFIG_TRACE_ADDR;
    config.size = BOOST_SBP_CONFIG_TRACE_SIZE;
    config.data = (void **)traceData;
    config.names = traceNames;

    ocpTraceInitialize(BOOST_SBP_CONFIG_TRACE_ID, &config, "Boost SBP trace");

    boostSbpInitializeTraceSignals();

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpInitializeHw(void *intcInst){

    /* Initialize the hardware */
    boostSbpHwInitConfig_t hwConfig;

    hwConfig.intc = intcInst;
    hwConfig.irqhandle = boostSbpAdcIrq;

    boostSbpHwInitialize(&hwConfig);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpInitializeTraceSignals(void){

    boostSbpConfigMeasurements_t *meas;
    boostSbpConfigControl_t *outputs;

    /* Adds measurements to trace */
    meas = (boostSbpConfigMeasurements_t *)bInputs;

    ocpTraceAddSignal(BOOST_SBP_CONFIG_TRACE_ID, &meas->ii, "Input current");
    ocpTraceAddSignal(BOOST_SBP_CONFIG_TRACE_ID, &meas->il, "Inductor current");
    ocpTraceAddSignal(BOOST_SBP_CONFIG_TRACE_ID, &meas->io, "Output current");

    ocpTraceAddSignal(BOOST_SBP_CONFIG_TRACE_ID, &meas->vi, "Input voltage");
    ocpTraceAddSignal(BOOST_SBP_CONFIG_TRACE_ID, &meas->vi_dc, "Input DC link");
    ocpTraceAddSignal(BOOST_SBP_CONFIG_TRACE_ID, &meas->vo_dc, "Output DC link");
    ocpTraceAddSignal(BOOST_SBP_CONFIG_TRACE_ID, &meas->vo, "Output voltage");

    /* Adds control signals to trace */
    outputs = (boostSbpConfigControl_t *)bOutputs;
    ocpTraceAddSignal(BOOST_SBP_CONFIG_TRACE_ID, &outputs->u, "Duty-cycle");

    /* Other signals to add */
    ocpTraceAddSignal(BOOST_SBP_CONFIG_TRACE_ID, &texec_boost_sbp, "Exec. time");

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpInitializeControlSystem(void){

    ocpCSConfig_t config;

    /* Initializes controller and hardware interface libs */
    boostSbpControllerInit();
    boostSbpHwIfInitialize();

    /* Initializes control sys lib */
    config.binputs = (void *)bInputs;
    config.boutputs = (void *)bOutputs;

    config.fhwInterface = boostSbpHwIf;
    config.fhwStatus = boostSbpHwStatus;

    //config.fgetInputs = buckOpilGetMeasurements;
    config.fgetInputs = boostSbpHwGetMeasurements;

    //config.fapplyOutputs = buckOpilUpdateControl;
    config.fapplyOutputs = boostSbpHwApplyOutputs;

    config.frun = boostSbpControllerRun;
    config.fcontrollerInterface = boostSbpControllerIf;
    config.fcontrollerStatus = boostSbpControllerStatus;

    config.fenable = boostSbpHwEnable;
    //config.fenable = 0;
    config.fdisable = boostSbpHwDisable;
    //config.fdisable = buckOpilDisable;

    config.fonEntry = 0;
    config.fonExit = 0;

    ocpCSInitialize(BOOST_SBP_CONFIG_CS_ID, &config, "Boost SBP control");

    return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------------- IRQ -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void boostSbpAdcIrq(void *callbackRef){

    uint32_t ticks;

    ticks = GetTicks();

    ocpCSRun(BOOST_SBP_CONFIG_CS_ID);
    ocpTraceSave(BOOST_SBP_CONFIG_TRACE_ID);

    ticks = ticks - GetTicks();
    texec_boost_sbp = TicksToS(ticks) / 1e-6;
}
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* SOC_CPU1 */
