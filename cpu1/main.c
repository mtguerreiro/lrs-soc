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
//=============================================================================

//=============================================================================
/*--------------------------------- Defines ---------------------------------*/
//=============================================================================
#define SYNC_FLAG  		(*(volatile unsigned long *)(SOC_CPU0_CPU1_SYNC_FLAG_ADR))

#define LED_ID XPAR_AXI_GPIO_RGB_LED_DEVICE_ID
#define LED_CHANNEL 1
#define LED_MASK 0b111

#define RELAY_ID 			XPAR_AXI_GPIO_RGB_LED_DEVICE_ID
#define RELAY_CHANNEL 		1

#define GPIODEBUG_ID 		XPAR_AXI_GPIO_DEBUG_DEVICE_ID
#define GPIODEBUG_CHANNEL 	1

#define INTC		    XScuGic
#define INTC_DEVICE_ID	XPAR_PS7_SCUGIC_0_DEVICE_ID
#define INTC_HANDLER	XScuGic_InterruptHandler

typedef int32_t(*mainCmdHandle_t)(uint32_t **data);

typedef struct{
	uint32_t *p;
	uint32_t *end;
}mainTrace_t;


typedef struct{
	mainCmdHandle_t cmdHandle[SOC_CMD_CPU1_END];

	mainTrace_t trace;

	uint32_t error;

	uint32_t precharge;

	uint32_t enable;

}mainControl_t;
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
mainControl_t mainControl;

INTC   IntcInstancePtr;

XGpio_Config *cfg_ptr = 0;
XGpio led_device;
XGpio relay_device;
XGpio gpioDebug_device;

uint32_t blinkPeriod = 1000;


float ts = 1/5e3;
float h = 1.0; // frequency component to control


float e = 0.0, e_1 = 0.0;
float u_pi = 0.0, u_pi_1 = 0.0;

float ig_ref;

float a1, b0, b1;

//float Kp = 0.01, Ki = 1.0;
//float Kp = 0.01, Ki = 0.01;
//float Kp = 1.2, Ki = 10.0;
//float Kp = 0.01, Ki = 1.0;
//float Kp = 0.1, Ki = 1.0;
//float Kp = 0.4, Ki = 15.0;

//float Kp = 0.01, Ki = 1.0; //for sure working
float Kp = 0.4, Ki = 15.0; // for sure working - fast

float v_dc_ref = 20.0;

//float Kp_pr = 0.1, Ki_pr = 10.0; // for sure working
float Kp_pr = 0.4, Ki_pr = 30.0; // for sure working - fast
//float Kp_pr = 0.01, Ki_pr = 0.1;
//float Kp_pr = 0.01, Ki_pr = 10.0;
//float Kp_pr = 1.2, Ki_pr = 40.0;
//float Kp_pr = 0.01, Ki_pr = 10.0;
//float Kp_pr = 0.1, Ki_pr = 10.0;
//float Kp_pr = 0.2, Ki_pr = 15.0;

float w0 = 2.0*3.141592653589793*50.0, wc = 15.0;
float a0_pr, a1_pr, a2_pr;
float b0_pr, b1_pr, b2_pr;

float u_pr = 0.0, u_pr_1 = 0.0, u_pr_2 = 0.0, ei = 0.0, ei_1 = 0.0, ei_2 = 0.0;

float vs_ref;
float vs_ref_norm;


float v_ac_peak;

//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static int mainSysInit(void);
static int mainSetupIntrSystem(INTC *IntcInstancePtr);

static void mainInputRelayDisable(void);
static void mainInputRelayEnable(void);

static void mainOutputRelayDisable(void);
static void mainOutputRelayEnable(void);

