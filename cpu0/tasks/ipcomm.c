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

#include "axi_test.h"

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

#define AXI_TEST_BASE_ADR	0x43C10000

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
XScuGic *InterruptControllerInst; 	     /* Instance of the Interrupt Controller */

uint32_t irqCounter = 0;

uint32_t cpu1flag = 1;
//=============================================================================


//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static int ipcommSysInit(void);
void DeviceDriverHandler(void *CallbackRef);

static uint32_t ipcommCmdCpu1Blink(uifaceDataExchange_t *data);
static uint32_t ipcommCmdCpu1AdcEn(uifaceDataExchange_t *data);
static uint32_t ipcommCmdCpu1AdcSpiFreq(uifaceDataExchange_t *data);
static uint32_t ipcommCmdCpu1AdcSamplingFreq(uifaceDataExchange_t *data);
static uint32_t ipcommCmdCpu1AdcErrorRead(uifaceDataExchange_t *data);
static uint32_t ipcommCmdCpu1AdcErrorClear(uifaceDataExchange_t *data);
static uint32_t ipcommCmdCpu1AdcSamplingFreq(uifaceDataExchange_t *data);
static uint32_t ipcommCmdCpu1TraceStart(uifaceDataExchange_t *data);
static uint32_t ipcommCmdTraceSizeSet(uifaceDataExchange_t *data);
static uint32_t ipcommCmdTraceSizeRead(uifaceDataExchange_t *data);

static uint32_t ipcommCmdTraceRead(uifaceDataExchange_t *data);

static uint32_t ipcommCmdControlEn(uifaceDataExchange_t *data);

//=============================================================================

