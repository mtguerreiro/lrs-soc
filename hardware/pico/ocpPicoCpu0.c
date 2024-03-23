/*
 * @file ocpPicoCpu0.c
 *
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "ocpPicoCpu0.h"
#include <stdio.h>
/* Open controller project */
#include "ocp/ocp/ocpTrace.h"
#include "ocp/ocp/ocpCS.h"
#include "ocp/ocp/ocpIfMaster.h"

#include "ocp/hardware/pico/wz_pico_init.h"
#include "ocp/hardware/pico/ipcClientPico.h"

#include "ocp/ipc/ipcClient.h"


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
	sleep_ms(5000);
	printf("Baud rate: %d\n", uart_init(OCP_PICO_CONFIG_UART_RS232_ID, OCP_PICO_CONFIG_UART_RS232_BAUD_RATE));
	// uart_set_format(OCP_PICO_CONFIG_UART_RS232_ID, 8, 2, UART_PARITY_EVEN);
    gpio_set_function(OCP_PICO_CONFIG_UART_RS232_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(OCP_PICO_CONFIG_UART_RS232_RX_PIN, GPIO_FUNC_UART);
	return wzPicoInit();
}
//-----------------------------------------------------------------------------
static int32_t ocpPicoCpu0InitializeIpc(void){

	ipcClientPicoInitialize();

	ipcClientInitialize(ipcClientPicoIrqSend, ipcClientPicoIrqReceive,
		ipcClientPicoLock, ipcClientPicoUnlock,
		OCP_PICO_CONFIG_MEM_CPU1_TO_CPU0_ADR, OCP_PICO_CONFIG_MEM_CPU1_TO_CPU0_SIZE,
		OCP_PICO_CONFIG_MEM_CPU0_TO_CPU1_ADR, OCP_PICO_CONFIG_MEM_CPU0_TO_CPU1_SIZE);

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

	ocpIfMasterInitialize();

	return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================