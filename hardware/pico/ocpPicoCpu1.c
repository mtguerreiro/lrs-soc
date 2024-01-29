/*
 * @file ocpPicoCpu1.c
 *
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "ocpPicoCpu1.h"
#include <stdio.h>
#include "pico/stdlib.h"

/* Open controller project */
#include "ocp/ocp/ocpTrace.h"
#include "ocp/ocp/ocpCS.h"
#include "ocp/ocp/ocpIf.h"
#include "ocp/ocp/ocpConfig.h"

#include "ocp/hardware/pico/wz_pico_init.h"
#include "ocp/hardware/pico/ipcServerPico.h"

#include "ocp/ipc/ipcServer.h"
#include "hardware/adc.h"
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
static float chan_0 = 0.0f;
static float chan_1 = 0.0f;

const float conversion_factor = 3.3f / (1 << 12);
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void ocpPicoCpu1Initialize(void){
    // sleep_ms(5000);
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


    // printf("Core 1: Initializing ADC\n");
    adc_init();

    // GPIO 26 and 27 enabled
    adc_gpio_init(26);
    adc_gpio_init(27);
    // printf("Core 1: Set round robin masking\n");
    adc_set_round_robin(_u(0x03));
    // printf("Core 1: Setup ADC FIFO\n");
    adc_fifo_setup(true, false, 0, false, false);
    adc_fifo_drain();
    // printf("Core 1: ADC FIFO level: %d\n", adc_fifo_get_level());
    

   /*Using timer*/
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
	ocpTraceAddSignal(OCP_TRACE_1, (void *)&chan_0, "Channel 0");
    ocpTraceAddSignal(OCP_TRACE_1, (void *)&chan_1, "Channel 1");

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
    uint16_t temp_chan_0, temp_chan_1;
    adc_select_input(0);
    adc_run(true);
    temp_chan_0 = adc_fifo_get_blocking();
    temp_chan_1 = adc_fifo_get_blocking();
    adc_run(false);
    adc_fifo_drain(); 
    
   ticks = time_us_32();
   counter = ticks;
   chan_0 = (float) temp_chan_0 * conversion_factor;
   chan_1 = (float) temp_chan_1 * conversion_factor;

   ocpTraceSave(OCP_TRACE_1);

   ticks = time_us_32() - ticks;
   texec = ((float)ticks);

  return true;
}
//-----------------------------------------------------------------------------
//=============================================================================
