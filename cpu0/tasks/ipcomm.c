/*
 * ipcomm.c
 *
 *  Created on: 23.05.2022
 *      Author: Marco Guerreiro
 *
 * CPU
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "ipcomm.h"

/* Kernel */
#include "FreeRTOS.h"
#include "task.h"

/* Device and drivers */
#include "xparameters.h"
#include "xil_types.h"

#include "xil_exception.h"
#include "xscugic_hw.h"
#include "xil_printf.h"
#include "xstatus.h"
#include "xscugic.h"

/* Tasks */
#include "uiface.h"

/* LRS SoC defs */
#include "soc_defs.h"
//=============================================================================

//=============================================================================
/*--------------------------------- Defines ---------------------------------*/
//=============================================================================
/*
 * Software generated interrupt (SIG) from CPU0 to CPU1. This interrupt is
 * generated by CPU0 to notify CPU1 that a new command is to be executed.
 */
#define IPCOMM_INT_CPU0_TO_CPU1		SOC_SIG_CPU0_TO_CPU1

/*
 * Software generated interrupt (SIG) from CPU1 to CPU0. This interrupt is
 * generated by CPU1 to notify CPU0 that a requested command was executed.
 */
#define IPCOMM_INT_CPU1_TO_CPU0		SOC_SIG_CPU1_TO_CPU0


/* Functions binding to CPU1 commands should have this signature */
typedef uint32_t(*ipcommCMDHandle_t)(uifaceDataExchange_t *data);

typedef struct{
	/*
	 * CPU1 flag.
	 *
	 * This flag is set by CPU0 before a command is issued to CPU1. After
	 * CPU1 executes its commands, it generates an interrupt in CPU1. The
	 * handler of this interrupt clears this flag. Thus, this flag is used
	 * to indicate that CPU1 executed a command issued by CPU0.
	 */
	volatile uint32_t cpu1flag; /* Should this be volatile? */

	/*
	 * CPU1 commands.
	 *
	 * The index corresponds to CPU1 commands, while the entries correspond
	 * to CPU0 commands.
	 */
	uint32_t cmd[SOC_CMD_CPU1_END];

	/* Interrupt controller instance */
	XScuGic *intcInstance;

}ipcommControl_t;

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
ipcommControl_t xipcommControl;
//=============================================================================


//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static int ipcommInitialize(void);

static void ipcommCMDRegister(uint32_t cpu0cmd, uint32_t cpu1cmd, ipcommCMDHandle_t handle);
static uint32_t ipcommCMDFind(uint32_t cpu0cmd);
static uint32_t ipcommCMDExecute(uifaceDataExchange_t *data);

void ipcommIRQCPU1(void *CallbackRef);
//=============================================================================

