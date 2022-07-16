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

/* Tasks */
#include "uiface.h"

/* LRS SoC defs */
#include "soc_defs.h"
//=============================================================================

//=============================================================================
/*--------------------------------- Defines ---------------------------------*/
//=============================================================================
#define BLINK_XIL_LED_ID 		XPAR_AXI_GPIO_RGB_LED_DEVICE_ID
#define BLINK_XIL_LED_CHANNEL 	1
#define BLINK_XIL_LED_MASK 		0b111

#define BLINK_LED_BLUE		(1 << 0)
#define BLINK_LED_GREEN		(1 << 1)
#define BLINK_LED_RED		(1 << 2)

typedef struct{

	/* RGB LED device */
	XGpio rgbled;

	/* Blink period */
	uint32_t period;

} blinkControl_t;
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
/* Task control structure */
blinkControl_t xblinkControl;
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static void blinkInitialize(void);
static int32_t blinkPeriodUpdate(uint32_t cmd, uint8_t **pbuf, uint32_t size);
static void blinkToggleColor(uint32_t color);
//=============================================================================

//=============================================================================
/*---------------------------------- Task -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void blink(void *param){

    blinkInitialize();

    while(1){
    	blinkToggleColor(BLINK_LED_GREEN);
        vTaskDelay(xblinkControl.period);
    }
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static void blinkInitialize(void){

	XGpio_Config *cfg;

	/* Initializes PYNQ's (RGB) LEDs */
	cfg = XGpio_LookupConfig(BLINK_XIL_LED_ID);
	XGpio_CfgInitialize(&xblinkControl.rgbled, cfg, cfg->BaseAddress);
	XGpio_SetDataDirection(&xblinkControl.rgbled, BLINK_XIL_LED_CHANNEL, 0);

	/* Sets default blinking period */
	xblinkControl.period = BLINK_CONFIG_DEFAULT_PERIOD_MS / portTICK_PERIOD_MS;

	/* Registers period update function with blink update command */
    uifaceRegisterHandle(SOC_CMD_CPU0_BLINK_CPU0, blinkPeriodUpdate);
}
//-----------------------------------------------------------------------------
static int32_t blinkPeriodUpdate(uint32_t cmd, uint8_t **pbuf, uint32_t size){

	uint32_t period;

	period = *((uint32_t *)(*pbuf));

	xblinkControl.period = period / portTICK_PERIOD_MS;

    return 0;
}
//-----------------------------------------------------------------------------
static void blinkToggleColor(uint32_t color){

	uint32_t prev, new;

	prev = XGpio_DiscreteRead(&xblinkControl.rgbled, BLINK_XIL_LED_CHANNEL);

	new = (color & BLINK_XIL_LED_MASK) ^ prev;

	XGpio_DiscreteWrite(&xblinkControl.rgbled,
						BLINK_XIL_LED_CHANNEL,
						new);
}
//-----------------------------------------------------------------------------
//=============================================================================
