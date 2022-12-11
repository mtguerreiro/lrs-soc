/*
 * ipcClient.h
 *
 *  Created on: 09.12.2022
 *      Author: mguerreiro
 */

#ifndef IPCCLIENT_H_
#define IPCCLIENT_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"

//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef int32_t (*ipcClientIrqSend)(void);
typedef int32_t (*ipcClientIrqReceive)(uint32_t timeout);

#define IPC_CLIENT_ERR_MEM_WRITE		-1
#define IPC_CLIENT_ERR_MEM_READ			-2
#define IPC_CLIENT_ERR_IRQ_SEND			-3
#define IPC_CLIENT_ERR_IRQ_RECEIVE		-4
#define IPC_CLIENT_ERR_SV_RESP_SIZE		-5
#define IPC_CLIENT_ERR_SV_REQ_SIZE		-6
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void ipcClientInitialize(ipcClientIrqSend irqSend, ipcClientIrqReceive irqReceive,
		   	   	   	     uint32_t clientMemAdd, int32_t clientMemSize,
		   	   	   	     uint32_t serverMemAdd, int32_t serverMemSize);
//-----------------------------------------------------------------------------
int32_t ipcClientRequest(uint32_t *req, int32_t reqsize,
		uint32_t *resp, int32_t maxrespsize,
		uint32_t timeout);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* IPCCLIENT_H_ */
