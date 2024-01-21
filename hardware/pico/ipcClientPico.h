/*
 * @file ipcClientPico.h
 *
 * @brief Client for the inter-processor communication (IPC) library.
 *
 */

#ifndef IPC_CLIENT_PICO_H_
#define IPC_CLIENT_PICO_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
#include "stddef.h"

//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
/* CPU1->CPU0 timeout error code */
#define IPC_CLIENT_PICO_ERR_CPU1_REPLY_TO				-1

/* Invalid CPU1 command */
#define IPC_CLIENT_PICO_ERR_CPU1_INVALID_CMD			-2

/* Data received exceeds CPU0->CPU1 buffer */
#define IPC_CLIENT_PICO_ERR_CPU0_CPU1_BUFFER_OVERFLOW	-3

/* Lock error code */
#define IPC_CLIENT_PICO_ERR_LOCK_TO                     -4
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void ipcClientPicoInitialize(void);
//-----------------------------------------------------------------------------
int32_t ipcClientPicoIrqSend(void);
//-----------------------------------------------------------------------------
int32_t ipcClientPicoIrqReceive(uint32_t timeout);
//-----------------------------------------------------------------------------
int32_t ipcClientPicoLock(uint32_t timeout);
//-----------------------------------------------------------------------------
void ipcClientPicoUnlock(void);
//-----------------------------------------------------------------------------
//=============================================================================
#endif /* IPC_CLIENT_PICO_H_ */
