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
     *  ---------------------------------------------------------
     *  | SET SLOPE (uint32) | CHANNEL (uint32) | SLOPE (float) |
     *  ---------------------------------------------------------
     *
     * Response
     * No response
     * 
     * Notes
     *  -the function is used to set the linear calibration coefficient of the channel (1/2/3)
     *  -channel: 1,2,3
     *  -value range of slope (i.e. the linear calibration coefficient): min(-9000) to max(9000)
     */
    ITM3903C_HW_DIGITAL_IF_SET_SLOPE,

    /**
     * Command
     *  -----------------------------------------
     *  | GET SLOPE (uint32) | CHANNEL (uint32) |
     *  -----------------------------------------
     *
     * Response
     *  ------------------
     *  | SLOPE (uint32) |
     *  ------------------
     * 
     * Notes
     *  -the function is used to query the linear calibration coefficient of the channel (1/2/3).
     *  -channel: 1,2,3
     */
    ITM3903C_HW_DIGITAL_IF_GET_SLOPE,

    /**
     * Command
     *  ------------------------
     *  | GET VERSION (uint32) |
     *  ------------------------
     *
     * Response
     *  --------------------
     *  | Version (string) |
     *  --------------------
     */
    ITM3903C_HW_DIGITAL_IF_GET_VERSION,

    /**
     * Command
     *  ----------------------
     *  | GET ERROR (uint32) |
     *  ----------------------
     *
     * Response
     *  ------------------
     *  | ERROR (string) |
     *  ------------------
     */
    ITM3903C_HW_DIGITAL_IF_GET_ERROR,

    /**
     * Command
     *  ------------------------
     *  | CLEAR ERROR (uint32) |
     *  ------------------------
     *
     * Response
     * No response
     *
     */
    ITM3903C_HW_DIGITAL_IF_CLEAR_ERROR,


    /**
     * Command
     *  ------------------------------------------------
     *  | SET OUTPUT STATUS (uint32) | STATUS (uint32) |
     *  ------------------------------------------------
     *
     * Response
     * No response
     * 
     * Notes
     *  -enable or disable the output.
     */
    ITM3903C_HW_DIGITAL_IF_SET_OUTPUT_STATUS,

    /**
     * Command
     *  ------------------------------
     *  | GET OUTPUT STATUS (uint32) |
     *  ------------------------------
     *
     * Response
     *  --------------------------
     *  | OUTPUT_STATUS (uint32) |
     *  --------------------------
     */
    ITM3903C_HW_DIGITAL_IF_GET_OUTPUT_STATUS,

    /**
     * Command
     *  ---------------------------------------------------------
     *  | SET ANALOG EXTERNAL STATUS (uint32) | STATUS (uint32) |
     *  ----------------------------------------------------------
     *
     * Response
     * No response
     * 
     * Notes
     *  -enable or disable the analog external.
     */
    ITM3903C_HW_DIGITAL_IF_SET_ANALOG_EXTERNAL_STATUS,

    /**
     * Command
     *  ---------------------------------------
     *  | GET ANALOG EXTERNAL STATUS (uint32) |
     *  ---------------------------------------
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
     * Notes
      * Notes
     *  -the function is used to set the offset calibration coefficient of the channel (1/2/3).
     *  -channel: 1,2,3
     *  -value range of offset (i.e. offset calibration coefficient): min(-9000) to max(9000)
     */
    ITM3903C_HW_DIGITAL_IF_SET_OFFSET,

    /**
     * Command
     *  ------------------------------------------
     *  | GET OFFSET (uint32) | CHANNEL (uint32) |
     *  ------------------------------------------
     *
     * Response
     *  ------------------
     *  | OFFSET (float) |
     *  ------------------
     * 
     * Notes
     *  -the function is used to query the offset calibration coefficient of the channel (1/2/3).
     *  -channel: 1,2,3
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
     * Notes
     *  -the function is used to set the working mode of the power supply
     *  - If FUNCMODE is 0, then current mode is set. If 1, the voltage mode.
     */
    ITM3903C_HW_DIGITAL_IF_SET_FUNC_MODE,

    /**
     * Command
     *  ------------------------------
     *  | GET FUNCTION MODE (uint32) |
     *  ------------------------------
     *
     * Response
     *  --------------------
     *  | FUCMODE (string) |
     *  --------------------
     */
    ITM3903C_HW_DIGITAL_IF_GET_FUNC_MODE,

    /**
     * Command
     *  --------------------------------------
     *  | SET VALUE (uint32) | VALUE (float) | 
     *  --------------------------------------
     *
     * Response
     * No response
     * 
     * Note 
     *  -value in volts.
     */
    ITM3903C_HW_DIGITAL_IF_SET_VOLT_VALUE,

    /**
     * Command
     *  --------------------------------------
     *  | SET VALUE (uint32) | VALUE (float) |
     *  --------------------------------------
     *
     * Response
     * No response
     * 
     * Note 
     *  -value in amperes.
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
     *  | VOLTAGE_MEASUREMENT (float) |
     *  --------------------------------
     * 
     * Note 
     *  -Values returned in volts.
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
     *  | CURRENT_MEASUREMENT (float) |
     *  --------------------------------
     * 
     * Note 
     *  -Values returned in amperes.
     */
    ITM3903C_HW_DIGITAL_IF_GET_CURRENT_MEASUREMENT,

    /**
     * Command
     *  --------------------------------
     *  | GET VOLTAGE MAXIMUM (uint32) |
     *  --------------------------------
     *
     * Response
     *  -----------------------
     *  | VOLTAGE_MAX (float) |
     *  -----------------------
     * 
     * Note 
     *  -Values returned in volts.
     */
    ITM3903C_HW_DIGITAL_IF_GET_VOLTAGE_MAX,

    /**
     * Command
     *  --------------------------------
     *  | GET VOLTAGE MINIMUM (uint32) |
     *  --------------------------------
     *
     * Response
     *  -----------------------
     *  | VOLTAGE_MIN (float) |
     *  -----------------------
     * 
     * Note 
     *  -Values returned in volts.
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
     * Note 
     *  -value in volts.
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
     * Note 
     *  -value in volts.
     */
    ITM3903C_HW_DIGITAL_IF_SET_VOLTAGE_MIN,

    /**
     * Command
     *  ------------------------------
     *  | GET POWER MAXIMUM (uint32) |
     *  ------------------------------
     *
     * Response
     *  ---------------------
     *  | POWER_MAX (float) |
     *  ---------------------
     */
    ITM3903C_HW_DIGITAL_IF_GET_POWER_MAX,

    /**
     * Command
     *  ------------------------------
     *  | GET POWER MINIMUM (uint32) |
     *  ------------------------------
     *
     * Response
     *  ---------------------
     *  | POWER_MIN (float) |
     *  ---------------------
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
     *  -----------------------
     *  | CURRENT_MAX (float) |
     *  -----------------------
     * 
     * Note 
     *  -Values returned in amperes.
     */
    ITM3903C_HW_DIGITAL_IF_GET_CURRENT_MAX,

    /**
     * Command
     *  --------------------------------
     *  | GET CURRENT MINIMUM (uint32) |
     *  --------------------------------
     *
     * Response
     *  -----------------------
     *  | CURRENT_MIN (float) |
     *  -----------------------
     * 
     * Note 
     *  -Values returned in amperes.
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
     * Note 
     *  -value in amperes.
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
     * Note 
     *  -value in amperes.
     */
    ITM3903C_HW_DIGITAL_IF_SET_CURRENT_MIN,

    /**
     * Command
     *  -------------------------------------------
     *  | GET VOLTAGE PROTECTION STATUS (uint32) |
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
     *  | SET VOLTAGE PROTECTION STATUS (uint32) | SETSTATUS (uint32) |
     *  ----------------------------------------------------------------
     *
     * Response
     * No response
     * 
     * Notes
     *  -enable or disable the overvoltage protection.
     */
    ITM3903C_HW_DIGITAL_IF_SET_VOLTAGE_PROTECTION_STATUS,

    /**
     * Command
     *  -----------------------------------------
     *  | GET POWER PROTECTION STATUS (uint32) |
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
     *  | SET POWER PROTECTION STATUS (uint32) | SETSTATUS (uint32) |
     *  --------------------------------------------------------------
     *
     * Response
     * No response
     * 
     * Notes
     *  -enable or disable the power protection.
     */
    ITM3903C_HW_DIGITAL_IF_SET_POWER_PROTECTION_STATUS,

    /**
     * Command
     *  -------------------------------------------
     *  | GET CURRENT PROTECTION STATUS (uint32) |
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
     *  | SET CURRENT PROTECTION STATUS (uint32) | SETSTATUS (uint32) |
     *  ----------------------------------------------------------------
     *
     * Response
     * No response
     * 
     * Notes
     *  -enable or disable the overcurrent protection.
     */
    ITM3903C_HW_DIGITAL_IF_SET_CURRENT_PROTECTION_STATUS,

    /**
     * Command
     *  -----------------------------------
     *  | GET VOLTAGE SLEW RATE (uint32) |
     *  -----------------------------------
     *
     * Response
     *  ----------------------
     *  | SLEW_RATE (float) |
     *  ----------------------
     */
    ITM3903C_HW_DIGITAL_IF_GET_VOLTAGE_SLOW_RATE,

    /**
     * Command
     *  ---------------------------------------------------
     *  | SET VOLTAGE SLEW RATE (uint32) | VALUE (float) |
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

    /**
     * Command
     *  ------------------------------------------------
     *  | SET SAMPL. STATUS (uint32) | STATUS (uint32) |
     *  ------------------------------------------------
     *
     * Response
     * No response
     * 
     * Notes
     *  - If STATUS is set to 0, sampling is disable. Any other value enables 
     *  sampling.
     */
    ITM3903C_HW_ANALOG_IF_SET_SAMPLING_STATUS,

    /**
     * Command
     *  ------------------------------
     *  | GET SAMPL. STATUS (uint32) |
     *  ------------------------------
     *
     * Response
     *  -------------------
     *  | STATUS (uint32) |
     *  -------------------
     * 
     * Notes
     *  - If STATUS is set to 0, sampling is disable. Any other value enables 
     *  sampling.
     */
    ITM3903C_HW_ANALOG_IF_GET_SAMPLING_STATUS,

    /**
     * Command
     *  ------------------------------------------
     *  | SET SAMP FREQ (uint32) | FREQ (uint32) |
     *  ------------------------------------------
     * 
     * Response
     * No response
     * 
     * Notes
     *  - FREQ is in Hz.
     */
    ITM3903C_HW_ANALOG_IF_SET_SAMPLING_FREQ,

    /**
     * Command
     *  --------------------------
     *  | GET SAMP FREQ (uint32) |
     *  --------------------------
     *
     * Response
     *  -----------------
     *  | FREQ (uint32) |
     *  -----------------
     * 
     * Notes
     *  - FREQ is in Hz.
     */
    ITM3903C_HW_ANALOG_IF_GET_SAMPLING_FREQ,

    /**
     * Command
     *  -----------------------------------------------
     *  | SET DAC A1 OFFSET (uint32) | OFFSET (float) |
     *  -----------------------------------------------
     * 
     * Response
     * No response
     * 
     * Notes
     *  - OFFSET is in volts.
     */
    ITM3903C_HW_ANALOG_IF_SET_DAC_A1_OFFSET,

    /**
     * Command
     *  -----------------------------------------
     *  | SET DAC A1 ADJ (uint32) | ADJ (float) |
     *  -----------------------------------------
     * 
     * Response
     * No response
     * 
     * Notes
     *  - ADJ is in volts.
     */
    ITM3903C_HW_ANALOG_IF_SET_DAC_A1_ADJ,

    /**
     * Command
     *  -----------------------------------------
     *  | SET DAC23 A2 (uint32) | VALUE (float) |
     *  -----------------------------------------
     * 
     * Response
     * No response
     * 
     * Notes
     *  - VALUE is in volts.
     */
    ITM3903C_HW_ANALOG_IF_SET_DAC_A2,

    /**
     * Command
     *  -----------------------------------------
     *  | SET DAC23 A3 (uint32) | VALUE (float) |
     *  -----------------------------------------
     * 
     * Response
     * No response
     * 
     * Notes
     *  - VALUE is in volts.
     */
    ITM3903C_HW_ANALOG_IF_SET_DAC_A3,

    /**
     * Command
     *  -------------------------------------------
     *  | SET CAL DATA (uint32) | DATA (8 floats) |
     *  -------------------------------------------
     * 
     * Response
     * No response
     * 
     * Notes
     *  - Calibration data is expected in the following order:
     *    - A1_ADJ_GAIN
     *    - A1_ADJ_OFFSET
     *    - A1_OFFSET_GAIN
     *    - A1_OFFSET_OFFSET
     *    - A2_GAIN
     *    - A2_OFFSET
     *    - A3_GAIN
     *    - A3_OFFSET
     */
    ITM3903C_HW_ANALOG_IF_SET_DAC_CAL_DATA,

    /**
     * Command
     *  -------------------------------------------
     *  | GET CAL DATA (uint32) | DATA (8 floats) |
     *  -------------------------------------------
     * 
     * Response
     *  -----------------------
     *  | CAL DATA (8 floats) |
     *  -----------------------
     * 
     * Notes
     *  - Calibration data is returned in the following order:
     *    - A1_ADJ_GAIN
     *    - A1_ADJ_OFFSET
     *    - A1_OFFSET_GAIN
     *    - A1_OFFSET_OFFSET
     *    - A2_GAIN
     *    - A2_OFFSET
     *    - A3_GAIN
     *    - A3_OFFSET
     */
    ITM3903C_HW_ANALOG_IF_GET_DAC_CAL_DATA,
        /**
     * Command
     *  -------------------------------------------
     *  | GET CAL DATA (uint32) | DATA (8 floats) |
     *  -------------------------------------------
     * 
     * Response
     *  -----------------------
     *  | CAL DATA (8 floats) |
     *  -----------------------
     * 
     * Notes
     *  - Calibration data is returned:
     *    - I Gain
     *    - I Offset
     *    - V Gain
     *    - V Offset
     */
    ITM3903C_HW_ANALOG_IF_GET_ALPHA_VALUES,
        /**
     * Command
     *  -------------------------------------------
     *  | SET CAL DATA (uint32) | DATA (8 floats) |
     *  -------------------------------------------
     * 
     * Response
     * No response
     * 
     * Notes
     *  - Calibration data entered:
     *    - I Gain
     *    - I Offset
     *    - V Gain
     *    - V Offset
     */
    ITM3903C_HW_ANALOG_IF_SET_ADC_CAL_DATA,

    /**
     * Command
     *  -------------------------------------------
     *  | GET CAL DATA (uint32) | DATA (8 floats) |
     *  -------------------------------------------
     * 
     * Response
     *  -----------------------
     *  | CAL DATA (8 floats) |
     *  -----------------------
     * 
     * Notes
     *  - Calibration data is returned:
     *    - I Gain
     *    - I Offset
     *    - V Gain
     *    - V Offset
     */
    ITM3903C_HW_ANALOG_IF_GET_ADC_CAL_DATA,
        /**
     * Command
     *  -------------------------------------------
     *  | GET CAL DATA (uint32) | DATA (8 floats) |
     *  -------------------------------------------
     * 
     * Response
     *  -----------------------
     *  | CAL DATA (8 floats) |
     *  -----------------------
     * 
     * Notes
     *  - Calibration data is returned:
     *    - I Gain
     *    - I Offset
     *    - V Gain
     *    - V Offset
     */
    ITM3903C_HW_ANALOG_IF_SET_ALPHA_VALUES,



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
