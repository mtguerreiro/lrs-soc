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
/*
 * Priority and stack size of this task. The stack size is given in number of
 * ints, not bytes. So, 1024 means 1024 * 4 bytes if sizeof(int) is 4.
 */
#define UIFACE_CONFIG_TASK_PRIO			2
#define UIFACE_CONFIG_TASK_STACK_SIZE	1024

/*
 * Signature of the functions binded to commands on the user interface.
 * Whenever a new command is received, the user interface task will call the
 * function binded to the received ID, passing the received command, the
 * buffer containing the data received and the number of data bytes received.
 *
 * The binded function should return an signed integer value. If there was
 * no error with the received command, the binded function should return 0
 * or a positive value. If the function wants to send data back, the positive
 * value returned should be the number of bytes to send back, and the
 * parameter pbuf should be modified to reflect the location of the data to
 * be sent back. If there was an error executing the command, the binded
 * function should return a negative value, representing the error. This
 * negative value will be propagated forward.
 */
typedef int32_t(*uifaceHandle_t)(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

/*
 * Size of the buffer used to receive data from the network. This size is in
 * bytes. Bear in mind that one such buffer is allocated for each connection.
 */
#define UIFACE_CONFIG_RECV_BUFFER		128

/*
 * Defines how long a new request should wait until the last one is processed.
 * If the time expires, the new request will be denied, i.e., the connection
 * will be closed upon time expiration.
 */
#define UIFACE_CONFIG_MUTEX_WAIT_MS		2000

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
 * @brief Binds a command to a handle.
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
