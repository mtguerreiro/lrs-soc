/*
 * ipcomm.c
 *
 *  Created on: 23.05.2022
 *      Author: Marco Guerreiro
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
//#include "netif/xadapter.h"
//#include "platform_config.h"
#include "xil_printf.h"
#include "xil_types.h"

#include "xil_exception.h"
#include "xscugic_hw.h"
#include "xil_printf.h"
#include "xstatus.h"
#include "xscugic.h"

#include "soc_defs.h"

/* Tasks */
#include "uiface.h"
//=============================================================================

//=============================================================================
/*--------------------------------- Defines ---------------------------------*/
//=============================================================================
/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define INTC_DEVICE_ID		XPAR_SCUGIC_0_DEVICE_ID

#define IPCOMM_INT_CPU0_TO_CPU1		SOC_SIG_CPU0_TO_CPU1
#define IPCOMM_INT_CPU1_TO_CPU0		SOC_SIG_CPU1_TO_CPU0

#define SYNC_FLAG  		(*(volatile unsigned long *)(SOC_CPU0_CPU1_SYNC_FLAG_ADR))
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
XScuGic *InterruptControllerInst; 	     /* Instance of the Interrupt Controller */
//=============================================================================


//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static int ipcommSysInit(void);
void DeviceDriverHandler(void *CallbackRef);

static uint32_t ipcommCmdCpu1Blink(uifaceDataExchange_t *data);
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

//		XScuGic_SoftwareIntr ( InterruptControllerInst , IPCOMM_INT_CPU0_TO_CPU1 , SOC_SIG_CPU1_ID ) ;
	}
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//=============================================================================


//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int ipcommSysInit(void){

	XScuGic_Connect(InterruptControllerInst, IPCOMM_INT_CPU1_TO_CPU0, (Xil_ExceptionHandler)DeviceDriverHandler, (void  *)InterruptControllerInst) ;
	XScuGic_Enable(InterruptControllerInst, IPCOMM_INT_CPU1_TO_CPU0);

	uifaceRegisterHandle(SOC_CMD_CPU0_CPU1_BLINK, ipcommCmdCpu1Blink);
	return XST_SUCCESS;
}
//-----------------------------------------------------------------------------
static uint32_t ipcommCmdCpu1Blink(uifaceDataExchange_t *data){

	uint32_t period;
	uint32_t *p;

	period = (data->buffer[0] << 8) | data->buffer[1];


	p = (uint32_t *)SOC_MEM_CPU0_TO_CPU1_ADR;
	*p++ = SOC_CMD_CPU1_BLINK;
	*p = period;

	XScuGic_SoftwareIntr ( InterruptControllerInst , IPCOMM_INT_CPU0_TO_CPU1 , SOC_SIG_CPU1_ID ) ;

    return 0;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//=============================================================================


//=============================================================================
/*----------------------------------- IRQ -----------------------------------*/
//=============================================================================
void DeviceDriverHandler(void *CallbackRef){

	while(SYNC_FLAG == 1);
	xil_printf("CPU0: Got something from CPU1\n\r");

	SYNC_FLAG = 1;
}
//=============================================================================
