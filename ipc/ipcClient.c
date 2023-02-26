/*
 * @file ipcClient.c
 *
 * @brief Client for the inter-processor communication (IPC) library.
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "ipcClient.h"

//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
/**
 * @brief Reads from memory.
 *
 * @param src Pointer to memory to be read.
 *
 * @param dst Pointer to buffer to hold data read.
 *
 * @param size Number of bytes to be read.
 *
 * @return 0 if memory was read successfully, another value otherwise.
 */
static int32_t ipcClientMemRead(void *src, void *dst, int32_t size);
//-----------------------------------------------------------------------------
/**
 * @brief Writes to memory.
 *
 * @param src Pointer to buffer holding data to be written.
 *
 * @param dst Pointer to memory.
 *
 * @param size Number of bytes to be write.
 *
 * @return 0 if memory was written successfully, another value otherwise.
 */
static int32_t ipcClientMemWrite(void *src, void *dst, int32_t size);
//-----------------------------------------------------------------------------
/**
 * @brief Sends a request to the server.
 *
 * @param req Pointer to buffer holding the request.
 *
 * @param reqsize Size of the request, in bytes.
 *
 * @return 0 if the request was sent, any other value otherwise.
 */
static int32_t ipcClientRequestSend(void *req, int32_t reqsize);
//-----------------------------------------------------------------------------
/**
 * @brief Receives the server's response.
 *
 * @param resp Pointer to pointer holding the address of the buffer where data
 * sent from the server should be stored.
 *
 * @param maxrespsize Size of the buffer to hold the server's response.
 *
 * @return The size of the request's response (>= 0) or an error (< 0).
 */
static int32_t ipcClientRequestResponse(void **resp, int32_t maxrespsize,
		uint32_t timeout);
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{

	ipcClientIrqSend irqSend;
	ipcClientIrqReceive irqReceive;

	size_t serverAdd;
	int32_t serverSize;

	size_t clientAdd;
	int32_t clientSize;
}ipcClientCtl_t;
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static ipcClientCtl_t ipcClientCtl = {.serverAdd = 0, .serverSize = 0,
		.clientAdd = 0, .clientSize = 0};
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void ipcClientInitialize(ipcClientIrqSend irqSend, ipcClientIrqReceive irqReceive,
		size_t clientMemAdd, int32_t clientMemSize,
		size_t serverMemAdd, int32_t serverMemSize){

	ipcClientCtl.irqSend = irqSend;
	ipcClientCtl.irqReceive = irqReceive;

	ipcClientCtl.clientAdd = clientMemAdd;
	ipcClientCtl.clientSize = clientMemSize - 4U;

	ipcClientCtl.serverAdd = serverMemAdd;
	ipcClientCtl.serverSize = serverMemSize - 4U;
}
//-----------------------------------------------------------------------------
int32_t ipcClientRequest(void *req, int32_t reqsize,
		void **resp, int32_t maxrespsize,
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

	size_t *s = (size_t *)src;
	size_t *d = (size_t *)dst;

	/* Size is rounded up instead of being truncated */
	size = (size + sizeof(size_t) - 1) / sizeof(size_t);

	while(size--){
		*d++ = *s++;
	}

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ipcClientMemWrite(void *src, void *dst, int32_t size){

	size_t *s = (size_t *)src;
	size_t *d = (size_t *)dst;

	/* Size is rounded up instead of being truncated */
	size = (size + sizeof(size_t) - 1) / sizeof(size_t);

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
static int32_t ipcClientRequestResponse(void **resp, int32_t maxrespsize,
		uint32_t timeout){

	int32_t ret;
	int32_t respsize;

	ret = ipcClientCtl.irqReceive(timeout);
	if( ret != 0 ) return IPC_CLIENT_ERR_IRQ_RECEIVE;

	ret = ipcClientMemRead((void *)( ipcClientCtl.clientAdd ), (void *)( &respsize ), 4);
	if( ret != 0 ) return IPC_CLIENT_ERR_MEM_WRITE;

	if( respsize > maxrespsize ) return IPC_CLIENT_ERR_SV_RESP_SIZE;

	if( ( respsize > 0 ) && ( *resp != 0 ) ){
		ret = ipcClientMemWrite((void *)( ipcClientCtl.clientAdd + 4 ), (void *)*resp, respsize);
		if( ret != 0 ) return IPC_CLIENT_ERR_MEM_WRITE;
	}

	return respsize;
}
//-----------------------------------------------------------------------------
//=============================================================================
