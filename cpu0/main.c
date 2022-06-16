/*
 * main.c
 *
 *  Created on: 23.05.2022
 *      Author: Marco Guerreiro
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
/* Standard */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/* Tasks */
#include "blink.h"
#include "uiface.h"
#include "ipcomm.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"

/* Xilinx includes */
#include "xil_printf.h"
#include "xparameters.h"
#include "netif/xadapter.h"

#include "xil_mmu.h"
#include "xil_exception.h"
#include "xpseudo_asm.h"
#include "xscugic.h"

#include "soc_defs.h"
//=============================================================================

//=============================================================================
/*--------------------------------- Defines ---------------------------------*/
//=============================================================================
/* Command to wake CPU1 from CPU0. */
#define sev()			__asm__("sev")

/* Definitions for the interrupt controller */
#define MAIN_XIL_INTC_DEVICE_ID			XPAR_PS7_SCUGIC_0_DEVICE_ID
#define MAIN_XIL_INTC_HANDLER			XScuGic_InterruptHandler

/*
 * This flag is used to synchronize CPU0 and CPU1. After CPU0 writes the start
 * address for CPU1, it sets the sync flag and wakes CPU1. At this point, CPU1
 * starts and does its initialization procedure. After CPU1 is initialized, it
 * clears the sync flag. CPU0 only continues execution after the sync flag has
 * been cleared by CPU1.
 */
#define MAIN_SYNC_FLAG  		(*(volatile unsigned long *)(SOC_CPU0_CPU1_SYNC_FLAG_ADR))
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static int mainSysInit(void);
static int mainSetupIntrSystem(XScuGic *intcInstance);
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
/*
 * Assign the driver structures for the interrupt controller.
 *
 * Before handling control to the OS, the interrupt controller is initialized.
 * The initialized structure is then provided to tasks requiring the interrupt
 * controller, such that they do not need to perform any initialization.
 */
XScuGic   xINTCInstance;
//=============================================================================

//=============================================================================
/*---------------------------------- Main -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int main(void){

	mainSysInit();

	sys_thread_new("uiface_thrd", uiface,
					0,
					UIFACE_CONFIG_TASK_STACK_SIZE,
					UIFACE_CONFIG_TASK_PRIO);

	sys_thread_new("ipcomm_thrd", ipcomm,
					(void *)&xINTCInstance,
					IPCOMM_CONFIG_TASK_STACK_SIZE,
					IPCOMM_CONFIG_TASK_PRIO);


	sys_thread_new("blink_thrd", blink,
					0,
					BLINK_CONFIG_TASK_STACK_SIZE,
					BLINK_CONFIG_TASK_PRIO);

	vTaskStartScheduler();
	while(1);
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int mainSysInit(void){

	int Status;

	xil_printf("%s: CPU0 is initializing...\r\n", __FUNCTION__);

    /* Disables cache on OCM */
    Xil_SetTlbAttributes(0xFFFF0000,0x14de2);           // S=b1 TEX=b100 AP=b11, Domain=b1111, C=b0, B=b0

	/* Initializes the SCU Interrupt Distributer (ICD) */
	Status = mainSetupIntrSystem(&xINTCInstance);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Sets sync flag (to be cleared by CPU1) */
	MAIN_SYNC_FLAG = 1;

	/*
	 * Writes start address for CPU0, waits until it has been written (dmb)
	 * and wakes CPU1 up.
	 */
	xil_printf("%s: Waking up CPU1...\r\n", __FUNCTION__);
    Xil_Out32(SOC_CPU1_RESET_ADR, SOC_CPU1_START_ADR);
    dmb();
    sev();

    xil_printf("%s: Waiting for CPU1...\r\n", __FUNCTION__);
	while(MAIN_SYNC_FLAG == 1);

    xil_printf("%s: CPU1 has initialized.\r\n", __FUNCTION__);

    return XST_SUCCESS;
}
//-----------------------------------------------------------------------------
static int mainSetupIntrSystem(XScuGic *intcInstance)
{
	int Status;

	XScuGic_Config *IntcConfig;

	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	IntcConfig = XScuGic_LookupConfig(MAIN_XIL_INTC_DEVICE_ID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(intcInstance, IntcConfig,
					IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Initialize the  exception table
	 */
	Xil_ExceptionInit();

	/*
	 * Register the interrupt controller handler with the exception table
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			 (Xil_ExceptionHandler)MAIN_XIL_INTC_HANDLER,
			 intcInstance);

	/*
	 * Enable non-critical exceptions
	 */
	Xil_ExceptionEnable();

	return XST_SUCCESS;
}
//-----------------------------------------------------------------------------
//=============================================================================
