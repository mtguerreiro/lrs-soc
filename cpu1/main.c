/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

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

//#include "xparameters.h"
#include "xgpio.h"
#include "xil_types.h"

//#define INTC		    XScuGic
//#define INTC_HANDLER	XScuGic_InterruptHandler
//#define INTC_DEVICE_ID	XPAR_PS7_SCUGIC_0_DEVICE_ID
//#define PL_IRQ_ID       XPS_IRQ_INT_ID
//
//#define IRQ_PCORE_GEN_BASE  XPAR_IRQ_GEN_0_BASEADDR


#define COMM_VAL    (*(volatile unsigned long *)(0xFFFF0000))
#define BLINK_VAL  (*((volatile unsigned long *)(0xFFFF0000)+16))

///**
// * This typedef contains configuration information for the device driver.
// */
//typedef struct {
//	u16 DeviceId;		/**< Unique ID of device */
//	u32 BaseAddress;	/**< Base address of the device */
//} Pl_Config;


/**
 * The driver instance data. The user is required to allocate a
 * variable of this type.
 * A pointer to a variable of this type is then passed to the driver API
 * functions.
 */
typedef struct {
//	Pl_Config Config;   /**< Hardware Configuration */
	u32 IsReady;		/**< Device is initialized and ready */
	u32 IsStarted;		/**< Device is running */
} XPlIrq;


void        Xil_L1DCacheFlush(void);
//static int  SetupIntrSystem(INTC *IntcInstancePtr, XPlIrq *PeriphInstancePtr, u16 IntrId);
//static void DisableIntrSystem(INTC *IntcInstancePtr, u16 IntrId);
//static void PlIntrHandler(void *CallBackRef);

extern u32 MMUTable;

/**
 * Assign the driver structures for the interrupt controller and
 * PL Core interrupt source
 */
//INTC   IntcInstancePtr;
//XPlIrq PlIrqInstancePtr;

// Global for IRQ communication to main()
int irq_count;


#define LED_ID XPAR_AXI_GPIO_RGB_LED_DEVICE_ID
#define LED_CHANNEL 1
#define LED_MASK 0b111

#define INTC		    XScuGic
#define INTC_DEVICE_ID	XPAR_PS7_SCUGIC_0_DEVICE_ID
#define INTC_HANDLER	XScuGic_InterruptHandler

#define IPCOMM_INT_CPU0_TO_CPU1		0x0E
#define IPCOMM_INT_CPU1_TO_CPU0		0x0F

#define CPU0_ID 1

INTC   IntcInstancePtr;

static int mainSysInit(void);
static int SetupIntrSystem(INTC *IntcInstancePtr);

XGpio_Config *cfg_ptr = 0;
XGpio led_device;

void DeviceDriverHandler(void *CallbackRef);

int main()
{

	uint32_t prevstate;
	uint32_t data;
//    init_platform();

	uint32_t blink;

	BLINK_VAL = 1;

	mainSysInit();

    while(1){

    	prevstate = XGpio_DiscreteRead(&led_device, LED_CHANNEL);

    	prevstate &= ~0b00111000;
       	XGpio_DiscreteWrite(&led_device, LED_CHANNEL, ((data & LED_MASK) << 3U) | prevstate);

       	blink = (uint32_t)(BLINK_VAL) * 1000;
       	usleep(blink);		//Delay so output can be seen

       	data = data << 1;
    	if( (data & (LED_MASK << 2 ) ) == 0 ) data = 0xFF;
    }

//    print("Hello World\n\r");
//    print("Successfully ran Hello World application");
//    cleanup_platform();
    return 0;
}

static int mainSysInit(void){

	int Status;

    //Disable cache on OCM
	Xil_SetTlbAttributes(0xFFFF0000,0x14de2);           // S=b1 TEX=b100 AP=b11, Domain=b1111, C=b0, B=b0


    print("CPU1: init_platform\n\r");


	// Initialize the SCU Interrupt Distributer (ICD)
	Status = SetupIntrSystem(&IntcInstancePtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}


	/* Initializes PYNQ's (RGB) LEDs */
    cfg_ptr = XGpio_LookupConfig(LED_ID);
	XGpio_CfgInitialize(&led_device, cfg_ptr, cfg_ptr->BaseAddress);
	XGpio_SetDataDirection(&led_device, LED_CHANNEL, 0);

	COMM_VAL=0;

	return XST_SUCCESS;
}

