/*
 * @file rp.h
 *
 * @brief Request processor (RP).
 *
 * The purpose of the RP library is to provide a mechanism to route data to
 * functions. For example, when building a command-line interface (CLI),
 * different commands can be assigned to different functions, e.g., blink an
 * LED, send a message, read an ADC.
 *
 * The RP library allows the user to assign callbacks to different commands.
 * Thus, whenever a request (or command) is received, the corresponding
 * callback is called.
 *
 * To achieve this, the RP library implements a small protocol. The request
 * must have the following format:
 *
 * ---------------------------------
 * | ID (4 bytes) | DATA (n bytes) |
 * ---------------------------------
 *
 * - ID is a 32-bit value that is assigned to a callback.
 * - DATA is the data to be passed to the function.
 *
 * When a request is received, RP identifies the ID an calls the assigned
 * function, forwarding only the data.
 *
 * To make the library smaller and more efficient, the IDs (or commands) must
 * be unsigned 32-bit integer values, starting at 0 and increasing
 * continuously.
 *
 */

#ifndef RP_H_
#define RP_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef uint32_t rpuint_t;
typedef int32_t rpint_t;
typedef rpuint_t rpid_t;

/**
 * @brief Callback function for each command.
 *
 * @param in Pointer to buffer holding input data.
 *
 * @param insize Size of the input data, in number of bytes.
 *
 * @param out Pointer to pointer holding the output buffer's address. This is
 * implemented as a pointer to pointer for maximum flexibility. The user can
 * either write the command's response to the buffer given initially, or the
 * user can overwrite the pointer to point to another buffer.
 *
 * @param maxoutsize Size of the initial output buffer.
 *
 * @return The size of the request's response (>= 0) or an error (< 0).
 */
typedef rpint_t (*rphandle_t)(void *in, rpuint_t insize, void **out, rpuint_t maxoutsize);

/**
 * @brief A structure to hold the RP's context.
 */
typedef struct rpctx_t{

	uint32_t maxid; /*!< Maximum number of IDs allowed */

	rphandle_t *handle; /*!< Pointer to vector to hold the callback functions */
}rpctx_t;

#define RP_ERR_INVALID_ID	-1	/*!< Received ID is invalid */
#define RP_ERR_NO_HANDLE	-2	/*!< No handle assigned to received ID */
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
/**
 * @brief Initializes the request processor.
 *
 * @param rp Context.
 *
 * @param maxid Maximum number of IDs.
 *
 * @param handlebuffer Buffer to hold registered callback functions.
 */
void rpInitialize(rpctx_t *rp, rpuint_t maxid, rphandle_t *handlebuffer);
//-----------------------------------------------------------------------------
/**
 * @brief Assign an ID to a callback.
 *
 * @param rp Context.
 *
 * @param id ID to be assigned to callback.
 *
 * @param handle Callback to be assigned to handle.
 *
 * @return 0 if the callback was assigned successfully, an error code
 * otherwise.
 */
rpint_t rpRegisterHandle(rpctx_t *rp, rpid_t id, rphandle_t handle);
//-----------------------------------------------------------------------------
/**
 * @brief Processes a request.
 *
 * @param rp Context.
 *
 * @param in Pointer to buffer holding the request.
 *
 * @param insize Size of the request.
 *
 * @param out Pointer to pointer holding the output buffer's address.
 *
 * @param maxoutsize Size of the initial output buffer.
 *
 * @return The size of the request's response (>= 0) or an error (< 0).
 */
rpint_t rpRequest(rpctx_t *rp, void *in, rpuint_t insize, void **out, rpuint_t maxoutsize);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* RP_H_ */
