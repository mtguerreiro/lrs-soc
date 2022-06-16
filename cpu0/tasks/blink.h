/*
 * blink.h
 *
 *  Created on: 23.05.2022
 *      Author: Marco Guerreiro
 */

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
#define BLINK_CONFIG_TASK_PRIO				2
#define BLINK_CONFIG_TASK_STACK_SIZE		1024

#define BLINK_CONFIG_DEFAULT_PERIOD_MS		1000
//=============================================================================

//=============================================================================
/*---------------------------------- Task -----------------------------------*/
//=============================================================================
void blink(void *param);
//=============================================================================

#endif /* TASKS_BLINK_H_ */