//=============================================================================
/*---------------------------------- Task -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void ipcomm(void *intcInstance){

	xipcommControl.intcInstance = (XScuGic *)intcInstance;

	ipcommInitialize();

	vTaskDelete(NULL);

	while(1);
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int ipcommInitialize(void){

	XScuGic_Connect(xipcommControl.intcInstance, IPCOMM_INT_CPU1_TO_CPU0,
					(Xil_ExceptionHandler)ipcommIRQCPU1,
					(void  *)xipcommControl.intcInstance);
	XScuGic_Enable(xipcommControl.intcInstance, IPCOMM_INT_CPU1_TO_CPU0);

	ipcommCMDRegister(SOC_CMD_CPU0_BLINK_CPU1, SOC_CMD_CPU1_BLINK, ipcommCMDExecute);

	ipcommCMDRegister(SOC_CMD_CPU0_ADC_EN, SOC_CMD_CPU1_ADC_EN, ipcommCMDExecute);

	ipcommCMDRegister(SOC_CMD_CPU0_ADC_SPI_FREQ_SET, SOC_CMD_CPU1_ADC_SPI_FREQ_SET, ipcommCMDExecute);

	ipcommCMDRegister(SOC_CMD_CPU0_ADC_SAMPLING_FREQ_SET, SOC_CMD_CPU1_ADC_SAMPLING_FREQ_SET, ipcommCMDExecute);

	ipcommCMDRegister(SOC_CMD_CPU0_ADC_ERROR_READ, SOC_CMD_CPU1_ADC_ERROR_READ, ipcommCMDExecute);

	ipcommCMDRegister(SOC_CMD_CPU0_ADC_ERROR_CLEAR, SOC_CMD_CPU1_ADC_ERROR_CLEAR, ipcommCMDExecute);

	ipcommCMDRegister(SOC_CMD_CPU0_TRACE_START, SOC_CMD_CPU1_TRACE_START, ipcommCMDExecute);

	ipcommCMDRegister(SOC_CMD_CPU0_TRACE_READ, SOC_CMD_CPU1_TRACE_READ, ipcommCMDExecute);

	ipcommCMDRegister(SOC_CMD_CPU0_TRACE_SIZE_SET, SOC_CMD_CPU1_TRACE_SIZE_SET, ipcommCMDExecute);

	ipcommCMDRegister(SOC_CMD_CPU0_TRACE_SIZE_READ, SOC_CMD_CPU1_TRACE_SIZE_READ, ipcommCMDExecute);

	ipcommCMDRegister(SOC_CMD_CPU0_CONTROL_EN, SOC_CMD_CPU1_CONTROL_EN, ipcommCMDExecute);

	return XST_SUCCESS;
}
//-----------------------------------------------------------------------------
static void ipcommCMDRegister(uint32_t cpu0cmd, uint32_t cpu1cmd, ipcommCMDHandle_t handle){

	xipcommControl.cmd[cpu1cmd] = cpu0cmd;
	uifaceRegisterHandle(cpu0cmd, handle);
}
//-----------------------------------------------------------------------------
static uint32_t ipcommCMDFind(uint32_t cpu0cmd){

	uint32_t i;

	for(i = 0; i < SOC_CMD_CPU1_END; i++){
		if( xipcommControl.cmd[i] == cpu0cmd ) break;
	}

	return i;
}
//-----------------------------------------------------------------------------
static uint32_t ipcommCMDExecute(uifaceDataExchange_t *data){

	uint32_t cpu0cmd, cpu1cmd;
	uint32_t *p;
	uint8_t *src, *dst;
	uint32_t i;

	/*
	 * Don't do anything if the size of the received data exceeds the
	 * CPU0->CPU1 buffer size.
	 */
	if( data->size > SOC_MEM_CPU0_TO_CPU1_SIZE ) return 0;

	/*
	 * Sets CPU1 flag. This flag is only cleared in the CPU1->CPU0 interrupt
	 * handler.
	 */
	xipcommControl.cpu1flag = 1;

	cpu0cmd = data->cmd;
	cpu1cmd = ipcommCMDFind(cpu0cmd);

	/*
	 * First, the command to be executed by CPU1 is written in the first address
	 * of the CPU0->CPU1 buffer. After that, the data (if any) is copied to
	 * the subsequent addresses of the buffer.
	 */
	p = (uint32_t *)SOC_MEM_CPU0_TO_CPU1_ADR;
	*p = cpu1cmd;

	if( data->size != 0 ){
		dst = (uint8_t *)(SOC_MEM_CPU0_TO_CPU1_ADR + 4);
		src = data->buffer;
		for(i = 0; i < data->size; i++) *dst++ = *src++;
	}

	/* Generates a software interrupt on CPU1 */
	XScuGic_SoftwareIntr ( xipcommControl.intcInstance, IPCOMM_INT_CPU0_TO_CPU1, SOC_SIG_CPU1_ID );

	/* Waits until CPU1 flag is cleared */
	while( xipcommControl.cpu1flag == 1 );

	/* Checks if there is any data that should be sent back */
	p = (uint32_t *)SOC_MEM_CPU1_TO_CPU0_ADR;
	if( *p == 0 ) return 0;

	/*
	 * Here, we expect that the address given by CPU1 is accessible by CPU0.
	 * This address could be the CPU1->CPU0 shared memory or RAM (for the
	 * trace buffer, for instance).
	 */
	data->buffer = (uint8_t *)p[0];
	data->size = p[1];

	return 1;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------------- IRQ -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void ipcommIRQCPU1(void *CallbackRef){

	xipcommControl.cpu1flag = 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
