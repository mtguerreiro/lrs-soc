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
static int32_t ipcServerMemRead(void *src, void *dst, int32_t size);
static int32_t ipcServerMemWrite(void *src, void *dst, int32_t size);
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
	ipcServerCtl.clientSize = clientMemSize - 4U;

	ipcServerCtl.serverAdd = serverMemAdd;
	ipcServerCtl.serverSize = serverMemSize - 4U;
}
//-----------------------------------------------------------------------------
int32_t ipcServerRequest(void){

	int32_t ret;
	int32_t reqsize;
	int32_t respsize;
	void *resp;

	resp = (void *)(ipcServerCtl.clientAdd + 4);

	ret = ipcServerMemRead((void *)( ipcServerCtl.serverAdd ), (void *)( &reqsize ), 4);
	if( ret != 0 ) return IPC_SERVER_ERR_MEM_WRITE;

	respsize = ipcServerCtl.reqHandle((void *)( ipcServerCtl.serverAdd + 4 ), reqsize,
			(void **)( &resp ), ipcServerCtl.clientSize);
	if( respsize < 0 ) return respsize;

	ret = ipcServerMemWrite((void *)( &respsize ), (void *)(ipcServerCtl.clientAdd), 4);
	if( ret != 0 ) return IPC_SERVER_ERR_MEM_WRITE;

	if( ((uint32_t)( resp )) != (ipcServerCtl.clientAdd + 4) ){
		ret = ipcServerMemWrite( resp, (void *)(ipcServerCtl.clientAdd + 4), respsize);
		if( ret != 0 ) return IPC_SERVER_ERR_MEM_WRITE;
	}

	ret = ipcServerCtl.irqSend();
	if( ret != 0 ) return IPC_SERVER_ERR_IRQ_SEND;

	return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t ipcServerMemRead(void *src, void *dst, int32_t size){

	uint8_t *s = (uint8_t *)src;
	uint8_t *d = (uint8_t *)dst;

	while(size--){
		*d++ = *s++;
	}

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ipcServerMemWrite(void *src, void *dst, int32_t size){

	uint8_t *s = (uint8_t *)src;
	uint8_t *d = (uint8_t *)dst;

	while(size--){
		*d++ = *s++;
	}

	return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
