/*
 * ocpWin.c
 *
 *  Created on: 06.10.2023
 *      Author: mguerreiro
 */


//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "ocpWin.h"


/* Open controller project */
#include "ocpConfig.h"
#include "ocpTrace.h"
#include "ocpCS.h"
#include "ocpIf.h"
#include "ocpOpil.h"

/* Controller lib */
#include "../controller/controller.h"


#include "cukOpil.h"
#include "cukController.h"

//#include "cukHwIf.h"
#include "cukHwWin.h"
#include "cukConfig.h"

//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t ocpWinInitializeTraces(void);
//-----------------------------------------------------------------------------
static int32_t ocpWinInitializeTracesMeas(void);
//-----------------------------------------------------------------------------
static int32_t ocpWinInitializeControlSystem(void);
//-----------------------------------------------------------------------------
static int32_t ocpWinInitializeInterface(void);
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

#define OCP_WIN_CONFIG_TRACE_0_SIZE             (40 * 1024 * 1024)

#define OCP_WIN_CONFIG_TRACE_0_NAME_LEN         500
#define OCP_WIN_CONFIG_TRACE_0_MAX_SIGNALS      20

#define OCP_WIN_CONFIG_INPUT_BUF_SIZE           50
#define OCP_WIN_CONFIG_OUTPUT_BUG_SIZE          20

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static char trace0Names[OCP_WIN_CONFIG_TRACE_0_NAME_LEN];
static size_t trace0Data[OCP_WIN_CONFIG_TRACE_0_MAX_SIGNALS];

static float bInputs[OCP_WIN_CONFIG_INPUT_BUF_SIZE];
static float bOutputs[OCP_WIN_CONFIG_OUTPUT_BUG_SIZE];

uint8_t traceBuffer[OCP_WIN_CONFIG_TRACE_0_SIZE];

static float texec = 0.0f;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void ocpWinInitialize(void *intcInst){

    ocpWinInitializeTraces();
    ocpWinInitializeControlSystem();
    ocpWinInitializeInterface();
}
//-----------------------------------------------------------------------------
//=============================================================================


//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t ocpWinInitializeTraces(void){

    ocpTraceConfig_t config;

    config.mem = (void *)traceBuffer;
    config.size = OCP_WIN_CONFIG_TRACE_0_SIZE;
    config.data = (void **)trace0Data;
    config.names = trace0Names;

    ocpTraceInitialize(OCP_TRACE_1, &config, "Main Trace");

    ocpWinInitializeTracesMeas();

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpWinInitializeTracesMeas(void){

    cukConfigMeasurements_t *meas;
    cukConfigControl_t *outputs;

    /* Adds measurements to trace */
    meas = (cukConfigMeasurements_t *)bInputs;
    ocpTraceAddSignal(OCP_TRACE_1, &meas->i_i, "Input current");
    ocpTraceAddSignal(OCP_TRACE_1, &meas->i_1, "Primary inductor current");
    ocpTraceAddSignal(OCP_TRACE_1, &meas->v_in, "Input voltage");
    ocpTraceAddSignal(OCP_TRACE_1, &meas->v_dc, "DC link voltage");
    ocpTraceAddSignal(OCP_TRACE_1, &meas->v_1, "Primary coupling cap voltage");

    /* Adds control signals to trace */
    outputs = (cukConfigControl_t *)bOutputs;
    ocpTraceAddSignal(OCP_TRACE_1, &outputs->u, "Duty-cycle");
    ocpTraceAddSignal(OCP_TRACE_1, &outputs->sw_o, "Output switch");

    /* Other signals to add */
    ocpTraceAddSignal(OCP_TRACE_1, &texec, "Exec. time");

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpWinInitializeControlSystem(void){

    ocpCSConfig_t config;
    cukControllerConfig_t cukconfig;

    /* Initializes controller and hardware interface libs */
    cukconfig.disable = cukHwWinControllerDisable;
    cukconfig.enable = cukHwWinControllerEnable;
    cukControllerInitialize(&cukconfig);
//    cukHwIfInitialize();

    /* Initializes control sys lib */
    config.binputs = (void *)bInputs;
    config.boutputs = (void *)bOutputs;

    config.fhwInterface = 0;
    config.fhwStatus = cukHwWinStatus;

    config.fgetInputs = cukOpilGetMeasurements;
    //config.fgetInputs = cukHwGetMeasurements;

    config.fapplyOutputs = cukOpilUpdateControl;
    //config.fapplyOutputs = cukHwApplyOutputs;

    config.frun = cukControllerRun;
    config.fcontrollerInterface = cukControllerInterface;
    config.fcontrollerStatus = cukControllerStatus;

    //config.fenable = cukHwEnable;
    config.fenable = 0;
    //config.fdisable = cukHwDisable;
    config.fdisable = cukOpilDisable;

    config.fonEntry = 0;
    config.fonExit = 0;

    ocpCSInitialize(OCP_CS_1, &config, "Converter control");

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpWinInitializeInterface(void){

    /* Initializes OPiL interface */
    ocpOpilConfig_t config;

    config.updateMeas = cukOpilUpdateMeasurements;
    config.updateSimData = cukOpilUpdateSimData;

    config.initControl = 0;
    config.runControl = ocpWinAdcIrq;

    config.getControl = cukOpilGetControl;
    config.getControllerData = cukOpilGetControllerData;

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
void ocpWinAdcIrq(void *callbackRef){

    ocpCSRun(OCP_CS_1);
    ocpTraceSave(OCP_TRACE_1);
}
//-----------------------------------------------------------------------------
//=============================================================================

