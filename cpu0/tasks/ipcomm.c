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
#include "semphr.h"

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

#include "ipcClient.h"
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

/* CPU1->CPU0 reply timeout, in system ticks */
#define IPCOMM_CONFIG_CPU1_REPLY_TO		(IPCOMM_CONFIG_CPU1_REPLY_TO_MS / portTICK_PERIOD_MS)

/* Functions binding to CPU1 commands should have this signature */
typedef uifaceHandle_t ipcommCMDHandle_t;

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

	/*
	 * Semaphore used to signal a response from CPU1.
	 *
	 * This semaphore is taken before a command is issued to CPU1. When CPU1
	 * replies back, an interrupt is generated in CPU0, which releases the
	 * semaphore. Thus, the semaphore is used to indicate that CPU1 replied
	 * to a command issued by CPU0.
	 */
	SemaphoreHandle_t cpu1Semaphore;

	uint8_t reqbuffer[2048];

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

static void ipCommInitializeCMDs(void);

static void ipcommCMDRegister(uint32_t cpu0cmd, uint32_t cpu1cmd, ipcommCMDHandle_t handle);
//static uint32_t ipcommCMDFind(uint32_t cpu0cmd);
//static int32_t ipcommCMDExecute(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

void ipcommIRQCPU1(void *CallbackRef);

static void ipcommIpcInit(void);
static int32_t ipcommIrqSend();
static int32_t ipcommIrqReceive(uint32_t timeout);

