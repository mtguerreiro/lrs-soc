/*
 * ipcServer.h
 *
 *  Created on: 09.12.2022
 *      Author: mguerreiro
 */

#ifndef IPCSERVER_H_
#define IPCSERVER_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"

//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef int32_t (*ipcServerIrqSend)(void);
typedef int32_t (*ipcServerRequestHandle)(void *req, int32_t reqsize, void **resp, int32_t maxrespsize);

#define IPC_SERVER_ERR_MEM_WRITE		-1
#define IPC_SERVER_ERR_MEM_READ			-2
#define IPC_SERVER_ERR_IRQ_SEND			-3
#define IPC_SERVER_ERR_IRQ_RECEIVE		-4
#define IPC_SERVER_ERR_SV_RESP_SIZE		-5
#define IPC_SERVER_ERR_SV_REQ_SIZE		-6
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void ipcServerInitialize(ipcServerRequestHandle reqHandle,
		ipcServerIrqSend irqSend,
		uint32_t serverMemAdd, int32_t serverMemSize,
		uint32_t clientMemAdd, int32_t clientMemSize);
//-----------------------------------------------------------------------------
int32_t ipcServerRequest(void);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* IPCSERVER_H_ */
