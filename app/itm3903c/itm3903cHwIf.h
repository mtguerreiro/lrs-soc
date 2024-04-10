/*
 * imt3903cHwIf.h
 *
 */

#ifndef ITM3903C_HW_IF_H_
#define ITM3903C_HW_IF_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef enum{
    /**
     * Command
     *  ----------------------------------------------------------
     *  | SET SLOPE (uint32)t | CHANNEL (uint32) | SLOPE (float) |
     *  ----------------------------------------------------------
     *
     * Response
     * No response
     *
     */
    ITM3903C_HW_DIGITAL_IF_SET_SLOPE,

    /**
     * Command
     *  ------------------------------------------
     *  | GET SLOPE (uint32)t | CHANNEL (uint32) |
     *  ------------------------------------------
     *
     * Response
     *  ------------------
     *  | SLOPE (uint32) |
     *  ------------------
     */
    ITM3903C_HW_DIGITAL_IF_GET_SLOPE,

    /**
     * Command
     *  --------------------------------------------------------
     *  | GET VERSION (uint32)t | *O (char) | MAXSIZE (uint32) |
     *  --------------------------------------------------------
     *
     * Response
     *  ------------------
     *  | SIZE (uint32) |
     *  ------------------
     */
    ITM3903C_HW_DIGITAL_IF_GET_VERSION,

    /**
     * Command
     *  --------------------------------------------------------
     *  | GET ERROR (uint32)t | *O (char) | MAXSIZE (uint32) |
     *  --------------------------------------------------------
     *
     * Response
     *  ------------------
     *  | SIZE (uint32) |
     *  ------------------
     */
    ITM3903C_HW_DIGITAL_IF_GET_ERROR,

    /**
     * Command
     *  -------------------------
     *  | CLEAR ERROR (uint32)t |
     *  -------------------------
     *
     * Response
     * No response
     *
     */
    ITM3903C_HW_DIGITAL_IF_CLEAR_ERROR,


    /**
     * Command
     *  ----------------------------------------------------
     *  | SET OUTPUT STATUS (uint32)t | SETSTATUS (uint32) |
     *  ----------------------------------------------------
     *
     * Response
     * No response
     *
     */
    ITM3903C_HW_DIGITAL_IF_SET_OUTPUT_STATUS,

    /**
     * Command
     *  -------------------------------
     *  | GET OUTPUT STATUS (uint32)t |
     *  -------------------------------
     *
     * Response
     *  --------------------------
     *  | OUTPUT_STATUS (uint32) |
     *  --------------------------
     */
    ITM3903C_HW_DIGITAL_IF_GET_OUTPUT_STATUS,

    /**
     * Command
     *  -------------------------------------------------------------
     *  | SET ANALOG EXTERNAL STATUS (uint32)t | SETSTATUS (uint32) |
     *  -------------------------------------------------------------
     *
     * Response
     * No response
     *
     */
    ITM3903C_HW_DIGITAL_IF_SET_ANALOG_EXTERNAL_STATUS,

    /**
     * Command
     *  ----------------------------------------
     *  | GET ANALOG EXTERNAL STATUS (uint32)t |
     *  ----------------------------------------
     *
     * Response
     *  --------------------------
     *  | OUTPUT_STATUS (uint32) |
     *  --------------------------
     */
    ITM3903C_HW_DIGITAL_IF_GET_ANALOG_EXTERNAL_STATUS,
    
    /**
     * Command
     *  -----------------------------------------------------------
     *  | SET OFFSET (uint32) | CHANNEL (uint32) | OFFSET (float) |
     *  -----------------------------------------------------------
     *
     * Response
     * No response
     *
     */
    ITM3903C_HW_DIGITAL_IF_SET_OFFSET,

    /**
     * Command
     *  ------------------------------------------
     *  | GET OFFSET (uint32) | CHANNEL (uint32) |
     *  ------------------------------------------
     *
     * Response
     *  -------------------
     *  | OFFSET (uint32) |
     *  -------------------
     */
    ITM3903C_HW_DIGITAL_IF_GET_OFFSET,

    /**
     * Command
     *  --------------------------------------------------
     *  | SET FUNCTION MODE (uint32) | FUNCMODE (uint32) |
     *  --------------------------------------------------
     *
     * Response
     * No response
     *
     */
    ITM3903C_HW_DIGITAL_IF_SET_FUNC_MODE,

    /**
     * Command
     *  --------------------------------------------------------------
     *  | GET FUNCTION MODE (uint32)t | *O (char) | MAXSIZE (uint32) |
     *  --------------------------------------------------------------
     *
     * Response
     *  ------------------
     *  | SIZE (uint32) |
     *  ------------------
     */
    ITM3903C_HW_DIGITAL_IF_GET_FUNC_MODE,

    /**
     * Command
     *  ----------------------------------------------------------
     *  | SET VALUE (uint32) | VALUE (float) | CURRORVOLT (bool) |
     *  ----------------------------------------------------------
     *
     * Response
     * No response
     *
     */
    ITM3903C_HW_DIGITAL_IF_SET_VOLT_VALUE,

    /**
     * Command
     *  ----------------------------------------------------------
     *  | SET VALUE (uint32) | VALUE (float) | CURRORVOLT (bool) |
     *  ----------------------------------------------------------
     *
     * Response
     * No response
     *
     */
    ITM3903C_HW_DIGITAL_IF_SET_CURR_VALUE,

    /**
     * Command
     *  ------------------------------------
     *  | GET VOLTAGE MESAUREMENT (uint32) |
     *  ------------------------------------
     *
     * Response
     *  --------------------------------
     *  | VOLTAGE_MEASUREMENT (uint32) |
     *  --------------------------------
     */
    ITM3903C_HW_DIGITAL_IF_GET_VOLTAGE_MEASUREMENT,

    /**
     * Command
     *  ------------------------------------
     *  | GET CURRENT MESAUREMENT (uint32) |
     *  ------------------------------------
     *
     * Response
     *  --------------------------------
     *  | CURRENT_MEASUREMENT (uint32) |
     *  --------------------------------
     */
    ITM3903C_HW_DIGITAL_IF_GET_CURRENT_MEASUREMENT,

    /**
     * Command
     *  --------------------------------
     *  | GET VOLTAGE MAXIMUM (uint32) |
     *  --------------------------------
     *
     * Response
     *  ------------------------
     *  | VOLTAGE_MAX (uint32) |
     *  ------------------------
     */
    ITM3903C_HW_DIGITAL_IF_GET_VOLTAGE_MAX,

    /**
     * Command
     *  --------------------------------
     *  | GET VOLTAGE MINIMUM (uint32) |
     *  --------------------------------
     *
     * Response
     *  ------------------------
     *  | VOLTAGE_MIN (uint32) |
     *  ------------------------
     */
    ITM3903C_HW_DIGITAL_IF_GET_VOLTAGE_MIN,

    /**
     * Command
     *  ------------------------------------------------
     *  | SET VOLTAGE MAXIMUM (uint32) | VALUE (float) |
     *  ------------------------------------------------
     *
     * Response
     * No response
     *
     */
    ITM3903C_HW_DIGITAL_IF_SET_VOLTAGE_MAX,

    /**
     * Command
     *  ------------------------------------------------
     *  | SET VOLTAGE MINIMUM (uint32) | VALUE (float) |
     *  ------------------------------------------------
     *
     * Response
     * No response
     *
     */
    ITM3903C_HW_DIGITAL_IF_SET_VOLTAGE_MIN,

    /**
     * Command
     *  ------------------------------
     *  | GET POWER MAXIMUM (uint32) |
     *  ------------------------------
     *
     * Response
     *  ----------------------
     *  | POWER_MAX (uint32) |
     *  ----------------------
     */
    ITM3903C_HW_DIGITAL_IF_GET_POWER_MAX,

    /**
     * Command
     *  ------------------------------
     *  | GET POWER MINIMUM (uint32) |
     *  ------------------------------
     *
     * Response
     *  ----------------------
     *  | POWER_MIN (uint32) |
     *  ----------------------
     */
    ITM3903C_HW_DIGITAL_IF_GET_POWER_MIN,

    /**
     * Command
     *  ----------------------------------------------
     *  | SET POWER MAXIMUM (uint32) | VALUE (float) |
     *  ----------------------------------------------
     *
     * Response
     * No response
     *
     */
    ITM3903C_HW_DIGITAL_IF_SET_POWER_MAX,

    /**
     * Command
     *  ----------------------------------------------
     *  | SET POWER MINIMUM (uint32) | VALUE (float) |
     *  ----------------------------------------------
     *
     * Response
     * No response
     *
     */
    ITM3903C_HW_DIGITAL_IF_SET_POWER_MIN,

    /**
     * Command
     *  --------------------------------
     *  | GET CURRENT MAXIMUM (uint32) |
     *  --------------------------------
     *
     * Response
     *  ------------------------
     *  | CURRENT_MAX (uint32) |
     *  ------------------------
     */
    ITM3903C_HW_DIGITAL_IF_GET_CURRENT_MAX,

    /**
     * Command
     *  --------------------------------
     *  | GET CURRENT MINIMUM (uint32) |
     *  --------------------------------
     *
     * Response
     *  ------------------------
     *  | CURRENT_MIN (uint32) |
     *  ------------------------
     */
    ITM3903C_HW_DIGITAL_IF_GET_CURRENT_MIN,

    /**
     * Command
     *  ------------------------------------------------
     *  | SET CURRENT MAXIMUM (uint32) | VALUE (float) |
     *  ------------------------------------------------
     *
     * Response
     * No response
     *
     */
    ITM3903C_HW_DIGITAL_IF_SET_CURRENT_MAX,

    /**
     * Command
     *  ------------------------------------------------
     *  | SET CURRENT MINIMUM (uint32) | VALUE (float) |
     *  ------------------------------------------------
     *
     * Response
     * No response
     *
     */
    ITM3903C_HW_DIGITAL_IF_SET_CURRENT_MIN,

    /**
     * Command
     *  -------------------------------------------
     *  | GET VOLTAGE PROTECTION STATUS (uint32)t |
     *  -------------------------------------------
     *
     * Response
     *  -----------------------------
     *  | VOLT_PROT_STATUS (uint32) |
     *  -----------------------------
     */
    ITM3903C_HW_DIGITAL_IF_GET_VOLTAGE_PROTECTION_STATUS,

    /**
     * Command
     *  ----------------------------------------------------------------
     *  | SET VOLTAGE PROTECTION STATUS (uint32)t | SETSTATUS (uint32) |
     *  ----------------------------------------------------------------
     *
     * Response
     * No response
     *
     */
    ITM3903C_HW_DIGITAL_IF_SET_VOLTAGE_PROTECTION_STATUS,

    /**
     * Command
     *  -----------------------------------------
     *  | GET POWER PROTECTION STATUS (uint32)t |
     *  -----------------------------------------
     *
     * Response
     *  ------------------------------
     *  | POWER_PROT_STATUS (uint32) |
     *  ------------------------------
     */
    ITM3903C_HW_DIGITAL_IF_GET_POWER_PROTECTION_STATUS,

    /**
     * Command
     *  --------------------------------------------------------------
     *  | SET POWER PROTECTION STATUS (uint32)t | SETSTATUS (uint32) |
     *  --------------------------------------------------------------
     *
     * Response
     * No response
     *
     */
    ITM3903C_HW_DIGITAL_IF_SET_POWER_PROTECTION_STATUS,

    /**
     * Command
     *  -------------------------------------------
     *  | GET CURRENT PROTECTION STATUS (uint32)t |
     *  -------------------------------------------
     *
     * Response
     *  -----------------------------
     *  | CURR_PROT_STATUS (uint32) |
     *  -----------------------------
     */
    ITM3903C_HW_DIGITAL_IF_GET_CURRENT_PROTECTION_STATUS,

    /**
     * Command
     *  ----------------------------------------------------------------
     *  | SET CURRENT PROTECTION STATUS (uint32)t | SETSTATUS (uint32) |
     *  ----------------------------------------------------------------
     *
     * Response
     * No response
     *
     */
    ITM3903C_HW_DIGITAL_IF_SET_CURRENT_PROTECTION_STATUS,

    /**
     * Command
     *  -----------------------------------
     *  | GET VOLTAGE SLEW RATE (uint32)t |
     *  -----------------------------------
     *
     * Response
     *  ----------------------
     *  | SLEW_RATE (uint32) |
     *  ----------------------
     */
    ITM3903C_HW_DIGITAL_IF_GET_VOLTAGE_SLOW_RATE,

    /**
     * Command
     *  ---------------------------------------------------
     *  | SET VOLTAGE SLEW RATE (uint32)t | VALUE (float) |
     *  ---------------------------------------------------
     *
     * Response
     * No response
     *
     */
    ITM3903C_HW_DIGITAL_IF_SET_VOLTAGE_SLOW_RATE,
    
    ITM3903C_HW_DIGITAL_IF_END   
}itm3903cHwDigitalIfCommands_t;

typedef enum{

    ITM3903C_HW_ANALOG_IF_END
}itm3903cHwAnalogIfCommands_t;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t itm3903cHwIfInitialize(void);
//-----------------------------------------------------------------------------
int32_t itm3903cHwDigitalIf(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
int32_t itm3903cHwAnalogIf(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* ITM3903C_HW_IF_H_ */
