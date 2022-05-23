/*
 * blink.c
 *
 *  Created on: 23.05.2022
 *      Author: Marco Guerreiro
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "blink.h"

/* Kernel */
#include "FreeRTOS.h"
#include "task.h"

/* Device and drivers */
#include "xparameters.h"
#include "xgpio.h"
#include "xil_types.h"

/* Libs */
#include "soc_defs.h"

/* Tasks */
#include "uiface.h"
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
uint32_t blinkPeriod;


#define LED_ID XPAR_AXI_GPIO_RGB_LED_DEVICE_ID
#define LED_CHANNEL 1
#define LED_MASK 0b111

//#define BLINK_VAL  (*((volatile unsigned long *)(0xFFFF0000)+16))

//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static void blinkInitialize(XGpio_Config *cfg, XGpio *led);
static uint32_t blinkPeriodUpdate(uifaceDataExchange_t *data);
//static uint32_t blinkPeriodUpdateCPU1(uifaceDataExchange_t *data);
//=============================================================================

//=============================================================================
/*---------------------------------- Task -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void blink(void *param){

	uint32_t data;
	uint32_t prevstate;

	XGpio_Config *cfg_ptr = 0;
	XGpio led_device;

    blinkInitialize(cfg_ptr, &led_device);

    blinkPeriod = 1000 / portTICK_PERIOD_MS;

    uifaceRegisterHandle(SOC_CMD_CPU0_BLINK, blinkPeriodUpdate);
//    uifaceRegisterHandle(SOC_CMD_CPU0_CPU1_BLINK, blinkPeriodUpdateCPU1);

    while(1){

    	prevstate = XGpio_DiscreteRead(&led_device, LED_CHANNEL);
    	prevstate &= ~0b111;

    	XGpio_DiscreteWrite(&led_device, LED_CHANNEL, (data & LED_MASK) | prevstate);

    	data = data << 1;
    	if( (data & (LED_MASK << 2 ) ) == 0 ) data = 0xFF;

        vTaskDelay(blinkPeriod);
    }
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static void blinkInitialize(XGpio_Config *cfg, XGpio *led){

//	XGpio_Config *cfg_ptr;
//	XGpio led_device;

	/* Initializes PYNQ's (RGB) LEDs */
	cfg = XGpio_LookupConfig(LED_ID);
	XGpio_CfgInitialize(led, cfg, cfg->BaseAddress);
	XGpio_SetDataDirection(led, LED_CHANNEL, 0);
}
//-----------------------------------------------------------------------------
static uint32_t blinkPeriodUpdate(uifaceDataExchange_t *data){

	blinkPeriod = (data->buffer[0] << 8) | data->buffer[1];

	blinkPeriod = blinkPeriod / portTICK_PERIOD_MS;

    return 0;
}
//-----------------------------------------------------------------------------
//static uint32_t blinkPeriodUpdateCPU1(uifaceDataExchange_t *data){
//
//	uint32_t period;
//
//	period = (data->buffer[0] << 8) | data->buffer[1];
//
//	BLINK_VAL = period;
//
//    return 0;
//}
//-----------------------------------------------------------------------------
//=============================================================================
