/*
 * ipcClient.c
 *
 *  Created on: 09.12.2022
 *      Author: mguerreiro
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "ipcClient.h"

//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static int32_t ipcClientMemRead(void *src, void *dst, int32_t size);
static int32_t ipcClientMemWrite(void *src, void *dst, int32_t size);
static int32_t ipcClientRequestSend(void *req, int32_t reqsize);
static int32_t ipcClientRequestResponse(void *resp, int32_t maxrespsize,
		uint32_t timeout);
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{

	ipcClientIrqSend irqSend;
	ipcClientIrqReceive irqReceive;

	uint32_t serverAdd;
	int32_t serverSize;

	uint32_t clientAdd;
	int32_t clientSize;
}ipcClientCtl_t;
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
ipcClientCtl_t ipcClientCtl = {.serverAdd = 0, .serverSize = 0,
		.clientAdd = 0, .clientSize = 0};
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void ipcClientInitialize(ipcClientIrqSend irqSend, ipcClientIrqReceive irqReceive,
		   	   	   	     uint32_t clientMemAdd, int32_t clientMemSize,
		   	   	   	     uint32_t serverMemAdd, int32_t serverMemSize){

	ipcClientCtl.irqSend = irqSend;
	ipcClientCtl.irqReceive = irqReceive;

	ipcClientCtl.clientAdd = clientMemAdd;
	ipcClientCtl.clientSize = clientMemSize - 4U;

	ipcClientCtl.serverAdd = serverMemAdd;
	ipcClientCtl.serverSize = serverMemSize - 4U;
}
//-----------------------------------------------------------------------------
int32_t ipcClientRequest(void *req, int32_t reqsize,
		void *resp, int32_t maxrespsize,
		uint32_t timeout){

	int32_t status;
	int32_t size;

	status = ipcClientRequestSend(req, reqsize);
	if( status != 0 ) return status;

	size = ipcClientRequestResponse(resp, maxrespsize, timeout);

	return size;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t ipcClientMemRead(void *src, void *dst, int32_t size){

	uint8_t *s = (uint8_t *)src;
	uint8_t *d = (uint8_t *)dst;

	while(size--){
		*d++ = *s++;
	}

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ipcClientMemWrite(void *src, void *dst, int32_t size){

	uint8_t *s = (uint8_t *)src;
	uint8_t *d = (uint8_t *)dst;

	while(size--){
		*d++ = *s++;
	}

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ipcClientRequestSend(void *req, int32_t size){

	int32_t ret;

	if( size > ipcClientCtl.serverSize ) return IPC_CLIENT_ERR_SV_REQ_SIZE;

	ret = ipcClientMemWrite((void *)( &size ), (void *)( ipcClientCtl.serverAdd ), 4);
	if( ret != 0 ) return IPC_CLIENT_ERR_MEM_WRITE;

	ret = ipcClientMemWrite(req, (void *)( ipcClientCtl.serverAdd + 4 ), size);
	if( ret != 0 ) return IPC_CLIENT_ERR_MEM_WRITE;

	ret = ipcClientCtl.irqSend();
	if( ret != 0 ) return IPC_CLIENT_ERR_IRQ_SEND;

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ipcClientRequestResponse(void *resp, int32_t maxrespsize,
		uint32_t timeout){

	int32_t ret;
	int32_t respsize;

	ret = ipcClientCtl.irqReceive(timeout);
	if( ret != 0 ) return IPC_CLIENT_ERR_IRQ_RECEIVE;

	ret = ipcClientMemRead((void *)( ipcClientCtl.clientAdd ), (void *)( &respsize ), 4);
	if( ret != 0 ) return IPC_CLIENT_ERR_MEM_WRITE;

	if( respsize > maxrespsize ) return IPC_CLIENT_ERR_SV_RESP_SIZE;

	ret = ipcClientMemWrite((void *)( ipcClientCtl.clientAdd + 4 ), resp, respsize);
	if( ret != 0 ) return IPC_CLIENT_ERR_MEM_WRITE;

	return respsize;
}
//-----------------------------------------------------------------------------
//=============================================================================
