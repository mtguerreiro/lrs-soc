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

#include "soc_defs.h"

#include "axi_test.h"

#include "math.h"

#include "soc_trace.h"

#include "rp.h"

#include "ipcServer.h"
#include "ipcServerZynq.h"

#include "afe.h"
#include "afeIf.h"

#include "ocpIf.h"

#include "ocpZynqCpu1.h"

#include "zynqConfig.h"
//=============================================================================

//=============================================================================
/*--------------------------------- Defines ---------------------------------*/
//=============================================================================
#define SYNC_FLAG  		(*(volatile unsigned long *)(ZYNQ_CONFIG_CPU0_CPU1_SYNC_FLAG_ADR))

#define LED_ID XPAR_AXI_GPIO_RGB_LED_DEVICE_ID
#define LED_CHANNEL 1
#define LED_MASK 0b111

#define INTC		    XScuGic
#define INTC_DEVICE_ID	XPAR_PS7_SCUGIC_0_DEVICE_ID
#define INTC_HANDLER	XScuGic_InterruptHandler

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================

INTC   IntcInstancePtr;

XGpio_Config *cfg_ptr = 0;
XGpio led_device;
//XGpio relay_device;
//XGpio gpioDebug_device;

uint32_t blinkPeriod = 1000;

//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static int mainSysInit(void);
static int mainSetupIntrSystem(INTC *IntcInstancePtr);

//void DeviceDriverHandler(void *CallbackRef);

#define AXI_TEST_BASE_ADR			XPAR_ADC_PSCTL_0_S00_AXI_BASEADDR
#define AXI_PWM_BASE_ADR			XPAR_AXI_PWM_0_S00_AXI_BASEADDR
#define AXI_GPIO_DEBUG_BASE_ADR		XPAR_AXI_GPIO_DEBUG_BASEADDR
//=============================================================================

//=============================================================================
/*---------------------------------- Main -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int main(){

	uint32_t prevstate;
	uint32_t data = 0;

	mainSysInit();

    while(1){

    	prevstate = XGpio_DiscreteRead(&led_device, LED_CHANNEL);

    	prevstate &= ~0b00111000;
       	XGpio_DiscreteWrite(&led_device, LED_CHANNEL, ((data & LED_MASK) << 3U) | prevstate);

       	usleep(blinkPeriod * 1000);		//Delay so output can be seen

       	data = data << 1;
    	if( (data & (LED_MASK << 2 ) ) == 0 ) data = 0xFF;
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

	/* Initializes the application */
	afeInitialize( 0 );

	/* Initializes PYNQ's (RGB) LEDs */
    cfg_ptr = XGpio_LookupConfig(LED_ID);
	XGpio_CfgInitialize(&led_device, cfg_ptr, cfg_ptr->BaseAddress);
	XGpio_SetDataDirection(&led_device, LED_CHANNEL, 0);

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
//=============================================================================

////=============================================================================
///*----------------------------------- IRQ -----------------------------------*/
////=============================================================================
////-----------------------------------------------------------------------------
//void DeviceDriverHandler(void *CallbackRef){
//
//	ipcServerRequest();
//}
////-----------------------------------------------------------------------------
////=============================================================================
