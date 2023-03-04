/*
 * ocpIf.h
 *
 *  Created on: 17.02.2023
 *      Author: mguerreiro
 */

#ifndef OCP_IF_H_
#define OCP_IF_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"

//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define OCP_IF_CONFIG_CORE	0

typedef enum{
	OCP_IF_CMD_TRACE_READ = 0,
	OCP_IF_CMD_TRACE_RESET,
	OCP_IF_CMD_TRACE_GET_SIZE,
	OCP_IF_CMD_TRACE_SET_SIZE,
	OCP_IF_CMD_TRACE_GET_NUMBER_SIGNALS,
	OCP_IF_CMD_TRACE_GET_SIGNALS_NAMES,
	OCP_IF_CMD_TRACE_GET_NUMBER_TRACES,
	OCP_IF_CMD_TRACE_GET_TRACES_NAMES,
	OCP_IF_CMD_TRACE_GET_ADDRESS,
	OCP_IF_CMD_CS_STATUS,
	OCP_IF_CMD_CS_ENABLE,
	OCP_IF_CMD_CS_DISABLE,
	OCP_IF_CMD_CS_CONTROLLER_IF,
	OCP_IF_CMD_CS_HARDWARE_IF,
	OCP_IF_CMD_CS_GET_NUMBER_CONTROLLERS,
	OCP_IF_CMD_CS_GET_CONTROLLERS_NAMES,
	OCP_IF_CMD_PLATFORM_ID,
	OCP_IF_CMD_PLATFORM_IF,
	OCP_IF_CMD_END
}ocpIfCommands_t;

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t ocpIfInitialize(void);
//-----------------------------------------------------------------------------
int32_t ocpIf(void *in, int32_t insize, void **out, int32_t maxoutsize);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* OCPIF_H_ */
