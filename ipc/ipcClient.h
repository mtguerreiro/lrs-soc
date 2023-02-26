/*
 * @file ipcClient.h
 *
 * @brief Client for the inter-processor communication (IPC) library.
 *
 * For a description of how the inter-processor communication mechanism works,
 * please refer to the server file (ipcServer.h).
 */

#ifndef IPCCLIENT_H_
#define IPCCLIENT_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
#include "stddef.h"

//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
/**
 * @brief Callback function to generate an interrupt signal on the server core.
 *
 * @return 0 on success, any other valuer otherwise.
 */
typedef int32_t (*ipcClientIrqSend)(void);

/**
 * @brief Callback function to detect an interrupt signal from the server core.
 *
 * @param timeout Interrupt detection timeout.
 *
 * @return 0 if interrupt was detected, any other value otherwise.
 */
typedef int32_t (*ipcClientIrqReceive)(uint32_t timeout);

#define IPC_CLIENT_ERR_MEM_WRITE		-1	/*!< Failed to write to memory */
#define IPC_CLIENT_ERR_MEM_READ			-2  /*!< Failed to read from memory */
#define IPC_CLIENT_ERR_IRQ_SEND			-3	/*!< Failed to generate interrupt */
#define IPC_CLIENT_ERR_IRQ_RECEIVE		-4	/*!< Failed to receive interrupt */
#define IPC_CLIENT_ERR_SV_RESP_SIZE		-5	/*!< Server's response exceeds client's buffer size */
#define IPC_CLIENT_ERR_SV_REQ_SIZE		-6	/*!< Client's request exceeds server's buffer size */
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
/**
 * @brief Initializes the client.
 *
 * @param irqSend Callback to generate an interrupt on the server core.
 *
 * @param irqReceive Callback to detect an interrupt from the server core.
 *
 * @param clientMemAdd Address of buffer to hold data sent from the server to
 * the client (server->client memory).
 *
 * @param clientMemSize Size of the client's buffer.
 *
 * @param serverMemAdd Address of buffer holding data sent by the client
 * (client->server memory).
 *
 * @param serverMemSize Size of the server's buffer.
 *
 */
void ipcClientInitialize(ipcClientIrqSend irqSend, ipcClientIrqReceive irqReceive,
		size_t clientMemAdd, int32_t clientMemSize,
		size_t serverMemAdd, int32_t serverMemSize);
//-----------------------------------------------------------------------------
/**
 * @brief Sends a request to the server.
 *
 * @param req Pointer to buffer holding the request.
 *
 * @param reqsize Size of the request, in bytes.
 *
 * @param resp Pointer to pointer holding the address of the buffer where data
 * sent from the server should be stored.
 *
 * @param maxrespsize Size of the buffer to hold the server's response.
 *
 * @param timeout Time to wait for a response from the server.
 *
 * @return The size of the request's response (>= 0) or an error (< 0).
 */
int32_t ipcClientRequest(void *req, int32_t reqsize,
		void **resp, int32_t maxrespsize,
		uint32_t timeout);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* IPCCLIENT_H_ */