static int32_t mainCmdBlink(uint32_t **data);
static int32_t mainCmdAdcEn(uint32_t **data);
static int32_t mainCmdAdcSpiFreq(uint32_t **data);
static int32_t mainCmdAdcSamplingFreq(uint32_t **data);
static int32_t mainCmdAdcErrorRead(uint32_t **data);
static int32_t mainCmdAdcErrorClear(uint32_t **data);
static int32_t mainCmdTraceStart(uint32_t **data);
static int32_t mainCmdTraceRead(uint32_t **data);
static int32_t mainCmdTraceSizeSet(uint32_t **data);
static int32_t mainCmdTraceSizeRead(uint32_t **data);

static int32_t mainCmdControlEn(uint32_t **data);

static void mainControlReset(void);

void DeviceDriverHandler(void *CallbackRef);
void PLirqHandler(void *CallbackRef);

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
	uint32_t data;

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


	/* Initializes PYNQ's (RGB) LEDs */
    cfg_ptr = XGpio_LookupConfig(LED_ID);
	XGpio_CfgInitialize(&led_device, cfg_ptr, cfg_ptr->BaseAddress);
	XGpio_SetDataDirection(&led_device, LED_CHANNEL, 0);

	/* Initializes and turns relays off */
	cfg_ptr = XGpio_LookupConfig(XPAR_AXI_GPIO_RELAY_DEVICE_ID);
	XGpio_CfgInitialize(&relay_device, cfg_ptr, cfg_ptr->BaseAddress);
	XGpio_SetDataDirection(&relay_device, RELAY_CHANNEL, 0);

	cfg_ptr = XGpio_LookupConfig(XPAR_AXI_GPIO_DEBUG_DEVICE_ID);
	XGpio_CfgInitialize(&gpioDebug_device, cfg_ptr, cfg_ptr->BaseAddress);
	XGpio_SetDataDirection(&gpioDebug_device, GPIODEBUG_CHANNEL, 0);
	XGpio_DiscreteWrite(&gpioDebug_device, GPIODEBUG_CHANNEL, 0);


	mainInputRelayDisable();
	mainOutputRelayDisable();


//	mainInputRelayEnable();
//	mainOutputRelayEnable();

//	XGpio_DiscreteWrite(&relay_device, RELAY_CHANNEL, 0x03);
//	XGpio_DiscreteWrite(&relay_device, RELAY_CHANNEL, 0x00);
//	XGpio_DiscreteWrite(&relay_device, RELAY_CHANNEL, 0x07);

	/* Commands */
	mainControl.cmdHandle[SOC_CMD_CPU1_BLINK] = mainCmdBlink;
	mainControl.cmdHandle[SOC_CMD_CPU1_ADC_EN] = mainCmdAdcEn;
	mainControl.cmdHandle[SOC_CMD_CPU1_ADC_SPI_FREQ_SET] = mainCmdAdcSpiFreq;
	mainControl.cmdHandle[SOC_CMD_CPU1_ADC_SAMPLING_FREQ_SET] = mainCmdAdcSamplingFreq;
	mainControl.cmdHandle[SOC_CMD_CPU1_ADC_ERROR_READ] = mainCmdAdcErrorRead;
	mainControl.cmdHandle[SOC_CMD_CPU1_ADC_ERROR_CLEAR] = mainCmdAdcErrorClear;
	mainControl.cmdHandle[SOC_CMD_CPU1_TRACE_START] = mainCmdTraceStart;
	mainControl.cmdHandle[SOC_CMD_CPU1_TRACE_READ] = mainCmdTraceRead;
	mainControl.cmdHandle[SOC_CMD_CPU1_TRACE_SIZE_SET] = mainCmdTraceSizeSet;
	mainControl.cmdHandle[SOC_CMD_CPU1_TRACE_SIZE_READ] = mainCmdTraceSizeRead;
	mainControl.cmdHandle[SOC_CMD_CPU1_CONTROL_EN] = mainCmdControlEn;

	mainControl.trace.p = (uint32_t *)( SOC_MEM_TRACE_ADR );
	mainControl.trace.end = (uint32_t *)( SOC_MEM_TRACE_ADR + SOC_MEM_TRACE_SIZE_MAX );

	mainControl.error = 0;
	mainControl.precharge = 0;
	mainControl.enable = 0;

	/* Sets ADC stuff */
