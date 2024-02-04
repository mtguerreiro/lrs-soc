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
    ITM3903C_HW_IF_SET_SLOPE,

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
    ITM3903C_HW_IF_GET_SLOPE,

    ITM3903C_HW_IF_END
}itm3903cHwIfCommands_t;

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t itm3903cHwIfInitialize(void);
//-----------------------------------------------------------------------------
int32_t itm3903cHwIf(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* ITM3903C_HW_IF_H_ */
