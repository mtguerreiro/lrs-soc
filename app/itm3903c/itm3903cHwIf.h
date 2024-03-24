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

    ITM3903C_HW_DIGITAL_IF_GET_VERSION,

    ITM3903C_HW_DIGITAL_IF_GET_ERROR,

    ITM3903C_HW_DIGITAL_IF_CLEAR_ERROR,

    ITM3903C_HW_DIGITAL_IF_SET_OUTPUT_STATUS,

    ITM3903C_HW_DIGITAL_IF_GET_OUTPUT_STATUS,

    ITM3903C_HW_DIGITAL_IF_SET_ANALOG_EXTERNAL_STATUS,

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

    ITM3903C_HW_DIGITAL_IF_SET_FUNC_MODE,

    ITM3903C_HW_DIGITAL_IF_GET_FUNC_MODE,

    ITM3903C_HW_DIGITAL_IF_SET_VOLT_VALUE,

    ITM3903C_HW_DIGITAL_IF_SET_CURR_VALUE,

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