//=============================================================================
/*---------------------------------- Task -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void ipcomm(void *param){

	InterruptControllerInst = param;

	ipcommSysInit();

	vTaskDelay(5000 / portTICK_PERIOD_MS);


//	AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 4, 0x0000000A);
//	AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 8, 10000);
//	AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 12, 0xFFFF8000);

//	AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 0, (500000U << 1U) | 1U);

//	AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 4, 5000000);

//	AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 4, 5000000);

	while(1){

		vTaskDelay(2500 / portTICK_PERIOD_MS);

//		xil_printf("%s: Triggering AXI\r\n\r\n", __FUNCTION__);
//		AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 0, 1U);
//		vTaskDelay(2500 / portTICK_PERIOD_MS);
//		xil_printf("%s: irqCounter: %d\r\n\r\n", __FUNCTION__, irqCounter);
//		AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 0, 0);
//		vTaskDelay(2500 / portTICK_PERIOD_MS);
//
//		irqCounter = 0;


//		AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 0, write);
//		recv = AXI_TEST_mReadReg(AXI_TEST_BASE_ADR, 0);
//		xil_printf("%s: written: 0x%08X, read: 0x%08X\r\n\r\n", __FUNCTION__, write, recv);
//
//		vTaskDelay(1500 / portTICK_PERIOD_MS);
//
//		recv = Xil_In32(0xFFFF8000);
//		xil_printf("%s: ocm before writing: 0x%08X\r\n\r\n", __FUNCTION__, recv);
//		vTaskDelay(1500 / portTICK_PERIOD_MS);
//
//		Xil_Out32(0xFFFF8000, 0xAAAAAAAA);
//		recv = Xil_In32(0xFFFF8000);
//		xil_printf("%s: ocm after writing: 0x%08X\r\n\r\n", __FUNCTION__, recv);
//		vTaskDelay(1500 / portTICK_PERIOD_MS);

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
	uifaceRegisterHandle(SOC_CMD_CPU0_CPU1_ADC_EN, ipcommCmdCpu1AdcEn);
	uifaceRegisterHandle(SOC_CMD_CPU0_CPU1_ADC_SPI_FREQ, ipcommCmdCpu1AdcSpiFreq);
	uifaceRegisterHandle(SOC_CMD_CPU0_CPU1_ADC_SAMPLING_FREQ, ipcommCmdCpu1AdcSamplingFreq);
	uifaceRegisterHandle(SOC_CMD_CPU0_CPU1_ADC_SAMPLING_FREQ, ipcommCmdCpu1AdcSamplingFreq);
	uifaceRegisterHandle(SOC_CMD_CPU0_CPU1_ADC_ERROR_READ, ipcommCmdCpu1AdcErrorRead);
	uifaceRegisterHandle(SOC_CMD_CPU0_CPU1_ADC_ERROR_CLEAR, ipcommCmdCpu1AdcErrorClear);
	uifaceRegisterHandle(SOC_CMD_CPU0_TRACE_START, ipcommCmdCpu1TraceStart);
	uifaceRegisterHandle(SOC_CMD_CPU0_TRACE_READ, ipcommCmdTraceRead);
	uifaceRegisterHandle(SOC_CMD_CPU0_TRACE_SIZE_SET, ipcommCmdTraceSizeSet);
	uifaceRegisterHandle(SOC_CMD_CPU0_TRACE_SIZE_READ, ipcommCmdTraceSizeRead);
	uifaceRegisterHandle(SOC_CMD_CPU0_CONTROL_EN, ipcommCmdControlEn);

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
static uint32_t ipcommCmdCpu1AdcEn(uifaceDataExchange_t *data){

	uint32_t en;
	uint32_t *p;

	en = data->buffer[0];

	p = (uint32_t *)SOC_MEM_CPU0_TO_CPU1_ADR;
	*p++ = SOC_CMD_CPU1_ADC_EN;
	*p = en;

	XScuGic_SoftwareIntr ( InterruptControllerInst , IPCOMM_INT_CPU0_TO_CPU1 , SOC_SIG_CPU1_ID ) ;

	return 0;
}
//-----------------------------------------------------------------------------
static uint32_t ipcommCmdCpu1AdcSpiFreq(uifaceDataExchange_t *data){

	uint32_t freq;
	uint32_t *p;

	freq = (data->buffer[0] << 24) | (data->buffer[1] << 16) | (data->buffer[2] << 8) | data->buffer[3];

	p = (uint32_t *)SOC_MEM_CPU0_TO_CPU1_ADR;
	*p++ = SOC_CMD_CPU1_ADC_SPI_FREQ;
	*p = freq;

	XScuGic_SoftwareIntr ( InterruptControllerInst , IPCOMM_INT_CPU0_TO_CPU1 , SOC_SIG_CPU1_ID ) ;

	return 0;
}
//-----------------------------------------------------------------------------
static uint32_t ipcommCmdCpu1AdcSamplingFreq(uifaceDataExchange_t *data){

	uint32_t freq;
	uint32_t *p;

	freq = (data->buffer[0] << 24) | (data->buffer[1] << 16) | (data->buffer[2] << 8) | data->buffer[3];

	p = (uint32_t *)SOC_MEM_CPU0_TO_CPU1_ADR;
	*p++ = SOC_CMD_CPU1_ADC_SAMPLING_FREQ;
	*p = freq;

	XScuGic_SoftwareIntr ( InterruptControllerInst , IPCOMM_INT_CPU0_TO_CPU1 , SOC_SIG_CPU1_ID ) ;

	return 0;
}
//-----------------------------------------------------------------------------
static uint32_t ipcommCmdCpu1TraceStart(uifaceDataExchange_t *data){

	uint32_t *p;

	p = (uint32_t *)SOC_MEM_CPU0_TO_CPU1_ADR;
	*p++ = SOC_CMD_CPU1_TRACE_START;

	XScuGic_SoftwareIntr ( InterruptControllerInst , IPCOMM_INT_CPU0_TO_CPU1 , SOC_SIG_CPU1_ID ) ;

	return 0;
}
//-----------------------------------------------------------------------------
static uint32_t ipcommCmdTraceRead(uifaceDataExchange_t *data){

	uint32_t size;
	uint32_t *p;

	cpu1flag = 1;

	p = (uint32_t *)SOC_MEM_CPU0_TO_CPU1_ADR;
	*p++ = SOC_CMD_CPU1_TRACE_SIZE_READ;

	XScuGic_SoftwareIntr ( InterruptControllerInst , IPCOMM_INT_CPU0_TO_CPU1 , SOC_SIG_CPU1_ID ) ;

	while(cpu1flag == 1);

	p = (uint32_t *)SOC_MEM_CPU1_TO_CPU0_ADR;
	size = *p;

	data->buffer = (uint8_t*)SOC_MEM_TRACE_ADR;
	data->size = size;

	return 1;
}
//-----------------------------------------------------------------------------
static uint32_t ipcommCmdTraceSizeSet(uifaceDataExchange_t *data){

	uint32_t *p;
	uint32_t size;

	size = (data->buffer[0] << 24) | (data->buffer[1] << 16) | (data->buffer[2] << 8) | data->buffer[3];

	p = (uint32_t *)SOC_MEM_CPU0_TO_CPU1_ADR;
	*p++ = SOC_CMD_CPU1_TRACE_SIZE_SET;
	*p++ = size;

	XScuGic_SoftwareIntr ( InterruptControllerInst , IPCOMM_INT_CPU0_TO_CPU1 , SOC_SIG_CPU1_ID ) ;

	return 0;
}
//-----------------------------------------------------------------------------
static uint32_t ipcommCmdTraceSizeRead(uifaceDataExchange_t *data){

	uint32_t size;
	uint32_t *p;

	cpu1flag = 1;

	p = (uint32_t *)SOC_MEM_CPU0_TO_CPU1_ADR;
	*p++ = SOC_CMD_CPU1_TRACE_SIZE_READ;

	XScuGic_SoftwareIntr ( InterruptControllerInst , IPCOMM_INT_CPU0_TO_CPU1 , SOC_SIG_CPU1_ID ) ;

	while(cpu1flag == 1);

	p = (uint32_t *)SOC_MEM_CPU1_TO_CPU0_ADR;
	size = *p;

	data->buffer[0] = (uint8_t)(size >> 24);
	data->buffer[1] = (uint8_t)(size >> 16);
	data->buffer[2] = (uint8_t)(size >> 8);
	data->buffer[3] = (uint8_t)(size);
	data->size = 4;

	return 1;
}
//-----------------------------------------------------------------------------
static uint32_t ipcommCmdCpu1AdcErrorRead(uifaceDataExchange_t *data){

	uint32_t error;
	uint32_t *p;

	cpu1flag = 1;

	p = (uint32_t *)SOC_MEM_CPU0_TO_CPU1_ADR;
	*p++ = SOC_CMD_CPU1_ADC_ERROR_READ;

	XScuGic_SoftwareIntr ( InterruptControllerInst , IPCOMM_INT_CPU0_TO_CPU1 , SOC_SIG_CPU1_ID ) ;

	while(cpu1flag == 1);

	p = (uint32_t *)SOC_MEM_CPU1_TO_CPU0_ADR;
	error = *p;

	data->buffer[0] = (uint8_t)(error >> 24);
	data->buffer[1] = (uint8_t)(error >> 16);
	data->buffer[2] = (uint8_t)(error >> 8);
	data->buffer[3] = (uint8_t)(error);
	data->size = 4;

	return 1;
}
//-----------------------------------------------------------------------------
static uint32_t ipcommCmdCpu1AdcErrorClear(uifaceDataExchange_t *data){

	uint32_t *p;

	p = (uint32_t *)SOC_MEM_CPU0_TO_CPU1_ADR;
	*p++ = SOC_CMD_CPU1_ADC_ERROR_CLEAR;

	XScuGic_SoftwareIntr ( InterruptControllerInst , IPCOMM_INT_CPU0_TO_CPU1 , SOC_SIG_CPU1_ID ) ;

	return 0;
}
//-----------------------------------------------------------------------------
static uint32_t ipcommCmdControlEn(uifaceDataExchange_t *data){

	uint32_t en;
	uint32_t *p;

	en = data->buffer[0];

	p = (uint32_t *)SOC_MEM_CPU0_TO_CPU1_ADR;
	*p++ = SOC_CMD_CPU1_CONTROL_EN;
	*p = en;

	XScuGic_SoftwareIntr ( InterruptControllerInst , IPCOMM_INT_CPU0_TO_CPU1 , SOC_SIG_CPU1_ID ) ;

	return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================


//=============================================================================
/*----------------------------------- IRQ -----------------------------------*/
//=============================================================================
void DeviceDriverHandler(void *CallbackRef){

	cpu1flag = 0;
//	xil_printf("CPU0: Got something from CPU1\n\r");
	irqCounter++;
}
//=============================================================================
