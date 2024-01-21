/*
 * @file ocpPicoCpu1.c
 *
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "ocpPicoCpu1.h"

/* Open controller project */
#include "ocp/ocp/ocpTrace.h"
#include "ocp/ocp/ocpCS.h"
#include "ocp/ocp/ocpIf.h"
#include "ocp/ocp/ocpConfig.h"

#include "ocp/hardware/pico/wz_pico_init.h"
#include "ocp/hardware/pico/ipcServerPico.h"

#include "ocp/ipc/ipcServer.h"
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t ocpPicoCpu1InitializeHw(void);
//-----------------------------------------------------------------------------
static int32_t ocpPicoCpu1InitializeIpc(void);
//-----------------------------------------------------------------------------
static int32_t ocpPicoCpu1InitializeTraces(void);
//-----------------------------------------------------------------------------
static int32_t ocpPicoCpu1InitializeControlSystem(void);
//-----------------------------------------------------------------------------
static int32_t ocpPicoCpu1InitializeInterface(void);
//-----------------------------------------------------------------------------
static bool ocpPicoAdcIrq(struct repeating_timer *t);
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define OCP_PICO_C1_CONFIG_TRACE_0_NAME_LEN     50
#define OCP_PICO_C1_CONFIG_TRACE_0_MAX_SIGNALS  10

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static char trace0Names[OCP_PICO_C1_CONFIG_TRACE_0_NAME_LEN];
static size_t trace0Data[OCP_PICO_C1_CONFIG_TRACE_0_MAX_SIGNALS];

static struct repeating_timer timerAdc;

static float texec = 0.0f;
static float counter = 0.0f;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void ocpPicoCpu1Initialize(void){

	ocpPicoCpu1InitializeHw();
	ocpPicoCpu1InitializeIpc();
	ocpPicoCpu1InitializeTraces();
	ocpPicoCpu1InitializeControlSystem();
	ocpPicoCpu1InitializeInterface();
}
//-----------------------------------------------------------------------------
//=============================================================================


//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t ocpPicoCpu1InitializeHw(void){
	
    add_repeating_timer_ms(1, ocpPicoAdcIrq, NULL, &timerAdc);

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpPicoCpu1InitializeIpc(void){

    ipcServerPicoInitialize();

    ipcServerInitialize(ocpIf,
        ipcServerPicoIrqSend,
        OCP_PICO_CONFIG_MEM_CPU0_TO_CPU1_ADR, OCP_PICO_CONFIG_MEM_CPU0_TO_CPU1_SIZE,
        OCP_PICO_CONFIG_MEM_CPU1_TO_CPU0_ADR, OCP_PICO_CONFIG_MEM_CPU1_TO_CPU0_SIZE);

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpPicoCpu1InitializeTraces(void){

	ocpTraceConfig_t config;

	config.mem = (void *)OCP_PICO_CONFIG_MEM_TRACE_ADR;
	config.size = OCP_PICO_CONFIG_MEM_TRACE_SIZE_MAX;
	config.data = (void **)trace0Data;
	config.names = trace0Names;

	ocpTraceInitialize(OCP_TRACE_1, &config, "Pico Trace 1");

    ocpTraceAddSignal(OCP_TRACE_1, (void *)&texec, "Exec. time");
    ocpTraceAddSignal(OCP_TRACE_1, (void *)&counter, "Counter");

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpPicoCpu1InitializeControlSystem(void){

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpPicoCpu1InitializeInterface(void){

	ocpIfInitialize();

	return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------------- IRQ -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static bool ocpPicoAdcIrq(struct repeating_timer *t){

    uint32_t ticks;

    ticks = time_us_32();
    counter = ticks;

    //ocpCSRun(OCP_CS_1);
    ocpTraceSave(OCP_TRACE_1);

    ticks = time_us_32() - ticks;
    texec = ((float)ticks);

    return true;
}
//-----------------------------------------------------------------------------
//=============================================================================