//	AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 0, 0x00000000);
	//AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 4, 0x0000000A);
	//AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 8, 10000);
	AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 4, 500);
//	AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 8, 20000);
	AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 12, SOC_MEM_PL_TO_CPU1_ADR);

	AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 0, 0U);


	a1 = 1.0;
	b0 = 1.0 / 2.0 * (2.0 * Kp + ts * Ki);
	b1 = 1.0 / 2.0 * (ts * Ki - 2.0 * Kp);

	//b0_pr = ts*ts*w0*w0 + 4.0*ts*wc + 4.0;
	//
	//a1_pr = 4.0 * Ki_pr * ts * wc / b0_pr;
	//
	//b1_pr = (2.0 * ts*ts * w0*w0 - 8.0) / b0_pr;
	//b2_pr = (ts*ts*w0*w0 - 4.0*ts*wc + 4.0) / b0_pr;

	/* PR with limited bandwidth */
//	b0_pr = ts*ts*w0*w0 + 4.0*ts*wc + 4.0;
//	b1_pr = (2.0 * ts*ts * w0*w0 - 8.0);
//	b2_pr = (ts*ts*w0*w0 - 4.0*ts*wc + 4.0);
//
//
//	a1_pr = 4.0 * Ki_pr * ts * wc;
//	a2_pr = -a1_pr;

	/* PR with infinite gain */
	b0_pr = ts*ts*w0*w0 + 4.0;
	b1_pr = (2.0 * ts*ts * w0*w0 - 8.0);
	b2_pr = (ts*ts*w0*w0 + 4.0);


	a0_pr = 4*Ki_pr*ts;
	a1_pr = 0;
	a2_pr = -a0_pr;

//	b0_pr = Ki_pr*sinf(h*w0*ts)/(2.0*h*w0);
//	b1_pr = Ki_pr*0.0;
//	b2_pr = Ki_pr*(-sinf(h*w0*ts)/(2.0*h*w0));
//
//	a1_pr = -2.0*cosf(h*w0*ts);
//	a2_pr = 1.000000000000000;

	v_ac_peak = 10.0 * sqrtf(2.0);
	//v_ac_peak = 23;

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


	XScuGic_Connect(IntcInstancePtr, SOC_SIG_CPU0_TO_CPU1, (Xil_ExceptionHandler)DeviceDriverHandler, IntcInstancePtr) ;
	XScuGic_Enable(IntcInstancePtr, SOC_SIG_CPU0_TO_CPU1);

	XScuGic_SetPriorityTriggerType(IntcInstancePtr, SOC_IRQ_PL_TO_CPU1, 0xA0, 0x3);
	XScuGic_Connect(IntcInstancePtr, SOC_IRQ_PL_TO_CPU1, (Xil_ExceptionHandler)PLirqHandler, IntcInstancePtr) ;
	XScuGic_Enable(IntcInstancePtr, SOC_IRQ_PL_TO_CPU1);

	return XST_SUCCESS;
}
//-----------------------------------------------------------------------------
static int32_t mainCmdBlink(uint32_t **data){

	blinkPeriod = *(*data);

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t mainCmdAdcEn(uint32_t **data){

	uint32_t en;

	en = *(*data);

//	if( en == 0 ) AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 0, 0U);
//	else AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 0, 1U);

	if( en == 0 ) AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 0, 0U);
	else AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 0, 1U);

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t mainCmdAdcSpiFreq(uint32_t **data){

	uint32_t en, freq;

	freq = *(*data);

	en = AXI_TEST_mReadReg(AXI_PWM_BASE_ADR, 0);

	if( (en & 1) == 1 ) AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 0, 0U);

	AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 4, freq);

	if( (en & 1) == 1 ) AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 0, en);

	return 0;

