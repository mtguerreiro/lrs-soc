/*
 * ipcomm.h
 *
 *  Created on: 23.05.2022
 *      Author: Marco Guerreiro
 */

#ifndef TASKS_IPCOMM_H_
#define TASKS_IPCOMM_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include <stdint.h>
//=============================================================================

//=============================================================================
/*--------------------------------- Defines ---------------------------------*/
//=============================================================================
/* Priority and stack size of this task */
#define IPCOMM_CONFIG_TASK_PRIO				2
#define IPCOMM_CONFIG_TASK_STACK_SIZE		1024
//=============================================================================

//=============================================================================
/*---------------------------------- Task -----------------------------------*/
//=============================================================================
void ipcomm(void *intcInstance);
//=============================================================================

#endif /* TASKS_IPCOMM_H_ */
