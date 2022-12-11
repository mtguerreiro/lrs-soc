/*
 * ipcServer.c
 *
 *  Created on: 09.12.2022
 *      Author: mguerreiro
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "ipcServer.h"

//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static int32_t ipcServerMemRead(uint32_t *src, uint32_t *dst, int32_t size);
static int32_t ipcServerMemWrite(uint32_t *src, uint32_t *dst, int32_t size);
//static int32_t ipcServerRequestGet(uint32_t *req);
//static int32_t ipcServerRequestResponse(uint32_t *resp, int32_t respsize);
//static int32_t ipcServerRequestResponse(uint32_t *resp, uint32_t maxrespsize,
//		uint32_t timeout);
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{

	ipcServerIrqSend irqSend;

	uint32_t serverAdd;
	int32_t serverSize;

	uint32_t clientAdd;
	int32_t clientSize;

	ipcServerRequestHandle reqHandle;
}ipcServerCtl_t;
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
ipcServerCtl_t ipcServerCtl = {.serverAdd = 0, .serverSize = 0,
		.clientAdd = 0, .clientSize = 0};
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void ipcServerInitialize(ipcServerRequestHandle reqHandle,
		ipcServerIrqSend irqSend,
		uint32_t serverMemAdd, int32_t serverMemSize,
		uint32_t clientMemAdd, int32_t clientMemSize){

	ipcServerCtl.reqHandle = reqHandle;

	ipcServerCtl.irqSend = irqSend;

	ipcServerCtl.clientAdd = clientMemAdd;
	ipcServerCtl.clientSize = clientMemSize - 1U;

	ipcServerCtl.serverAdd = serverMemAdd;
	ipcServerCtl.serverSize = serverMemSize - 1U;
}
//-----------------------------------------------------------------------------
int32_t ipcServerRequest(void){

	int32_t status;

	int32_t respSize;
	uint32_t *resp;

	int32_t reqSize;
	uint32_t *req;

	req = (uint32_t *)ipcServerCtl.serverAdd;
	status = ipcServerMemRead(req, (uint32_t *)&reqSize, 1);
	if( status != 0 ) return IPC_SERVER_ERR_MEM_WRITE;
	req++;

	resp = (uint32_t *)ipcServerCtl.clientAdd;
	resp++;

	status = ipcServerCtl.reqHandle(req, reqSize, resp, ipcServerCtl.clientSize);
	if( status < 0 ) return status;
	respSize = status;

	resp = (uint32_t *)ipcServerCtl.clientAdd;

	status = ipcServerMemWrite((uint32_t *)&respSize, resp, 1);
	if( status != 0 ) return IPC_SERVER_ERR_MEM_WRITE;

	status = ipcServerCtl.irqSend();
	if( status != 0 ) return IPC_SERVER_ERR_IRQ_SEND;

	return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t ipcServerMemRead(uint32_t *src, uint32_t *dst, int32_t size){

	while(size--){
		*dst++ = *src++;
	}

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ipcServerMemWrite(uint32_t *src, uint32_t *dst, int32_t size){

	while(size--){
		*dst++ = *src++;
	}

	return 0;
}
//-----------------------------------------------------------------------------
//static int32_t ipcServerRequestGet(uint32_t *req){
//
//	int32_t size;
//	uint32_t *psv;
//	int32_t status;
//
//	psv = (uint32_t *)ipcServerCtl.serverAdd;
//
//	status = ipcServerMemRead(psv, &size, 1);
//	if( status != 0 ) return IPC_SERVER_ERR_MEM_WRITE;
//
//	psv++;
//
//	*req = (uint32_t)psv;
//
//	return size;
//}
//-----------------------------------------------------------------------------
//static int32_t ipcServerRequestResponse(uint32_t *resp, int32_t respsize){
//
//	int32_t status;
//	uint32_t *pcl;
//
//	if( respsize > ipcClientCtl.serverSize ) return IPC_SERVER_ERR_SV_RESP_SIZE;
//
//	pcl = (uint32_t *)ipcServerCtl.clientAdd;
//
//	status = ipcServerMemWrite(&respsize, pcl, 1);
//	if( status != 0 ) return IPC_SERVER_ERR_MEM_WRITE;
//
//	pcl++;
//	status = ipcServerMemWrite(resp, pcl, respsize);
//	if( status != 0 ) return IPC_SERVER_ERR_MEM_WRITE;
//
//	status = ipcServerCtl.irqSend();
//	if( status != 0 ) return IPC_SERVER_ERR_IRQ_SEND;
//
//	return 0;
//}
//-----------------------------------------------------------------------------
//=============================================================================