//	uint32_t en, freq;
//
//	freq = *data;
//
//	en = AXI_TEST_mReadReg(AXI_TEST_BASE_ADR, 0);
//
//	if( en == 1 ) AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 0, 0U);
//
//	AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 4, freq);
//
//	if( en == 1 ) AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 0, 1U);
//
//	return 0;
}
//-----------------------------------------------------------------------------
static int32_t mainCmdAdcSamplingFreq(uint32_t **data){

	uint32_t en, freq;

	freq = *(*data);

	en = AXI_TEST_mReadReg(AXI_PWM_BASE_ADR, 0);

	if( (en & 1) == 1 ) AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 0, 0U);

	//AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 0, 1U);
	AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 4, freq);
	AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 8, 0);

	//AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 8, freq);

	if( (en & 1) == 1 ) AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 0, en);

	return 0;

//	uint32_t en, freq;
//
//	freq = *data;
//
//	en = AXI_TEST_mReadReg(AXI_TEST_BASE_ADR, 0);
//
//	if( en == 1 ) AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 0, 0U);
//
//	AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 0, 1U);
//	AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 4, freq);
//	AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 8, freq >> 4U);
//
//	AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 8, freq);
//
//	if( en == 1 ) AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 0, 1U);
//
//	return 0;
}
//-----------------------------------------------------------------------------
static int32_t mainCmdTraceStart(uint32_t **data){

	mainControl.trace.p = (uint32_t *)SOC_MEM_TRACE_ADR;

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t mainCmdTraceRead(uint32_t **data){

	uint32_t size;

	size = ((uint32_t)mainControl.trace.end) - SOC_MEM_TRACE_ADR;

	*data = (uint32_t *)SOC_MEM_TRACE_ADR;

	return size;
}
//-----------------------------------------------------------------------------
static int32_t mainCmdTraceSizeSet(uint32_t **data){

	uint32_t size;

	size = *(*data);

	if( size <= SOC_MEM_TRACE_SIZE_MAX ){
		mainControl.trace.end = (uint32_t *)( SOC_MEM_TRACE_ADR + size );
	}

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t mainCmdTraceSizeRead(uint32_t **data){

	uint32_t *p;
	uint32_t size;

	p = (uint32_t *)( SOC_MEM_CPU1_TO_CPU0_CMD_DATA );
	size = ((uint32_t)mainControl.trace.end) - SOC_MEM_TRACE_ADR;
	*p = size;

	*data = (uint32_t *)( SOC_MEM_CPU1_TO_CPU0_CMD_DATA );

	return 4;
}
//-----------------------------------------------------------------------------
static int32_t mainCmdAdcErrorRead(uint32_t **data){

	uint32_t *p;

	p = (uint32_t *)( SOC_MEM_CPU1_TO_CPU0_CMD_DATA );
	*p = mainControl.error;

	*data = (uint32_t *)( SOC_MEM_CPU1_TO_CPU0_CMD_DATA );

	return 4;
}
//-----------------------------------------------------------------------------
static int32_t mainCmdAdcErrorClear(uint32_t **data){

	mainControl.error = 0;

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t mainCmdControlEn(uint32_t **data){

	uint32_t en;

	en = *(*data);

	if( en == 0 ) {
		mainControl.enable = 0;
		AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 0, 1U);
	}
	else {
		mainControl.enable = 1;
		AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 8, 0);
		AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 0, 3U);
	}

	mainControlReset();

	return 0;
}
//-----------------------------------------------------------------------------
static void mainInputRelayDisable(void){

	uint32_t data;

	data = XGpio_DiscreteRead(&relay_device, RELAY_CHANNEL);
	XGpio_DiscreteWrite(&relay_device, RELAY_CHANNEL, data | 0x06);

}
//-----------------------------------------------------------------------------
static void mainInputRelayEnable(void){

	uint32_t data;

	data = XGpio_DiscreteRead(&relay_device, RELAY_CHANNEL);
	XGpio_DiscreteWrite(&relay_device, RELAY_CHANNEL, data & 0x01);
}
//-----------------------------------------------------------------------------
static void mainOutputRelayDisable(void){

	uint32_t data;

	data = XGpio_DiscreteRead(&relay_device, RELAY_CHANNEL);
	XGpio_DiscreteWrite(&relay_device, RELAY_CHANNEL, data | 0x01);
}
//-----------------------------------------------------------------------------
static void mainOutputRelayEnable(void){

	uint32_t data;

	data = XGpio_DiscreteRead(&relay_device, RELAY_CHANNEL);
	XGpio_DiscreteWrite(&relay_device, RELAY_CHANNEL, data & 0x06);
}
//-----------------------------------------------------------------------------
static void mainControlReset(void){

	e = 0.0;
	e_1 = 0.0;

	u_pi = 0.0;
	u_pi_1 = 0.0;

	u_pr = 0.0;
	u_pr_1 = 0.0;
	u_pr_2 = 0.0;
	ei = 0.0;
	ei_1 = 0.0;
	ei_2 = 0.0;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------------- IRQ -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void DeviceDriverHandler(void *CallbackRef){

	uint32_t *p, *psrc;
	uint32_t **dp;

	uint32_t cmd;
	uint32_t size;
	uint32_t address;
	int32_t ret;

	/* Gets the command and data sent by CPU0 */
	cmd = *( (uint32_t *)SOC_MEM_CPU0_TO_CPU1_CMD );
	size = *( (uint32_t *)SOC_MEM_CPU0_TO_CPU1_CMD_SIZE );
	address = *( (uint32_t *)SOC_MEM_CPU0_TO_CPU1_CMD_DATA_ADDR );

	p = (uint32_t *)address;
	dp = (uint32_t **)&p;

	/* Executes the command */
	ret = mainControl.cmdHandle[cmd](dp);
	psrc = *dp;

	/* Replies back to CPU0 */
	*( (uint32_t *)SOC_MEM_CPU1_TO_CPU0_CMD_STATUS ) = ret;

	if( ret > 0 ){
		p = (uint32_t *)SOC_MEM_CPU1_TO_CPU0_CMD_DATA_ADDR;
		*p = (uint32_t) psrc;
	}

	XScuGic_SoftwareIntr ( &IntcInstancePtr , SOC_SIG_CPU1_TO_CPU0 , SOC_SIG_CPU0_ID ) ;
}
//-----------------------------------------------------------------------------
void PLirqHandler(void *CallbackRef){

	static float dcLinkVoltage_1 = 0.0;
	float dcLinkVoltage;
	float dclinkVoltageInst;
	float hbCurrent;
	static float hbCurrent_1 = 0.0;
	float gridVoltage;
	float loadCurrent;

	float v_dc;
	float v_ac;
	float i_ac;

	float d_i, d_v;


	float duty_float;
	uint32_t duty;

	uint32_t en;

	XGpio_DiscreteWrite(&gpioDebug_device, GPIODEBUG_CHANNEL, 3);



	/* Temporary solution to annoying init bug */
	en = AXI_TEST_mReadReg(AXI_PWM_BASE_ADR, 0);
	if( en == 0 ){
		XGpio_DiscreteWrite(&gpioDebug_device, GPIODEBUG_CHANNEL, 0);
		return;
	}

	/* Averages dc link voltage */
//	dclink = *((uint16_t *)(SOC_AFE_DCLINK));
//	dclink_avg = (dclink + dclink_1 + dclink_2 + dclink_3) >> 2U;
//	dclink_3 = dclink_2;
//	dclink_2 = dclink_1;
//	dclink_1 = dclink;

	/* Averages hb current */
//	ihb = *((uint16_t *)(SOC_AFE_HB_CURRENT));
//	ihb_avg = (ihb + ihb_1 + ihb_2 + ihb_3) >> 2U;
//	ihb_3 = ihb_2;
//	ihb_2 = ihb_1;
//	ihb_1 = ihb;

	dcLinkVoltage = SOC_ADC_TO_SIGNAL(*((uint16_t *)(SOC_AFE_DCLINK)), SOC_AFE_DCLINK_SENS_GAIN, SOC_AFE_DCLINK_SENS_OFFS);
	dclinkVoltageInst = dcLinkVoltage;
	d_v = dcLinkVoltage - dcLinkVoltage_1;
	if( (d_v > 6.0) || (d_v < (-6.0)) ) dcLinkVoltage = dcLinkVoltage_1;
	else dcLinkVoltage_1 = dcLinkVoltage;


	hbCurrent = SOC_ADC_TO_SIGNAL(*((uint16_t *)(SOC_AFE_HB_CURRENT)), SOC_AFE_HB_CURRENT_SENS_GAIN, SOC_AFE_HB_CURRENT_SENS_OFFS);
	d_i = hbCurrent - hbCurrent_1;
	if( (d_i > 5.0) || (d_i < (-5.0)) ) hbCurrent = hbCurrent_1;
	else hbCurrent_1 = hbCurrent;

	/* Converts ADC values to the actual measurements */
	//dclinkVoltageInst = SOC_ADC_TO_SIGNAL(*((uint16_t *)(SOC_AFE_DCLINK)), SOC_AFE_DCLINK_SENS_GAIN, SOC_AFE_DCLINK_SENS_OFFS);
	//dcLinkVoltage = SOC_ADC_TO_SIGNAL(dclink_avg, SOC_AFE_DCLINK_SENS_GAIN, SOC_AFE_DCLINK_SENS_OFFS);
	//hbCurrent = SOC_ADC_TO_SIGNAL(ihb, SOC_AFE_HB_CURRENT_SENS_GAIN, SOC_AFE_HB_CURRENT_SENS_OFFS);
	gridVoltage = SOC_ADC_TO_SIGNAL(*((uint16_t *)(SOC_AFE_GRID_VOLTAGE)), SOC_AFE_GRID_VOLTAGE_SENS_GAIN, SOC_AFE_GRID_VOLTAGE_SENS_OFFS);
	loadCurrent = SOC_ADC_TO_SIGNAL(*((uint16_t *)(SOC_AFE_LOAD_CURRENT)), SOC_AFE_LOAD_CURRENT_SENS_GAIN, SOC_AFE_LOAD_CURRENT_SENS_OFFS);

	v_dc = dcLinkVoltage;
	v_ac = gridVoltage;
	i_ac = hbCurrent;

	/* Checks if all is well */
	if( mainControl.precharge == 0 ){
		if( dcLinkVoltage > SOC_AFE_DC_LINK_PRECHARGE ) {
			mainInputRelayEnable();
			mainOutputRelayEnable();
			mainControl.precharge = 1;
		}
	}
	else{
		if( dcLinkVoltage < SOC_AFE_DC_LINK_MIN ) mainControl.error |= SOC_AFE_ERR_DCLINK_UNDER;
		if( dcLinkVoltage > SOC_AFE_DC_LINK_MAX ) mainControl.error |= SOC_AFE_ERR_DCLINK_OVER;
	}

	if( hbCurrent < SOC_AFE_HB_CURRENT_MIN ) mainControl.error |= SOC_AFE_ERR_HB_CURRENT_UNDER;
	if( hbCurrent > SOC_AFE_HB_CURRENT_MAX ) mainControl.error |= SOC_AFE_ERR_HB_CURRENT_OVER;

	if( gridVoltage < SOC_AFE_GRID_VOLTAGE_MIN ) mainControl.error |= SOC_AFE_ERR_GRID_VOLTAGE_UNDER;
	if( gridVoltage > SOC_AFE_GRID_VOLTAGE_MAX ) mainControl.error |= SOC_AFE_ERR_GRID_VOLTAGE_OVER;

	if( loadCurrent < SOC_AFE_LOAD_CURRENT_MIN ) mainControl.error |= SOC_AFE_ERR_LOAD_CURRENT_UNDER;
	if( loadCurrent > SOC_AFE_LOAD_CURRENT_MAX ) mainControl.error |= SOC_AFE_ERR_LOAD_CURRENT_OVER;

	if( mainControl.error != 0 ){
		mainControl.enable = 0;
		mainInputRelayDisable();
		mainOutputRelayDisable();
		AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 0, 1U);

	}
	else{
		if( mainControl.precharge == 1 ){
			mainInputRelayEnable();
			mainOutputRelayEnable();
		}
	}

	if( (mainControl.enable == 1) && (mainControl.error == 0) ){

		e = v_dc_ref - v_dc;

		u_pi = a1 * u_pi_1 + b0 * e + b1 * e_1;
		if(u_pi > 30.0) u_pi = 30.0;
		else if ( u_pi < (-30.0) ) u_pi = -30.0;

		ig_ref = u_pi * (v_ac / v_ac_peak);

		ei = ig_ref - i_ac;

		//u_pr = (-b1_pr * u_pr_1 - b2_pr * u_pr_2 + Kp_pr * b0_pr * ei + (Kp_pr*b1_pr+a1_pr)*ei_1 + (Kp_pr*b2_pr+a2_pr)*ei_2) / b0_pr;
		u_pr = (-b1_pr * u_pr_1 - b2_pr * u_pr_2 + (Kp_pr*b0_pr+a0_pr) * ei + (Kp_pr*b1_pr)*ei_1 + (Kp_pr*b2_pr+a2_pr)*ei_2) / b0_pr;
		//u_pr = b0_pr*ei + b1_pr*ei_1 + b2_pr*ei_2 - a1_pr*u_pr_1 - a2_pr*u_pr_2 + Kp_pr*ts*ei;

		vs_ref = v_ac - u_pr;

		vs_ref_norm = vs_ref / v_ac_peak;
		if(vs_ref_norm >  1.0) vs_ref_norm = 1.0;
		if(vs_ref_norm < -1.0) vs_ref_norm = -1.0;
		duty_float = (vs_ref_norm + 1.0) / 2.0 * 10000.0;
		if(duty_float < 0.0 ) duty_float = 0.0;
		duty = (uint32_t)duty_float;
		duty = 10000 - duty;
		AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 8, (uint32_t)(duty));

		u_pi_1 = u_pi;
		e_1 = e;

		ei_2 = ei_1;
		ei_1 = ei;

		u_pr_2 = u_pr_1;
		u_pr_1 = u_pr;

	}

	/* Saves data to memory */

	if( mainControl.trace.p < mainControl.trace.end ){
		*mainControl.trace.p++ = *((uint32_t *)(&hbCurrent));
		*mainControl.trace.p++ = *((uint32_t *)(&dcLinkVoltage));
		*mainControl.trace.p++ = *((uint32_t *)(&gridVoltage));
		*mainControl.trace.p++ = *((uint32_t *)(&loadCurrent));
		*mainControl.trace.p++ = *((uint32_t *)(&vs_ref_norm));
		*mainControl.trace.p++ = *((uint32_t *)(&u_pi));
		*mainControl.trace.p++ = *((uint32_t *)(&u_pr));
		*mainControl.trace.p++ = 0;
		*mainControl.trace.p++ = *((uint32_t *)(&dclinkVoltageInst));
		*mainControl.trace.p++ = *((uint32_t *)(&e));
		*mainControl.trace.p++ = *((uint32_t *)(&ei));
		*mainControl.trace.p++ = *((uint32_t *)(&ig_ref));
	}

	XGpio_DiscreteWrite(&gpioDebug_device, GPIODEBUG_CHANNEL, 0);


	//mainOutputRelayEnable();

	//XScuGic_SoftwareIntr ( &IntcInstancePtr , SOC_SIG_CPU1_TO_CPU0 , SOC_SIG_CPU0_ID ) ;
}
//-----------------------------------------------------------------------------
//=============================================================================
