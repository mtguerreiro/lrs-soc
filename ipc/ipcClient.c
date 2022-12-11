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
static int32_t ipcClientMemRead(uint32_t *src, uint32_t *dst, int32_t size);
static int32_t ipcClientMemWrite(uint32_t *src, uint32_t *dst, int32_t size);
static int32_t ipcClientRequestSend(uint32_t *req, int32_t reqsize);
static int32_t ipcClientRequestResponse(uint32_t *resp, int32_t maxrespsize,
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
	ipcClientCtl.clientSize = clientMemSize - 1U;

	ipcClientCtl.serverAdd = serverMemAdd;
	ipcClientCtl.serverSize = serverMemSize - 1U;
}
//-----------------------------------------------------------------------------
int32_t ipcClientRequest(uint32_t *req, int32_t reqsize,
		uint32_t *resp, int32_t maxrespsize,
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
static int32_t ipcClientMemRead(uint32_t *src, uint32_t *dst, int32_t size){

	while(size--){
		*dst++ = *src++;
	}

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ipcClientMemWrite(uint32_t *src, uint32_t *dst, int32_t size){

	while(size--){
		*dst++ = *src++;
	}

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ipcClientRequestSend(uint32_t *req, int32_t reqsize){

	int32_t status;
	uint32_t *psv;

	if( reqsize > ipcClientCtl.serverSize ) return IPC_CLIENT_ERR_SV_REQ_SIZE;

	psv = (uint32_t *)ipcClientCtl.serverAdd;

	status = ipcClientMemWrite((uint32_t *)&reqsize, psv, 1);
	if( status != 0 ) return IPC_CLIENT_ERR_MEM_WRITE;

	psv++;
	status = ipcClientMemWrite(req, psv, reqsize);
	if( status != 0 ) return IPC_CLIENT_ERR_MEM_WRITE;

	status = ipcClientCtl.irqSend();
	if( status != 0 ) return IPC_CLIENT_ERR_IRQ_SEND;

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ipcClientRequestResponse(uint32_t *resp, int32_t maxrespsize,
		uint32_t timeout){

	int32_t status;
	int32_t size;
	uint32_t *pcl;

	status = ipcClientCtl.irqReceive(timeout);
	if( status != 0 ) return IPC_CLIENT_ERR_IRQ_RECEIVE;

	pcl = (uint32_t *)ipcClientCtl.clientAdd;

	status = ipcClientMemRead(pcl, (uint32_t *)&size, 1);
	if( status != 0 ) return IPC_CLIENT_ERR_MEM_WRITE;

	if( size > maxrespsize ) return IPC_CLIENT_ERR_SV_RESP_SIZE;

	pcl++;
	status = ipcClientMemWrite(pcl, resp, size);
	if( status != 0 ) return IPC_CLIENT_ERR_MEM_WRITE;

	return size;
}
//-----------------------------------------------------------------------------
//=============================================================================
