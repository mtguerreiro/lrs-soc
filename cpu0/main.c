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
#define INTC		    XScuGic
#define INTC_DEVICE_ID	XPAR_PS7_SCUGIC_0_DEVICE_ID
#define INTC_HANDLER	XScuGic_InterruptHandler

#define sev()			__asm__("sev")
#define SYNC_FLAG  		(*(volatile unsigned long *)(SOC_CPU0_CPU1_SYNC_FLAG_ADR))
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static int mainSysInit(void);
static int mainSetupIntrSystem(INTC *IntcInstancePtr);
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
/*
 * Assign the driver structures for the interrupt controller
 */
INTC   IntcInstancePtr;
//=============================================================================

//=============================================================================
/*---------------------------------- Main -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int main(void){

	mainSysInit();

	sys_thread_new("uiface_thrd", (void(*)(void*))uiface, 0,
					UIFACE_CONFIG_TASK_STACK_SIZE,
					UIFACE_CONFIG_TASK_PRIO);

	sys_thread_new("ipcomm_thrd", (void(*)(void*))ipcomm, (void *)&IntcInstancePtr,
					IPCOMM_CONFIG_TASK_STACK_SIZE,
					IPCOMM_CONFIG_TASK_PRIO);


	sys_thread_new("blink_thrd", (void(*)(void*))blink, 0,
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

    //Disable cache on OCM
    Xil_SetTlbAttributes(0xFFFF0000,0x14de2);           // S=b1 TEX=b100 AP=b11, Domain=b1111, C=b0, B=b0


    SYNC_FLAG = 0;

	// Initialize the SCU Interrupt Distributer (ICD)
	Status = mainSetupIntrSystem(&IntcInstancePtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

    print("CPU0: writing startaddress for cpu1\n\r");
    Xil_Out32(SOC_CPU1_RESET_ADR, SOC_CPU1_START_ADR);
    dmb(); //waits until write has finished

	print("CPU0: Hello World CPU 0\n\r");
	SYNC_FLAG = 1;

    print("CPU0: sending the SEV to wake up CPU1\n\r");
    sev();

	while(SYNC_FLAG == 1);

	SYNC_FLAG = 1;

    return XST_SUCCESS;

}
//-----------------------------------------------------------------------------
static int mainSetupIntrSystem(INTC *IntcInstancePtr)
{
	int Status;


	XScuGic_Config *IntcConfig;

	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
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
			 (Xil_ExceptionHandler)INTC_HANDLER,
			 IntcInstancePtr);

	/*
	 * Enable non-critical exceptions
	 */
	Xil_ExceptionEnable();


	return XST_SUCCESS;
}
//-----------------------------------------------------------------------------
//=============================================================================
