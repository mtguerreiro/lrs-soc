/*
 * @file afeHwZynq.c
 *
 */

#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "afeHwZynq.h"

//#include "soc_defs.h"
#include "zynqConfig.h"

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

#include "axi_test.h"

#include "ocpConfig.h"
#include "ocpTrace.h"
#include "ocpCS.h"
#include "ocpIf.h"

#include "ipcServer.h"
#include "ipcServerZynq.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

#define AXI_TEST_BASE_ADR					XPAR_ADC_PSCTL_0_S00_AXI_BASEADDR
#define AXI_PWM_BASE_ADR					XPAR_AXI_PWM_0_S00_AXI_BASEADDR
#define AXI_GPIO_DEBUG_BASE_ADR				XPAR_AXI_GPIO_DEBUG_BASEADDR

//#define LED_ID XPAR_AXI_GPIO_RGB_LED_DEVICE_ID
//#define LED_CHANNEL 1
//#define LED_MASK 0b111

#define RELAY_ID 			XPAR_AXI_GPIO_RGB_LED_DEVICE_ID
#define RELAY_CHANNEL 		1

#define GPIODEBUG_ID 		XPAR_AXI_GPIO_DEBUG_DEVICE_ID
#define GPIODEBUG_CHANNEL 	1

/*
 * AFE measurements.
 *
 * +-----+--------------+--------+
 * | ADC |  Measurement | Offset |
 * +-----+--------------+--------+
 * |  1  |  HB current  |    0   |
 * +-----+--------------+--------+
 * |  2  |    DC link   |    2   |
 * +-----+--------------+--------+
 * |  3  | Grid voltage |    4   |
 * +-----+--------------+--------+
 * |  4  | Load current |    6   |
 * +-----+--------------+--------+
 *
 */

#define AFE_HW_ZYNQ_ADC_TO_SIGNAL(ADC, GAIN, OFFS) ((float)ADC) * ((float)GAIN) + ((float)OFFS)

#define AFE_HW_ZYNQ_HB_CURRENT					ZYNQ_CONFIG_MEM_PL_TO_CPU1_ADR
#define AFE_HW_ZYNQ_DCLINK						ZYNQ_CONFIG_MEM_PL_TO_CPU1_ADR + 4
#define AFE_HW_ZYNQ_GRID_VOLTAGE				ZYNQ_CONFIG_MEM_PL_TO_CPU1_ADR + 8
#define AFE_HW_ZYNQ_LOAD_CURRENT				ZYNQ_CONFIG_MEM_PL_TO_CPU1_ADR + 12

#define AFE_HW_ZYNQ_HB_CURRENT_SENS_GAIN		(5.0 / 4095.0 / 0.185)
#define AFE_HW_ZYNQ_HB_CURRENT_SENS_OFFS		(-2.5 / 0.185)

#define AFE_HW_ZYNQ_DCLINK_SENS_GAIN			(5.0 / 4095.0 * 900.0 / 23.2)
#define AFE_HW_ZYNQ_DCLINK_SENS_OFFS			(0.0)

#define AFE_HW_ZYNQ_GRID_VOLTAGE_SENS_GAIN		(5.0 / 4095.0 * 2.2 / 150.0 * 1.0 / 2.5 * 5000.0)
#define AFE_HW_ZYNQ_GRID_VOLTAGE_SENS_OFFS		(-2.5 * 2.2 / 150.0 * 1.0 / 2.5 * 5000.0)

#define AFE_HW_ZYNQ_LOAD_CURRENT_SENS_GAIN		(5.0 / 4095.0 * 2.2 / 150.0 * 1000.0)
#define AFE_HW_ZYNQ_LOAD_CURRENT_SENS_OFFS		(-2.5 * 2.2 / 150.0 * 1000.0)

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================


