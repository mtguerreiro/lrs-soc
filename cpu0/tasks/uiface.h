/*
 * uiface.h
 *
 *  Created on: 23.05.2022
 *      Author: Marco Guerreiro
 */

#ifndef TASKS_UIFACE_H_
#define TASKS_UIFACE_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include <stdint.h>

//=============================================================================

//=============================================================================
/*--------------------------------- Defines ---------------------------------*/
//=============================================================================
typedef struct{
	uint32_t cmd;
    uint32_t size;
    uint8_t *buffer;
}uifaceDataExchange_t;

/*
 * Function executed when data is received. The buffer holding the data
 * received and the number of bytes received are passed to the function on
 * the data structure. Additionally, should any message be sent back, this
 * function should return 1, and write the number of bytes and the buffer
 * on the data structure. If there is no reply, this function must return 0.
 */
typedef uint32_t(*uifaceHandle_t)(uifaceDataExchange_t *data);

#define UIFACE_CONFIG_TASK_PRIO			2
#define UIFACE_CONFIG_TASK_STACK_SIZE	1024
//=============================================================================

//=============================================================================
/*---------------------------------- Task -----------------------------------*/
//=============================================================================
void uiface(void *param);
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t uifaceRegisterHandle(uint32_t id, uifaceHandle_t handle);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* TASKS_uiface_H_ */
