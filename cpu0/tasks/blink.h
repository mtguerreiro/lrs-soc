#ifndef TASKS_BLINK_H_
#define TASKS_BLINK_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include <stdint.h>


//=============================================================================

//=============================================================================
/*--------------------------------- Defines ---------------------------------*/
//=============================================================================
//#define BLINK_CONFIG_TASK_STACK_SIZE    256
//#define BLINK_CONFIG_TASK_PRIO          5
//
//
//#define BLINK_CONFIG_LED                DEVICE_GPIO_PIN_LED1
//
//#define BLINK_CMD_UPDATE_PERIOD         0x01

#define BLINK_CONFIG_TASK_PRIO				2
#define BLINK_CONFIG_TASK_STACK_SIZE		1024
//=============================================================================

//=============================================================================
/*---------------------------------- Task -----------------------------------*/
//=============================================================================
void blink(void *param);
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
//uint32_t blinkPeriodUpdate(commDataExchange_t *data);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* TASKS_BLINK_H_ */