//XGpio led_device;
XGpio relay_device;
XGpio gpioDebug_device;
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t afeHwZynqInitializeHw(void *intcInst);
//-----------------------------------------------------------------------------
static int32_t afeHwZynqInitializeHwGpios(void);
//-----------------------------------------------------------------------------
static int32_t afeHwZynqInitializeHwAdc(void);
//-----------------------------------------------------------------------------
static int32_t afeHwZynqInitializeHwPlIrq(void *intcInst);
//-----------------------------------------------------------------------------
void afeHwZynqPlToCpuIrq(void *callbackRef);
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t afeHwZynqInitialize(void *intcInst){

	int32_t status;

	/* Initializes peripherals */
	status = afeHwZynqInitializeHw(intcInst);

	return status;
}
//-----------------------------------------------------------------------------
int32_t afeHwZynqAdcEnable(uint32_t enable){

	if( enable == 0 ) AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 0, 0U);
	else AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 0, 1U);

	return 0;
}
//-----------------------------------------------------------------------------
int32_t afeHwZynqAdcConfig(void *adcSpiFreq){

	uint32_t freq = *( (uint32_t *)adcSpiFreq );
	uint32_t en;

	en = AXI_TEST_mReadReg(AXI_PWM_BASE_ADR, 0);

	if( (en & 1) == 1 ) AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 0, 0U);

	AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 4, freq);

	if( (en & 1) == 1 ) AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 0, en);

	return 0;
}
//-----------------------------------------------------------------------------
int32_t afeHwZynqPwmEnable(uint32_t enable){

	if( enable == 0 ){
		AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 0, 1U);
	}
	else{
		AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 8, 0);
		AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 0, 3U);
	}

	return 0;
}
//-----------------------------------------------------------------------------
int32_t afeHwZynqPwmSetDuty(uint32_t duty){

	AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 8, (uint32_t)(duty));

	return 0;
}
//-----------------------------------------------------------------------------
int32_t afeHwZynqPwmConfig(void *pwmfreq){

	uint32_t freq = *( (uint32_t *)pwmfreq );
	uint32_t en;

	en = AXI_TEST_mReadReg(AXI_PWM_BASE_ADR, 0);

	if( (en & 1) == 1 ) AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 0, 0U);

	//AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 0, 1U);
	AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 4, freq);
	AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 8, 0);

	//AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 8, freq);

	if( (en & 1) == 1 ) AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 0, en);

	return 0;
}
//-----------------------------------------------------------------------------
void afeHwZynqInputRelaySet(uint32_t state){

	uint32_t data;

	data = XGpio_DiscreteRead(&relay_device, RELAY_CHANNEL);

	if( state == 0 ){
		XGpio_DiscreteWrite(&relay_device, RELAY_CHANNEL, data | 0x06);
	}
	else{
		XGpio_DiscreteWrite(&relay_device, RELAY_CHANNEL, data & 0x01);
	}
}
//-----------------------------------------------------------------------------
void afeHwZynqOutputRelaySet(uint32_t state){

	uint32_t data;

	data = XGpio_DiscreteRead(&relay_device, RELAY_CHANNEL);

	if( state == 0 ){
		XGpio_DiscreteWrite(&relay_device, RELAY_CHANNEL, data | 0x01);
	}
	else{
		XGpio_DiscreteWrite(&relay_device, RELAY_CHANNEL, data & 0x06);
	}
}
//-----------------------------------------------------------------------------
int32_t afeHwZynqGetInputs(void *inputs){

	float *p = (float *)inputs;

	static float dcLinkVoltage_1 = 0.0;
	static float hbCurrent_1 = 0.0;

	float hbCurrent;
	float dcLinkVoltage;
	float gridVoltage;
	float loadCurrent;
	float d_v, d_i;

	gridVoltage = AFE_HW_ZYNQ_ADC_TO_SIGNAL(*((uint16_t *)(AFE_HW_ZYNQ_GRID_VOLTAGE)), AFE_HW_ZYNQ_GRID_VOLTAGE_SENS_GAIN, AFE_HW_ZYNQ_GRID_VOLTAGE_SENS_OFFS);
	hbCurrent = AFE_HW_ZYNQ_ADC_TO_SIGNAL(*((uint16_t *)(AFE_HW_ZYNQ_HB_CURRENT)), AFE_HW_ZYNQ_HB_CURRENT_SENS_GAIN, AFE_HW_ZYNQ_HB_CURRENT_SENS_OFFS);
	dcLinkVoltage = AFE_HW_ZYNQ_ADC_TO_SIGNAL(*((uint16_t *)(AFE_HW_ZYNQ_DCLINK)), AFE_HW_ZYNQ_DCLINK_SENS_GAIN, AFE_HW_ZYNQ_DCLINK_SENS_OFFS);
	loadCurrent = AFE_HW_ZYNQ_ADC_TO_SIGNAL(*((uint16_t *)(AFE_HW_ZYNQ_LOAD_CURRENT)), AFE_HW_ZYNQ_LOAD_CURRENT_SENS_GAIN, AFE_HW_ZYNQ_LOAD_CURRENT_SENS_OFFS);

	d_v = dcLinkVoltage - dcLinkVoltage_1;
	if( (d_v > 6.0) || (d_v < (-6.0)) ) dcLinkVoltage = dcLinkVoltage_1;
	else dcLinkVoltage_1 = dcLinkVoltage;

	hbCurrent = AFE_HW_ZYNQ_ADC_TO_SIGNAL(*((uint16_t *)(AFE_HW_ZYNQ_HB_CURRENT)), AFE_HW_ZYNQ_HB_CURRENT_SENS_GAIN, AFE_HW_ZYNQ_HB_CURRENT_SENS_OFFS);
	d_i = hbCurrent - hbCurrent_1;
	if( (d_i > 5.0) || (d_i < (-5.0)) ) hbCurrent = hbCurrent_1;
	else hbCurrent_1 = hbCurrent;

	*p++ = hbCurrent;
	*p++ = gridVoltage;
	*p++ = dcLinkVoltage;
	*p = loadCurrent;

	return 4;
}
//-----------------------------------------------------------------------------
int32_t afeHwZynqProcInputs(void *inputs, void *procinputs, int32_t n){

	float *src = (float *)inputs;
	float *dst = (float *)procinputs;

	while(n--) *dst++ = *src++;

	return 4;
}
//-----------------------------------------------------------------------------
int32_t afeHwZynqProcOutputs(void *outputs, void *procoputs, int32_t n){

	float duty = *( (float *)outputs );
	uint32_t dutyint;

	duty = duty * 10000.0f;
	if(duty < 0.0 ) duty = 0.0;

	dutyint = (uint32_t)duty;
	dutyint = 10000 - dutyint;

	*((uint32_t *)procoputs) = dutyint;

	return 1;
}
//-----------------------------------------------------------------------------
int32_t afeHwZynqApplyOutputs(void *outputs, int32_t n){

	uint32_t duty = *( (uint32_t *)outputs );

	afeHwZynqPwmSetDuty(duty);

	return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t afeHwZynqInitializeHw(void *intcInst){

	afeHwZynqInitializeHwGpios();
	afeHwZynqInitializeHwAdc();
	afeHwZynqInitializeHwPlIrq(intcInst);

	//ocpTraceAddSignal(OCP_TRACE_1, (void *)&gridVoltage, "Grid voltage");

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t afeHwZynqInitializeHwGpios(void){

	XGpio_Config *cfg_ptr = 0;

	/* Initializes and turns relays off */
	cfg_ptr = XGpio_LookupConfig(XPAR_AXI_GPIO_RELAY_DEVICE_ID);
	XGpio_CfgInitialize(&relay_device, cfg_ptr, cfg_ptr->BaseAddress);
	XGpio_SetDataDirection(&relay_device, RELAY_CHANNEL, 0);

	cfg_ptr = XGpio_LookupConfig(XPAR_AXI_GPIO_DEBUG_DEVICE_ID);
	XGpio_CfgInitialize(&gpioDebug_device, cfg_ptr, cfg_ptr->BaseAddress);
	XGpio_SetDataDirection(&gpioDebug_device, GPIODEBUG_CHANNEL, 0);
	XGpio_DiscreteWrite(&gpioDebug_device, GPIODEBUG_CHANNEL, 0);

	afeHwZynqInputRelaySet(0);
	afeHwZynqOutputRelaySet(0);

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t afeHwZynqInitializeHwAdc(void){

	/* Sets ADC stuff */
//	AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 0, 0x00000000);
	//AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 4, 0x0000000A);
	//AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 8, 10000);
	AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 4, 500);
//	AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 8, 20000);
	AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 12, ZYNQ_CONFIG_MEM_PL_TO_CPU1_ADR);

	AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 0, 0U);

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t afeHwZynqInitializeHwPlIrq(void *intcInst){

	XScuGic_SetPriorityTriggerType(intcInst, ZYNQ_CONFIG_IRQ_PL_TO_CPU1, 0xA0, 0x3);
	XScuGic_Connect(intcInst, ZYNQ_CONFIG_IRQ_PL_TO_CPU1, (Xil_ExceptionHandler)afeHwZynqPlToCpuIrq, intcInst) ;
	XScuGic_Enable(intcInst, ZYNQ_CONFIG_IRQ_PL_TO_CPU1);

	return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------------- IRQ -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void afeHwZynqPlToCpuIrq(void *callbackRef){

	ocpCSRun(OCP_CS_1);

	ocpTraceSave(OCP_TRACE_1);
}
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* SOC_CPU1 */
