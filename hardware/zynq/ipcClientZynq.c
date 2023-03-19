/*
 * @file ipcClientZynq.c
 *
 * @brief Client for the inter-processor communication (IPC) library.
 */

#ifdef SOC_CPU0
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "ipcClientZynq.h"

/* Kernel */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Device and drivers */
#include "xparameters.h"
#include "xil_types.h"

#include "xil_exception.h"
#include "xscugic_hw.h"
#include "xil_printf.h"
#include "xstatus.h"
#include "xscugic.h"

/* LRS SoC defs */
#include "soc_defs.h"
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void ipcClientZynqIrq(void *callbackRef);
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
/*
 * Software generated interrupt (SIG) from CPU0 to CPU1. This interrupt is
 * generated by CPU0 to notify CPU1 that a new command is to be executed.
 */
#define IPC_CLIENT_ZYNQ_INT_CPU0_TO_CPU1			SOC_SIG_CPU0_TO_CPU1

/*
 * Software generated interrupt (SIG) from CPU1 to CPU0. This interrupt is
 * generated by CPU1 to notify CPU0 that a requested command was executed.
 */
#define IPC_CLIENT_ZYNQ_INT_CPU1_TO_CPU0			SOC_SIG_CPU1_TO_CPU0

/* CPU1->CPU0 reply timeout, in system ticks */
#define IPC_CLIENT_ZYNQ_CONFIG_CPU1_REPLY_TO		(IPC_CLIENT_ZYNQ_CONFIG_CPU1_REPLY_TO_MS / portTICK_PERIOD_MS)

/**/
#define IPC_CLIENT_ZYNQ_CONFIG_SIG_CPU1_ID			SOC_SIG_CPU1_ID


typedef struct{

	/* Interrupt controller instance */
	XScuGic *intcInstance;

	/*
	 * Semaphore used to signal a response from CPU1.
	 *
	 * This semaphore is taken before a command is issued to CPU1. When CPU1
	 * replies back, an interrupt is generated in CPU0, which releases the
	 * semaphore. Thus, the semaphore is used to indicate that CPU1 replied
	 * to a command issued by CPU0.
	 */
	SemaphoreHandle_t cpu1Semaphore;

}ipcClientZynqControl_t;
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
ipcClientZynqControl_t xipcClientZynqControl;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void ipcClientZynqInitialize(void *irqInst){

	xipcClientZynqControl.intcInstance = (XScuGic *)irqInst;

	XScuGic_Connect(xipcClientZynqControl.intcInstance, IPC_CLIENT_ZYNQ_INT_CPU1_TO_CPU0,
					(Xil_ExceptionHandler)ipcClientZynqIrq,
					(void  *)xipcClientZynqControl.intcInstance);
	XScuGic_Enable(xipcClientZynqControl.intcInstance, IPC_CLIENT_ZYNQ_INT_CPU1_TO_CPU0);

	xipcClientZynqControl.cpu1Semaphore = xSemaphoreCreateBinary();
}
//-----------------------------------------------------------------------------
int32_t ipcClientZynqIrqSend(void){

	/* Makes sure semaphore is taken */
	xSemaphoreTake(xipcClientZynqControl.cpu1Semaphore, 0);

	/* Generates a software interrupt on CPU1 */
	XScuGic_SoftwareIntr ( xipcClientZynqControl.intcInstance,
			IPC_CLIENT_ZYNQ_INT_CPU0_TO_CPU1,  IPC_CLIENT_ZYNQ_CONFIG_SIG_CPU1_ID);

	return 0;
}
//-----------------------------------------------------------------------------
int32_t ipcClientZynqIrqReceive(uint32_t timeout){

	/* Waits until CPU1 replies back */
	if( xSemaphoreTake(xipcClientZynqControl.cpu1Semaphore, IPC_CLIENT_ZYNQ_CONFIG_CPU1_REPLY_TO) != pdTRUE ){
		return IPC_CLIENT_ZYNQ_ERR_CPU1_REPLY_TO;
	}

	return 0;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------------- IRQ -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void ipcClientZynqIrq(void *callbackRef){

	BaseType_t xHigherPriorityTaskWoken;

	xSemaphoreGiveFromISR( xipcClientZynqControl.cpu1Semaphore, &xHigherPriorityTaskWoken );

	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
//-----------------------------------------------------------------------------
//=============================================================================
#endif /* SOC_CPU0 */
