/*
 * @file ipcServer.h
 *
 * @brief Server for the inter-processor communication (IPC) library.
 *
 * The purpose of the IPC library is to allow two cores of a dual-core system
 * to exchange data, based on a client-server methodology. For this, the IPC
 * library implements a small protocol based on some assumptions.
 *
 * - Both cores share some memory region.
 * - The client core always initiates a transaction, and the server core
 *   always reply to a request.
 * - Each core can generate an interrupt signal on the other.
 *
 * The protocol
 * ============
 *
 * The protocol is as follows. To make a request, the client must send the
 * data to the server with the following format:
 *
 * -----------------------------------
 * | SIZE (4 bytes) | DATA (n bytes) |
 * -----------------------------------
 *
 * - SIZE is the number of bytes that to be sent to the server.
 *
 * - DATA is the data to be sent to the server.
 *
 * The server replies to all requests, following the same format. If the
 * request requires no answer, the server still writes SIZE, but the value
 * will be zero.
 *
 * Inner workings
 * =============
 *
 * Internally, the data exchange between the two cores works based on memory
 * sharing and interrupt signals.
 *
 * - The client first writes the request to a memory region known and
 *   accessible by the server (server's address, or client->server memory).
 *   The request has the format given above.
 *
 * - After writing all data, the client generates and interrupt on the server,
 *   and waits for an interrupt from the server.
 *
 * - Upon receiving an interrupt from the client, the server accesses the
 *   memory region designated for receiving data from the client.
 *
 * - The server processes the request and writes its response to a memory
 *   region known and accessible by the client (client's address, or
 *   server->client memory). If there is no response to be written, the server
 *   still writes SIZE, but as 0.
 *
 * - After writing all data, the server generates an interrupt on the client.
 *
 * - Upon receiving an interrupt from the server, the client accesses the
 * 	 memory region designated for receiving data from the server.
 *
 * Design decisions
 * ================
 *
 * The library needs some hardware details to work properly, namely generating
 * interrupts, knowing shared memory addresses and generating interrupts. To
 * try to make the library portable, these aspects are configured by the user.
 *
 * - The user must register callback functions that generate or detect
 *   interrupts (depending if client or server).
 *
 * - Memory regions and its sizes for client->server and server->client
 *   exchange are set when initializing the library.
 *
 * - Internally, the library uses a standard memory write/read function for
 *   memory accesses. These functions copy data 1 byte at a time. These
 *   functions can be changed to be adapted to specific platforms (for
 *   example, C2000 does not accept byte access).
 *
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
/**
 * @brief Callback function to generate an interrupt signal on the client core.
 *
 * @return 0 on success, any other valuer otherwise.
 */
typedef int32_t (*ipcServerIrqSend)(void);

/**
 * @brief Callback function to process a client request.
 *
 * @param req Pointer to buffer holding the request.
 *
 * @param reqsize Size of request, in bytes.
 *
 * @param resp Pointer to pointer holding the response. The user can either
 * write to the buffer given by this pointer or can overwrite this pointer to
 * indicate the location of the response.
 *
 * @param maxrespsize Maximum response size, when using the buffer given by
 * resp.
 *
 * @return The size of the request's response (>= 0) or an error (< 0).
 */
typedef int32_t (*ipcServerRequestHandle)(void *req, int32_t reqsize, void **resp, int32_t maxrespsize);

#define IPC_SERVER_ERR_MEM_WRITE		-1	/*!< Failed to write to memory */
#define IPC_SERVER_ERR_MEM_READ			-2  /*!< Failed to read from memory */
#define IPC_SERVER_ERR_IRQ_SEND			-3	/*!< Failed to generate interrupt */
#define IPC_SERVER_ERR_IRQ_RECEIVE		-4	/*!< Failed to receive interrupt */
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
/**
 * @brief Initializes the server.
 *
 * @param reqHandle Request handler. This function will be called whenever a
 * new request is received.
 *
 * @param irqSend Callback to generate an interrupt on the client core.
 *
 * @param serverMemAdd Address of buffer holding data sent by the client
 * (client->server memory).
 *
 * @param serverMemSize Size of the server's buffer.
 *
 * @param clientMemAdd Address of buffer to hold data sent from the server to
 * the client (server->client memory).
 *
 * @param clientMemSize Size of the client's buffer.
 */
void ipcServerInitialize(ipcServerRequestHandle reqHandle,
		ipcServerIrqSend irqSend,
		uint32_t serverMemAdd, int32_t serverMemSize,
		uint32_t clientMemAdd, int32_t clientMemSize);
//-----------------------------------------------------------------------------
/**
 * @brief Processes a request.
 *
 * This function should be called whenever an interrupt from the client core
 * is detected on the server core.
 *
 * @param 0 if the request was processed successfully, an error code otherwise.
 */
int32_t ipcServerRequest(void);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* IPCSERVER_H_ */
