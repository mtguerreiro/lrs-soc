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
#ifdef SOC_CPU0
#define OCP_IF_CONFIG_CORE	1
#else
#define OCP_IF_CONFIG_CORE	0
#endif

typedef enum{
    /**
     * Command
     *  -----------------------------------------
     *  | TRACE READ (uint32) | TRACE ID (uint32) |
     *  -----------------------------------------
     *
     * Response
     *  ------------------------
     *  | TRACE DATA (N bytes) |
     *  ------------------------
     *
     *  Notes
     *  - There is no specified format for the trace data. It is up to the user
     *    to decode these values.
     */
	OCP_IF_CMD_TRACE_READ = 0,

    /**
     * Command
     *  --------------------------------------------
     *  | RESET TRACE (uint32) | TRACE ID (uint32) |
     *  --------------------------------------------
     *
     * Response
     * No response
     */
	OCP_IF_CMD_TRACE_RESET,

    /**
     * Command
     *  -----------------------------------------------
     *  | GET TRACE SIZE (uint32) | TRACE ID (uint32) |
     *  -----------------------------------------------
     *
     * Response
     *  -----------------------
     *  | TRACE SIZE (uint32) |
     *  -----------------------
     */
	OCP_IF_CMD_TRACE_GET_SIZE,

    /**
     * Command
     *  ---------------------------------------------------------------
     *  | SET TRACE SIZE (uint32) | TRACE ID (uint32) | SIZE (uint32) |
     *  ---------------------------------------------------------------
     *
     * Response
     * No response
     *
     * Notes
     * - The size of the trace is always in bytes. Thus, if SIZE is 10,
     *   10 bytes will be allocated to the specified trace.
     * - This command will fail if the required size is larger than the
     *   maximum allowed size.
     */
	OCP_IF_CMD_TRACE_SET_SIZE,

    /**
     * Command
     *  --------------------------------------------------------
     *  | GET # OF TRACED SIGNALS (uint32) | TRACE ID (uint32) |
     *  --------------------------------------------------------
     *
     * Response
     *  --------------------------------
     *  | # OF TRACED SIGNALS (uint32) |
     *  --------------------------------
     */
	OCP_IF_CMD_TRACE_GET_NUMBER_SIGNALS,

    /**
     * Command
     *  ------------------------------------------------------------
     *  | GET NAMES OF TRACED SIGNALS (uint32) | TRACE ID (uint32) |
     *  ------------------------------------------------------------
     *
     * Response
     *  -------------------------------------
     *  | NAMES OF TRACED SIGNALS (N bytes) |
     *  -------------------------------------
     *
     * Notes
     * - Names are returned as strings, each terminated by '\0'.
     */
	OCP_IF_CMD_TRACE_GET_SIGNALS_NAMES,

    /**
     * Command
     *  ---------------------------------
     *  | GET NUMBER OF TRACES (uint32) |
     *  ---------------------------------
     *
     * Response
     *  -----------------------------
     *  | NUMBER OF TRACES (uint32) |
     *  -----------------------------
     *
     * Notes
     * - Names are returned as strings, each terminated by '\0'.
     */
	OCP_IF_CMD_TRACE_GET_NUMBER_TRACES,

    /**
     * Command
     *  --------------------------------
     *  | GET NAMES OF TRACES (uint32) |
     *  --------------------------------
     *
     * Response
     *  -----------------------------
     *  | NAMES OF TRACES (N bytes) |
     *  -----------------------------
     *
     * Notes
     * - Names are returned as strings, each terminated by '\0'.
     */
	OCP_IF_CMD_TRACE_GET_TRACES_NAMES,

    /**
     * Command
     *  ---------------------------------------------------
     *  | GET TRACE ADDRESS (uint32) |  TRACE ID (uint32) |
     *  ---------------------------------------------------
     *
     * Response
     *  --------------------------
     *  | TRACE ADDRESS (uint32) |
     *  --------------------------
     */
	OCP_IF_CMD_TRACE_GET_ADDRESS,

    /**
     * Command
     *  ----------------------------------------------
     *  | GET STATUS OF CS (uint32) | CS ID (uint32) |
     *  ----------------------------------------------
     *
     * Response
     *  ----------------------
     *  | CS STATUS (uint32) |
     *  ----------------------
     *
     * Notes
     * - The meaning of the status is defined in the "ocpCSStatus_t"
     *   declaration in "ocpCS.h".
     */
	OCP_IF_CMD_CS_STATUS,

    /**
     * Command
     *  ---------------------------------------
     *  | ENABLE CS (uint32) | CS ID (uint32) |
     *  ---------------------------------------
     *
     * Response
     * No response
     */
	OCP_IF_CMD_CS_ENABLE,

    /**
     * Command
     *  ----------------------------------------
     *  | DISABLE CS (uint32) | CS ID (uint32) |
     *  ----------------------------------------
     *
     * Response
     * No response
     */
	OCP_IF_CMD_CS_DISABLE,

    /**
     * Command
     *  --------------------------------------------------------------------
     *  | ACCESS CTL. IF. OF CS (uint32) | CS ID (uint32) | DATA (N bytes) |
     *  --------------------------------------------------------------------
     *
     * Response
     *  ------------------
     *  | DATA (M bytes) |
     *  ------------------
     *
     * Note
     * - The contents of the response depends on the controller interface,
     *   and commands may or may not have responses.
     */
	OCP_IF_CMD_CS_CONTROLLER_IF,

    /**
     * Command
     *  ------------------------------------------------------------------
     *  | ACCESS HW IF. OF CS (uint32) | CS ID (uint32) | DATA (N bytes) |
     *  ------------------------------------------------------------------
     *
     * Response
     *  ------------------
     *  | DATA (M bytes) |
     *  ------------------
     *
     * Note
     * - The contents of the response depends on the hardware interface,
     *   and commands may or may not have responses.
     */
	OCP_IF_CMD_CS_HARDWARE_IF,

    /**
     * Command
     *  -------------------------
     *  | GET # OF CSs (uint32) |
     *  -------------------------
     *
     * Response
     *  ---------------------
     *  | # OF CSs (uint32) |
     *  ---------------------
     *
     */
	OCP_IF_CMD_CS_GET_NUMBER_CONTROLLERS,

    /**
     * Command
     *  -----------------------------
     *  | GET NAMES OF CSs (uint32) |
     *  -----------------------------
     *
     * Response
     *  --------------------------
     *  | NAMES OF CSs (N bytes) |
     *  --------------------------
     *
     * Notes
     * - Names are returned as strings, each terminated by '\0'.
     */
	OCP_IF_CMD_CS_GET_CONTROLLERS_NAMES,

	OCP_IF_CMD_PLATFORM_ID,
	OCP_IF_CMD_PLATFORM_IF,
	OCP_IF_CMD_OPIL_UPDATE_MEASUREMENTS,
    OCP_IF_CMD_OPIL_UPDATE_SIM_DATA,
    OCP_IF_CMD_OPIL_RUN_CONTROL,
    OCP_IF_CMD_OPIL_INITIALIZE_CONTROL,
    OCP_IF_CMD_OPIL_GET_CONTROL,
    OCP_IF_CMD_OPIL_GET_CONTROLLER_DATA,
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

#endif /* OCP_IF_H_ */