static int32_t ipcommCmdCpu1Blink(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t ipcommCmdCpu1AdcEn(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t ipcommCmdCpu1AdcSpiFreqSet(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t ipcommCmdCpu1AdcSamplingFreqSet(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t ipcommCmdCpu1AdcErrorRead(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t ipcommCmdCpu1AdcErrorClear(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t ipcommCmdCpu1TraceStart(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t ipcommCmdCpu1TraceReadTags(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t ipcommCmdCpu1TraceRead(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t ipcommCmdCpu1TraceSizeSet(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t ipcommCmdCpu1TraceSizeRead(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t ipcommCmdCpu1ControlEn(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t ipcommRequest(uint32_t cmd, void *in, uint32_t insize, void **out, uint32_t maxoutsize);
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

	ipCommInitializeCMDs();

	xipcommControl.cpu1Semaphore = xSemaphoreCreateBinary();

	ipcommIpcInit();

	return XST_SUCCESS;
}
//-----------------------------------------------------------------------------
static void ipcommIpcInit(void){

	ipcClientInitialize(ipcommIrqSend, ipcommIrqReceive,
			SOC_MEM_CPU1_TO_CPU0_ADR, SOC_MEM_CPU1_TO_CPU0_SIZE,
			SOC_MEM_CPU0_TO_CPU1_ADR, SOC_MEM_CPU0_TO_CPU1_SIZE);
}
//-----------------------------------------------------------------------------
static int32_t ipcommIrqSend(void){

	/* Generates a software interrupt on CPU1 */
	XScuGic_SoftwareIntr ( xipcommControl.intcInstance, IPCOMM_INT_CPU0_TO_CPU1, SOC_SIG_CPU1_ID );

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ipcommIrqReceive(uint32_t timeout){

	/* Waits until CPU1 replies back */
	if( xSemaphoreTake(xipcommControl.cpu1Semaphore, IPCOMM_CONFIG_CPU1_REPLY_TO) != pdTRUE ){
		return IPCOMM_ERR_CPU1_REPLY_TO;
	}

	return 0;
}
//-----------------------------------------------------------------------------
static void ipCommInitializeCMDs(void){

	ipcommCMDRegister(SOC_CMD_CPU0_BLINK_CPU1, SOC_CMD_CPU1_BLINK, ipcommCmdCpu1Blink);

	ipcommCMDRegister(SOC_CMD_CPU0_ADC_EN, SOC_CMD_CPU1_ADC_EN, ipcommCmdCpu1AdcEn);

	ipcommCMDRegister(SOC_CMD_CPU0_ADC_SPI_FREQ_SET, SOC_CMD_CPU1_ADC_SPI_FREQ_SET, ipcommCmdCpu1AdcSpiFreqSet);

	ipcommCMDRegister(SOC_CMD_CPU0_ADC_SAMPLING_FREQ_SET, SOC_CMD_CPU1_ADC_SAMPLING_FREQ_SET, ipcommCmdCpu1AdcSamplingFreqSet);

	ipcommCMDRegister(SOC_CMD_CPU0_ADC_ERROR_READ, SOC_CMD_CPU1_ADC_ERROR_READ, ipcommCmdCpu1AdcErrorRead);

	ipcommCMDRegister(SOC_CMD_CPU0_ADC_ERROR_CLEAR, SOC_CMD_CPU1_ADC_ERROR_CLEAR, ipcommCmdCpu1AdcErrorClear);

	ipcommCMDRegister(SOC_CMD_CPU0_TRACE_START, SOC_CMD_CPU1_TRACE_START, ipcommCmdCpu1TraceStart);

	ipcommCMDRegister(SOC_CMD_CPU0_TRACE_READ_TAGS, SOC_CMD_CPU1_TRACE_READ_TAGS, ipcommCmdCpu1TraceReadTags);

	ipcommCMDRegister(SOC_CMD_CPU0_TRACE_READ, SOC_CMD_CPU1_TRACE_READ, ipcommCmdCpu1TraceRead);

	ipcommCMDRegister(SOC_CMD_CPU0_TRACE_SIZE_SET, SOC_CMD_CPU1_TRACE_SIZE_SET, ipcommCmdCpu1TraceSizeSet);

	ipcommCMDRegister(SOC_CMD_CPU0_TRACE_SIZE_READ, SOC_CMD_CPU1_TRACE_SIZE_READ, ipcommCmdCpu1TraceSizeRead);

	ipcommCMDRegister(SOC_CMD_CPU0_CONTROL_EN, SOC_CMD_CPU1_CONTROL_EN, ipcommCmdCpu1ControlEn);

}
//-----------------------------------------------------------------------------
static void ipcommCMDRegister(uint32_t cpu0cmd, uint32_t cpu1cmd, ipcommCMDHandle_t handle){

	xipcommControl.cmd[cpu1cmd] = cpu0cmd;
	uifaceRegisterHandle(cpu0cmd, handle);
}
////-----------------------------------------------------------------------------
//static uint32_t ipcommCMDFind(uint32_t cpu0cmd){
//
//	uint32_t i;
//
//	for(i = 0; i < SOC_CMD_CPU1_END; i++){
//		if( xipcommControl.cmd[i] == cpu0cmd ) break;
//	}
//
//	return i;
//}
////-----------------------------------------------------------------------------
//static int32_t ipcommCMDExecute(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
//
//	uint32_t cpu0cmd, cpu1cmd;
//	uint8_t *src, *dst;
//	uint32_t *p;
//	uint32_t i;
//	uint32_t status;
//
//	/*
//	 * If amount of data to be passed to CPU1 exceeds the available memory,
//	 * an error is generated.
//	 */
//	if( insize > SOC_MEM_CPU0_TO_CPU1_SIZE ) return IPCOMM_ERR_CPU0_CPU1_BUFFER_OVERFLOW;
//
//	p = (uint32_t)in;
//	cpu0cmd = *p++;
//	cpu1cmd = ipcommCMDFind(cpu0cmd);
//
//	/* If the command received does not exist, returns an error */
//	if( cpu1cmd >= SOC_CMD_CPU1_END) return IPCOMM_ERR_CPU1_INVALID_CMD;
//
//	xSemaphoreTake(xipcommControl.cpu1Semaphore, 0);
//
//	/*
//	 * Transferring data to CPU1 follows the procedure described in the
//	 * soc_defs.h file.
//	 */
//	/* Writes command to be executed */
//	*( (uint32_t *)SOC_MEM_CPU0_TO_CPU1_CMD ) = cpu1cmd;
//
//	/* Writes size of data (in number of bytes) */
//	*( (uint32_t *)SOC_MEM_CPU0_TO_CPU1_CMD_SIZE ) = insize;
//
//	/*
//	 * Writes where data will be located at. Here, we will always copy data
//	 * from the uiface buffer to the CPU0->CPU1 buffer.
//	 */
//	if( insize > 0 ){
//		//*( (uint32_t *)SOC_MEM_CPU0_TO_CPU1_CMD_DATA_ADDR ) = SOC_MEM_CPU0_TO_CPU1_DATA;
//		dst = (uint8_t *)(SOC_MEM_CPU0_TO_CPU1_DATA);
//		src = (uint8_t *)p;
//		for(i = 0; i < insize; i++) *dst++ = *src++;
//	}
//
//	/* Generates a software interrupt on CPU1 */
//	XScuGic_SoftwareIntr ( xipcommControl.intcInstance, IPCOMM_INT_CPU0_TO_CPU1, SOC_SIG_CPU1_ID );
//
//	/* Waits until CPU1 replies back */
//	if( xSemaphoreTake(xipcommControl.cpu1Semaphore, IPCOMM_CONFIG_CPU1_REPLY_TO) != pdTRUE ){
//		return IPCOMM_ERR_CPU1_REPLY_TO;
//	}
//
//	/*
//	 * Gets the command status and writes to pbuf the address of where the
//	 * data (if any) is located at.
//	 */
//	status = *( (uint32_t *)(SOC_MEM_CPU1_TO_CPU0_CMD_STATUS + 4U));
//	*out = (uint8_t *) ( *( (uint32_t *)(SOC_MEM_CPU1_TO_CPU0_CMD_DATA_ADDR + 4U) ) );
//
//	return status;
//}
//-----------------------------------------------------------------------------
static int32_t ipcommRequest(uint32_t cmd, void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	int32_t ret;
	uint32_t k;
	uint8_t *pin;
	uint8_t *pbuf;

	/* Copies command to first 4 bytes of reqbuffer */
	*( (uint32_t *)( xipcommControl.reqbuffer ) ) = cmd;

	/* Copies the request to reqbuffer */
	pin = (uint8_t *)in;
	pbuf = &xipcommControl.reqbuffer[4];
	k = insize;
	while( k-- ){
		*pbuf++ = *pin++;
	}

	xSemaphoreTake(xipcommControl.cpu1Semaphore, 0);

	ret = ipcClientRequest((void *)(xipcommControl.reqbuffer), insize + 4,
			(void *)*out, maxoutsize,
			IPCOMM_CONFIG_CPU1_REPLY_TO);

	return ret;
}
//-----------------------------------------------------------------------------
static int32_t ipcommCmdCpu1Blink(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	int32_t ret;
	uint32_t cmd;

	cmd = SOC_CMD_CPU1_BLINK;

	ret = ipcommRequest(cmd, in, insize, out, maxoutsize);

	return ret;
}
//-----------------------------------------------------------------------------
static int32_t ipcommCmdCpu1AdcEn(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	int32_t ret;
	uint32_t cmd;

	cmd = SOC_CMD_CPU1_ADC_EN;

	ret = ipcommRequest(cmd, in, insize, out, maxoutsize);

	return ret;
}
//-----------------------------------------------------------------------------
static int32_t ipcommCmdCpu1AdcSpiFreqSet(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	int32_t ret;
	uint32_t cmd;

	cmd = SOC_CMD_CPU1_ADC_SPI_FREQ_SET;

	ret = ipcommRequest(cmd, in, insize, out, maxoutsize);

	return ret;
}
//-----------------------------------------------------------------------------
static int32_t ipcommCmdCpu1AdcSamplingFreqSet(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	int32_t ret;
	uint32_t cmd;

	cmd = SOC_CMD_CPU1_ADC_SAMPLING_FREQ_SET;

	ret = ipcommRequest(cmd, in, insize, out, maxoutsize);

	return ret;
}
//-----------------------------------------------------------------------------
static int32_t ipcommCmdCpu1AdcErrorRead(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	int32_t ret;
	uint32_t cmd;

	cmd = SOC_CMD_CPU1_ADC_ERROR_READ;

	ret = ipcommRequest(cmd, in, insize, out, maxoutsize);

	return ret;
}
//-----------------------------------------------------------------------------
static int32_t ipcommCmdCpu1AdcErrorClear(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	int32_t ret;
	uint32_t cmd;

	cmd = SOC_CMD_CPU1_ADC_ERROR_CLEAR;

	ret = ipcommRequest(cmd, in, insize, out, maxoutsize);

	return ret;
}
//-----------------------------------------------------------------------------
static int32_t ipcommCmdCpu1TraceStart(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	int32_t ret;
	uint32_t cmd;

	cmd = SOC_CMD_CPU1_TRACE_START;

	ret = ipcommRequest(cmd, in, insize, out, maxoutsize);

	return ret;
}
//-----------------------------------------------------------------------------
static int32_t ipcommCmdCpu1TraceReadTags(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	int32_t ret;
	uint32_t cmd;

	cmd = SOC_CMD_CPU1_TRACE_READ_TAGS;

	ret = ipcommRequest(cmd, in, insize, out, maxoutsize);

	return ret;
}
//-----------------------------------------------------------------------------
static int32_t ipcommCmdCpu1TraceRead(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	int32_t ret;
	uint32_t cmd;

	uint32_t *p;
	int32_t traceSize;
	uint32_t traceAddress;

	/*
	 * To read the trace, we'll first read its size and then get its address.
	 */
	p = (uint32_t *)( &traceSize );
	cmd = SOC_CMD_CPU1_TRACE_SIZE_READ;
	ret = ipcommRequest(cmd, 0, 0, (void **)( &p ), 4);
	if( ret < 0 ) return ret;

	p = (uint32_t *)( &traceAddress );
	cmd = SOC_CMD_CPU1_TRACE_READ;
	ret = ipcommRequest(cmd, 0, 0, (void **)( &p ), 4);
	if( ret < 0 ) return ret;

	*out = (uint32_t *)traceAddress;

	return traceSize;
}
//-----------------------------------------------------------------------------
static int32_t ipcommCmdCpu1TraceSizeSet(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	int32_t ret;
	uint32_t cmd;

	cmd = SOC_CMD_CPU1_TRACE_SIZE_SET;

	ret = ipcommRequest(cmd, in, insize, out, maxoutsize);

	return ret;
}
//-----------------------------------------------------------------------------
static int32_t ipcommCmdCpu1TraceSizeRead(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	int32_t ret;
	uint32_t cmd;

	cmd = SOC_CMD_CPU1_TRACE_SIZE_READ;

	ret = ipcommRequest(cmd, in, insize, out, maxoutsize);

	return ret;
}
//-----------------------------------------------------------------------------
static int32_t ipcommCmdCpu1ControlEn(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	int32_t ret;
	uint32_t cmd;

	cmd = SOC_CMD_CPU1_CONTROL_EN;

	ret = ipcommRequest(cmd, in, insize, out, maxoutsize);

	return ret;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------------- IRQ -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void ipcommIRQCPU1(void *CallbackRef){

	BaseType_t xHigherPriorityTaskWoken;

	xipcommControl.cpu1flag = 0;

	xSemaphoreGiveFromISR( xipcommControl.cpu1Semaphore, &xHigherPriorityTaskWoken );

	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
//-----------------------------------------------------------------------------
//=============================================================================
