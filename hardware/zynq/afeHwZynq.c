/*
 * @file afeHwZynq.c
 *
 */

#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "afeHwZynq.h"

#include "soc_defs.h"

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

#include "ocpTrace.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

#define AFE_HW_ZYNQ_CONFIG_IRQ_PL_TO_CPU1	SOC_IRQ_PL_TO_CPU1

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

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static float gridVoltage = 0.0f;

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

	return afeHwZynqInitializeHw(intcInst);
}
//-----------------------------------------------------------------------------
int32_t afeHwZynqAdcEn(uint32_t enable){

	if( enable == 0 ) AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 0, 0U);
	else AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 0, 1U);

	return 0;
}
//-----------------------------------------------------------------------------
int32_t afeHwZynqSetAdc(void *adcSpiFreq){

	uint32_t freq = *( (uint32_t *)adcSpiFreq );
	uint32_t en;

	en = AXI_TEST_mReadReg(AXI_PWM_BASE_ADR, 0);

	if( (en & 1) == 1 ) AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 0, 0U);

	AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 4, freq);

	if( (en & 1) == 1 ) AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 0, en);

	return 0;
}
//-----------------------------------------------------------------------------
int32_t afeHwZynqSetPwm(void *pwmfreq){

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
void afeHwZynqSetInputRelay(uint32_t state){

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
void afeHwZynqSetOutputRelay(uint32_t state){

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
//=============================================================================

//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t afeHwZynqInitializeHw(void *intcInst){

	afeHwZynqInitializeHwGpios();
	afeHwZynqInitializeHwPlIrq(intcInst);

	ocpTraceAddSignal(OCP_TRACE_1, (void *)&gridVoltage, "Grid voltage");

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

	afeHwZynqSetInputRelay(0);
	afeHwZynqSetOutputRelay(0);

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
	AXI_TEST_mWriteReg(AXI_TEST_BASE_ADR, 12, SOC_MEM_PL_TO_CPU1_ADR);

	AXI_TEST_mWriteReg(AXI_PWM_BASE_ADR, 0, 0U);

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t afeHwZynqInitializeHwPlIrq(void *intcInst){

	XScuGic_SetPriorityTriggerType(intcInst, AFE_HW_ZYNQ_CONFIG_IRQ_PL_TO_CPU1, 0xA0, 0x3);
	XScuGic_Connect(intcInst, AFE_HW_ZYNQ_CONFIG_IRQ_PL_TO_CPU1, (Xil_ExceptionHandler)afeHwZynqPlToCpuIrq, intcInst) ;
	XScuGic_Enable(intcInst, AFE_HW_ZYNQ_CONFIG_IRQ_PL_TO_CPU1);

	return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------------- IRQ -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void afeHwZynqPlToCpuIrq(void *callbackRef){

	gridVoltage = SOC_ADC_TO_SIGNAL(*((uint16_t *)(SOC_AFE_GRID_VOLTAGE)), SOC_AFE_GRID_VOLTAGE_SENS_GAIN, SOC_AFE_GRID_VOLTAGE_SENS_OFFS);

	ocpTraceSave(OCP_TRACE_1);
}
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* SOC_CPU1 */
