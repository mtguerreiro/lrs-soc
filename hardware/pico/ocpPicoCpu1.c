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
#include "hardware/uart.h"

#include "ocp/app/itm3903c/itm3903cConfig.h"
#include "ocp/app/itm3903c/itm3903cController.h"
#include "ocp/app/itm3903c/itm3903cHw.h"
#include "ocp/app/itm3903c/itm3903cHwIf.h"
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

#define OCP_PICO_C1_CONFIG_INPUT_BUF_SIZE       20
#define OCP_PICO_C1_CONFIG_OUTPUT_BUF_SIZE      20

#define UART_ID uart1
#define BAUD_RATE 115200
#define UART_TX_PIN 4
#define UART_RX_PIN 5
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

static float bInputs[OCP_PICO_C1_CONFIG_INPUT_BUF_SIZE];
static float bOutputs[OCP_PICO_C1_CONFIG_OUTPUT_BUF_SIZE];
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

    uart_init(UART_ID, BAUD_RATE);

    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // uart_puts(UART_ID, " Hello, UART!\n");

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
    itm3903cConfigMeasurements_t *meas;

    meas = (itm3903cConfigMeasurements_t *)bInputs;

	config.mem = (void *)OCP_PICO_CONFIG_MEM_TRACE_ADR;
	config.size = OCP_PICO_CONFIG_MEM_TRACE_SIZE_MAX;
	config.data = (void **)trace0Data;
	config.names = trace0Names;

	ocpTraceInitialize(OCP_TRACE_1, &config, "Pico Trace 1");

    ocpTraceAddSignal(OCP_TRACE_1, (void *)&texec, "Exec. time");
    ocpTraceAddSignal(OCP_TRACE_1, (void *)&counter, "Counter");

	ocpTraceAddSignal(OCP_TRACE_1, (void *)&meas->v, "Channel 0");
	ocpTraceAddSignal(OCP_TRACE_1, (void *)&meas->i, "Channel 1");

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpPicoCpu1InitializeControlSystem(void){

	ocpCSConfig_t config;
	itm3903cControllerConfig_t itconfig;

    /* Initializes controller and hardware interface libs */
	itconfig.disable = 0;
    itconfig.enable = 0;
	itm3903cControllerInitialize(&itconfig);
    itm3903cHwIfInitialize();

    /* Initializes control sys lib */
    config.binputs = (void *)bInputs;
    config.boutputs = (void *)bOutputs;

    config.fhwInterface = itm3903cHwIf;
    config.fhwStatus = itm3903cHwStatus;

    //config.fgetInputs = cukOpilGetMeasurements;
    config.fgetInputs = itm3903cHwGetMeasurements;

    //config.fapplyOutputs = cukOpilUpdateControl;
    config.fapplyOutputs = itm3903cHwApplyOutputs;

    config.frun = itm3903cControllerRun;
    config.fcontrollerInterface = itm3903cControllerInterface;
    config.fcontrollerStatus = itm3903cControllerStatus;

    config.fenable = 0;
    config.fdisable = 0;

    config.fonEntry = 0;
    config.fonExit = 0;

	ocpCSInitialize(OCP_CS_1, &config, "Supply control");

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

    ocpCSRun(OCP_CS_1);
    ocpTraceSave(OCP_TRACE_1);

    ticks = time_us_32() - ticks;
    texec = ((float)ticks);

    return true;
}
//-----------------------------------------------------------------------------
//=============================================================================
