/*
 * main.c
 *
 *  Created on: 23.05.2022
 *      Author: Marco Guerreiro
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include <stdio.h>
//#include "platform.h"
#include "xil_printf.h"

#include "xparameters.h"
#include <stdio.h>
#include "xil_io.h"
#include "xil_mmu.h"
#include "xil_cache.h"
#include "xil_exception.h"
#include "xscugic.h"
#include "sleep.h"

#include "xgpio.h"
#include "xil_types.h"

#include "zynqAxiPwm.h"
#include "zynqAxiAdc.h"

//#include "afe.h"
//#include "afeIf.h"

#include "ocpZynqCpu1.h"

#include "zynqConfig.h"
//=============================================================================

//=============================================================================
/*--------------------------------- Defines ---------------------------------*/
//=============================================================================
#define SYNC_FLAG  		(*(volatile unsigned long *)(ZYNQ_CONFIG_CPU0_CPU1_SYNC_FLAG_ADR))

#define MAIN_LED_ID         XPAR_AXI_GPIO_RGB_LED_DEVICE_ID
#define MAIN_LED_CHANNEL    1
#define MAIN_LED_MASK       0b111
#define MAIN_LED_OFS        3

#define MAIN_LED_BLUE      (1 << 0)
#define MAIN_LED_GREEN     (1 << 1)
#define MAIN_LED_RED       (1 << 2)

#define INTC		    XScuGic
#define INTC_DEVICE_ID	XPAR_PS7_SCUGIC_0_DEVICE_ID
#define INTC_HANDLER	XScuGic_InterruptHandler

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================

INTC   IntcInstancePtr;

XGpio led;


uint32_t blinkPeriod = 1000;

//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static int mainSysInit(void);
static int mainSetupIntrSystem(INTC *IntcInstancePtr);

static void mainRgbLedToggleColor(uint32_t color);
//=============================================================================

//=============================================================================
/*---------------------------------- Main -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int main(){

	mainSysInit();

    while(1){

        mainRgbLedToggleColor(MAIN_LED_GREEN);
       	usleep(blinkPeriod * 1000);
    }

    return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int mainSysInit(void){

	int Status;
	XGpio_Config *cfg_ptr = 0;

    //Disable cache on OCM
	Xil_SetTlbAttributes(0xFFFF0000,0x14de2);           // S=b1 TEX=b100 AP=b11, Domain=b1111, C=b0, B=b0

    print("CPU1: init_platform\n\r");

	// Initialize the SCU Interrupt Distributer (ICD)
	Status = mainSetupIntrSystem(&IntcInstancePtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/* Initializes hardware and OCP */
	ocpZynqCpu1Initialize(&IntcInstancePtr);

	/* Initializes PYNQ's (RGB) LEDs */
    cfg_ptr = XGpio_LookupConfig(MAIN_LED_ID);
	XGpio_CfgInitialize(&led, cfg_ptr, cfg_ptr->BaseAddress);
	XGpio_SetDataDirection(&led, MAIN_LED_CHANNEL, 0);

	SYNC_FLAG = 0;

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
static void mainRgbLedToggleColor(uint32_t color){

    uint32_t prev, new;

    prev = XGpio_DiscreteRead(&led, MAIN_LED_CHANNEL);

    new = ((color & MAIN_LED_MASK) << MAIN_LED_OFS) ^ prev;

    XGpio_DiscreteWrite(&led, MAIN_LED_CHANNEL, new);
}
//-----------------------------------------------------------------------------
//=============================================================================
