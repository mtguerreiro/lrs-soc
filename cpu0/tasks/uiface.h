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

/* LRS SoC defs */
#include "soc_defs.h"
//=============================================================================

//=============================================================================
/*--------------------------------- Defines ---------------------------------*/
//=============================================================================
/* Priority and stack size of this task */
#define UIFACE_CONFIG_TASK_PRIO			2
#define UIFACE_CONFIG_TASK_STACK_SIZE	1024

/*
 * This structure holds data received by the user interface. Whenever a
 * command is received, the data is saved on the buffer, and the corresponding
 * handle is called.
 */
typedef struct{
	/* Command received */
	uint32_t cmd;

	/* Number of bytes received */
    uint32_t size;

    /* Buffer holding data received */
    uint8_t *buffer;

}uifaceDataExchange_t;

/*
 * Function executed when data is received. The command, the buffer holding
 * the data and the number of bytes received are passed to the function on
 * the data structure. Additionally, should any message be sent back, this
 * function should return 1. The number of bytes should be written on the
 * structure, as well as the pointer to the buffer holding data to be sent
 * back. If there is no reply, this function must return 0.
 */
typedef uint32_t(*uifaceHandle_t)(uifaceDataExchange_t *data);

/*
 * Defines if DHCP should be used. For now, we've only tested with
 * DHCP enabled.
 */
#define UIFACE_CONFIG_USE_DHCP			SOC_UIFACE_USE_DHCP

/*
 * Size of the buffer used to receive data from the network. Bear in mind that
 * one such buffer is allocated for each connection.
 */
#define UIFACE_CONFIG_RECV_BUFFER		2048

/*
 * Defines how long a new request should wait until the last one is processed.
 * If the time expires, the new request will be denied, i.e., the connection
 * will be closed upon time expiration.
 */
#define UIFACE_CONFIG_MUTEX_WAIT_MS		10000
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
/**
 * @brief Binds an command to a handle.
 *
 * If a command was previously registered and this function is called with the
 * the same command again, the previous handle will be overwritten.
 *
 * @param id ID of the command. Should be a value between 0 and
 * 		  SOC_CMD_CPU0_END.
 * @param handle Function to be called when ID is received.
 *
 * @return 0 if handle was registered, -1 otherwise.
 */
int32_t uifaceRegisterHandle(uint32_t id, uifaceHandle_t handle);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* TASKS_uiface_H_ */
