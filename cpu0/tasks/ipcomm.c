//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "ipcomm.h"

/* Kernel */
#include "FreeRTOS.h"
#include "task.h"

/* Device and drivers */
#include "xparameters.h"
//#include "netif/xadapter.h"
//#include "platform_config.h"
#include "xil_printf.h"
#include "xil_types.h"

#include "xil_exception.h"
#include "xscugic_hw.h"
#include "xil_printf.h"
#include "xstatus.h"
#include "xscugic.h"
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define INTC_DEVICE_ID		XPAR_SCUGIC_0_DEVICE_ID
#define INTC_DEVICE_INT_ID	0x0E

#define IPCOMM_INT_CPU0_TO_CPU1		0x0E
#define IPCOMM_INT_CPU1_TO_CPU0		0x0F

XScuGic *InterruptControllerInst; 	     /* Instance of the Interrupt Controller */
//static XScuGic_Config *GicConfig;    /* The configuration parameters of the controller */

#define CPU1_ID 2

#define COMM_VAL  (*(volatile unsigned long *)(0xFFFF0000))
//=============================================================================


//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static int ipcommSysInit(void);

void DeviceDriverHandler(void *CallbackRef);

//static int SetUpInterruptSystem(XScuGic *XScuGicInstancePtr);

//=============================================================================

//=============================================================================
/*---------------------------------- Task -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void ipcomm(void *param){

	InterruptControllerInst = param;

	ipcommSysInit();

	while(1){
		vTaskDelay(10000 / portTICK_PERIOD_MS);

		XScuGic_SoftwareIntr ( InterruptControllerInst , IPCOMM_INT_CPU0_TO_CPU1 , CPU1_ID ) ;
	}
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//int32_t ipcommRegisterHandle(uint32_t id, commHandle_t handle){
//
//	/* Cannot register ID as 0 */
//	if( id == 0 ){
//		return COMM_ERR_INVALID_ID;
//	}
//	/* Checks if ID is available */
//	if( commFindID(id) != commControl.n ){
//		return COMM_ERR_INVALID_ID;
//	}
//
//	if( commControl.n >= COMM_CONFIG_IDS ) return COMM_ERR_EXCEEDED_MAX_ID;
//
//	//SERIAL_CRITICAL_ENTER;
//
//	commControl.id[commControl.n] = id;
//	commControl.handle[commControl.n] = handle;
//
//	commControl.n++;
//
//	//SERIAL_CRITICAL_EXIT;
//
//	return 0;
//}
//-----------------------------------------------------------------------------
//=============================================================================


//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int ipcommSysInit(void){

	int Status;

	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
//	GicConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
//	if (NULL == GicConfig) {
//		return XST_FAILURE;
//	}
//
//	Status = XScuGic_CfgInitialize(&InterruptController, GicConfig,
//					GicConfig->CpuBaseAddress);
//	if (Status != XST_SUCCESS) {
//		return XST_FAILURE;
//	}

//
//	/*
//	 * Perform a self-test to ensure that the hardware was built
//	 * correctly
//	 */
//	Status = XScuGic_SelfTest(&InterruptController);
//	if (Status != XST_SUCCESS) {
//		return XST_FAILURE;
//	}
//
//
//	/*
//	 * Setup the Interrupt System
//	 */
//	Status = SetUpInterruptSystem(&InterruptController);
//	if (Status != XST_SUCCESS) {
//		return XST_FAILURE;
//	}
//
//	/*
//	 * Connect a device driver handler that will be called when an
//	 * interrupt for the device occurs, the device driver handler performs
//	 * the specific interrupt processing for the device
//	 */
//	Status = XScuGic_Connect(&InterruptController, INTC_DEVICE_INT_ID,
//			   (Xil_ExceptionHandler)DeviceDriverHandler,
//			   (void *)&InterruptController);
//
//	if (Status != XST_SUCCESS) {
//		return XST_FAILURE;
//	}
//
//	/*
//	 * Enable the interrupt for the device and then cause (simulate) an
//	 * interrupt so the handlers will be called
//	 */
//	XScuGic_Enable(&InterruptController, INTC_DEVICE_INT_ID);

	/*
	 *  Simulate the Interrupt
	 */
//	Status = XScuGic_SoftwareIntr(&InterruptController,
//			IPCOMM_INT_CPU0_TO_CPU1,
//					CPU1_ID);
//	if (Status != XST_SUCCESS) {
//		return XST_FAILURE;
//	}

//	/*
//	 * Wait for the interrupt to be processed, if the interrupt does not
//	 * occur this loop will wait forever
//	 */
//	while (1) {
//		/*
//		 * If the interrupt occurred which is indicated by the global
//		 * variable which is set in the device driver handler, then
//		 * stop waiting
//		 */
//		if (InterruptProcessed) {
//			break;
//		}
//	}

	XScuGic_Connect(InterruptControllerInst, IPCOMM_INT_CPU1_TO_CPU0, (Xil_ExceptionHandler)DeviceDriverHandler, (void  *)InterruptControllerInst) ;
	XScuGic_Enable(InterruptControllerInst, IPCOMM_INT_CPU1_TO_CPU0);

	return XST_SUCCESS;
}
//-----------------------------------------------------------------------------
//static int SetUpInterruptSystem(XScuGic *XScuGicInstancePtr)
//{
//
//	/*
//	 * Connect the interrupt controller interrupt handler to the hardware
//	 * interrupt handling logic in the ARM processor.
//	 */
//	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
//			(Xil_ExceptionHandler) XScuGic_InterruptHandler,
//			XScuGicInstancePtr);
//
//	/*
//	 * Enable interrupts in the ARM
//	 */
//	Xil_ExceptionEnable();
//
//	return XST_SUCCESS;
//}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//=============================================================================


//=============================================================================
/*----------------------------------- IRQ -----------------------------------*/
//=============================================================================
void DeviceDriverHandler(void *CallbackRef){

	while(COMM_VAL == 1);
	xil_printf("CPU0: Got something from CPU1\n\r");

	COMM_VAL = 1;
}
//=============================================================================