static int SetupIntrSystem(INTC *IntcInstancePtr)
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


	XScuGic_Connect(IntcInstancePtr, IPCOMM_INT_CPU0_TO_CPU1, (Xil_ExceptionHandler)DeviceDriverHandler, IntcInstancePtr) ;
	XScuGic_Enable(IntcInstancePtr, IPCOMM_INT_CPU0_TO_CPU1);

	return XST_SUCCESS;
}

//=============================================================================
/*----------------------------------- IRQ -----------------------------------*/
//=============================================================================
void DeviceDriverHandler(void *CallbackRef){

	while(COMM_VAL == 0);
	xil_printf("CPU1: Got something from CPU0\n\r");

	XScuGic_SoftwareIntr ( &IntcInstancePtr , IPCOMM_INT_CPU1_TO_CPU0 , CPU0_ID ) ;

	COMM_VAL = 0;
}
//=============================================================================

/*****************************************************************************/
/**
*
* This function setups the interrupt system such that PL interrupt can occur
* for the peripheral. This function is application specific since the actual
* system may or may not have an interrupt controller. The peripheral device could be
* directly connected to a processor without an interrupt controller. The
* user should modify this function to fit the application.
*
* @param	IntcInstancePtr is a pointer to the instance of the Intc driver.
* @param	PeriphInstancePtr is a pointer to the instance of peripheral driver.
* @param	IntrId is the Interrupt Id of the peripheral interrupt
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note		None.
*
******************************************************************************/
//static int SetupIntrSystem(INTC *IntcInstancePtr,
//		XPlIrq *PeriphInstancePtr,
//				u16 IntrId)
//{
//	int Status;
//
//
//	XScuGic_Config *IntcConfig;
//
//	/*
//	 * Initialize the interrupt controller driver so that it is ready to
//	 * use.
//	 */
//	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
//	if (NULL == IntcConfig) {
//		return XST_FAILURE;
//	}
//
//	Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
//					IntcConfig->CpuBaseAddress);
//	if (Status != XST_SUCCESS) {
//		return XST_FAILURE;
//	}
//
//	/*
//	 * Connect the interrupt handler that will be called when an
//	 * interrupt occurs for the device.
//	 */
//	Status = XScuGic_Connect(IntcInstancePtr, IntrId,
//				 (Xil_ExceptionHandler)PlIntrHandler,
//				 PeriphInstancePtr);
//	if (Status != XST_SUCCESS) {
//		return Status;
//	}
//
//	/*
//	 * Enable the interrupt for the PL device.
//	 */
//	XScuGic_Enable(IntcInstancePtr, IntrId);
//
//
//	/*
//	 * Initialize the  exception table
//	 */
//	Xil_ExceptionInit();
//
//	/*
//	 * Register the interrupt controller handler with the exception table
//	 */
//	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
//			 (Xil_ExceptionHandler)INTC_HANDLER,
//			 IntcInstancePtr);
//
//	/*
//	 * Enable non-critical exceptions
//	 */
//	Xil_ExceptionEnable();
//
//
//	return XST_SUCCESS;
//}

/*****************************************************************************/
/**
*
* This function is the Interrupt handler for the PL Interrupt.
* It is called when the PL creates an interrupt and the interrupt gets serviced.
*
* This function sets the global varialbe irq_count=1 and clears the interrupt source.
*
* @param	CallBackRef is a pointer to the callback function.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
//static void PlIntrHandler(void *CallBackRef)
//{
//
//	XPlIrq *InstancePtr = (XPlIrq *)CallBackRef;
//
//	/*
//	 * Clear the interrupt source
//	 */
//	Xil_Out32(InstancePtr->Config.BaseAddress, 0);
//
//	irq_count = 1;
//
//}



/*****************************************************************************/
/**
*
* This function disables the interrupts that occur
*
* @param	IntcInstancePtr is the pointer to the instance of INTC driver.
* @param	IntrId is the Interrupt Id of the peripheral
*		value from xparameters.h.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
//static void DisableIntrSystem(INTC *IntcInstancePtr, u16 IntrId)
//{
//
//	/*
//	 * Disconnect and disable the interrupt
//	 */
//	/* Disconnect the interrupt */
//	XScuGic_Disable(IntcInstancePtr, IntrId);
//	XScuGic_Disconnect(IntcInstancePtr, IntrId);
//}
