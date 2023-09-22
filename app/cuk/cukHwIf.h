/*
 * cukHwIf.h
 *
 *  Created on: 31 de ago de 2023
 *      Author: marco
 */

#ifndef CUK_HW_IF_H_
#define CUK_HW_IF_H_

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
     *  -------------------------------------------
     *  | SET PWM RESET (uint32) | RESET (uint32) |
     *  -------------------------------------------
     *
     * Response
     * No response
     *
     * Notes
     * - The value of RESET should be either 0 or 1. A value of 1 places the
     *   PWM under reset (the PWM becomes inactive). A value of 0 releases the
     *   PWM for operation.
     */
    CUK_HW_IF_SET_PWM_RESET,

    /**
     * Command
     *  --------------------------
     *  | GET PWM RESET (uint32) |
     *  --------------------------
     *
     * Response
     *  ------------------
     *  | RESET (uint32) |
     *  ------------------
     */
    CUK_HW_IF_GET_PWM_RESET,

    /**
     * Command
     *  ----------------------------------------------------
     *  | SET PWM OUTPUT ENABLE (uint32) | ENABLE (uint32) |
     *  ----------------------------------------------------
     *
     * Response
     * No response
     *
     * Notes
     * - ENABLE should be either 0 or 1. A value of 1 enables the PWM signals
     *   at the GPIOs, and the state of the GPIOs depends on the PWM signals.
     *   A value of 0 disables the PWM signals and the GPIOs are set to 0.
     */
    CUK_HW_IF_SET_PWM_OUTPUT_ENABLE,

    /**
     * Command
     *  ----------------------------------
     *  | GET PWM OUTPUT ENABLE (uint32) |
     *  ----------------------------------
     *
     * Response
     *  --------------------------
     *  | OUTPUT ENABLE (uint32) |
     *  --------------------------
     */
    CUK_HW_IF_GET_PWM_OUTPUT_ENABLE,

    /**
     * Command
     *  ---------------------------------------------------------
     *  | SET PWM OVF TRIGGER ENABLE (uint32) | ENABLE (uint32) |
     *  ---------------------------------------------------------
     *
     * Response
     * No response
     *
     * Notes
     * - ENABLE should be either 0 or 1. A value of 1 enables generation of an
     *   overflow signal, generated when the PWM counter reaches the maximum
     *   value. A value of 0 disables generation of the overflow signal. This
     *   signal is used to trigger the ADC. Thus, if disabled, no ADC
     *   conversions will be triggered, even if the ADC is enabled.
     */
    CUK_HW_IF_SET_PWM_OVF_TRIGGER_ENABLE,

    /**
     * Command
     *  ---------------------------------------
     *  | GET PWM OVF TRIGGER ENABLE (uint32) |
     *  ---------------------------------------
     *
     * Response
     *  -------------------------------
     *  | OVF TRIGGER ENABLE (uint32) |
     *  -------------------------------
     */
    CUK_HW_IF_GET_PWM_OVF_TRIGGER_ENABLE,

    /**
     * Command
     *  ---------------------------------------------------
     *  | SET PWM FREQUENCY (uint32) | FREQUENCY (uint32) |
     *  ---------------------------------------------------
     *
     * Response
     * No response
     *
     * Notes
     * - FREQUENCY is given in Hz. Note that it may not be possible to generate
     *   arbitrary frequencies. Internally, the PWM period is computed by
     *   using the following relation:
     *
     *      PERIOD = PWM_CLOCK / (2 * FREQUENCY)
     *
     *   PERIOD must be an integer value and is always truncated. Thus, it is
     *   not possible to generate arbitrary PWM frequencies.
     */
    CUK_HW_IF_SET_PWM_FREQ,

    /**
     * Command
     *  ------------------------------
     *  | GET PWM FREQUENCY (uint32) |
     *  ------------------------------
     *
     * Response
     *  ----------------------
     *  | FREQUENCY (uint32) |
     *  ----------------------
     *
     * Notes
     * - FREQUENCY is given in Hz.
     */
    CUK_HW_IF_GET_PWM_FREQ,

    /**
     * Command
     *  ----------------------------------------
     *  | SET PWM DUTY (uint32) | DUTY (float) |
     *  ----------------------------------------
     *
     * Response
     * No response
     *
     * Notes
     * - DUTY is given as a floating point value between 0 and 1. Internally,
     *   the PWM counter is set according to the following relation:
     *
     *          COUNTER = DUTY * PWM_PERIOD
     *
     *   COUNTER must be a integer, and is truncated, which limits the
     *   resolution of the actual duty cycle .
     */
    CUK_HW_IF_SET_PWM_DUTY,

    /**
     * Command
     *  -------------------------
     *  | GET PWM DUTY (uint32) |
     *  -------------------------
     *
     * Response
     *  ----------------
     *  | DUTY (float) |
     *  ----------------
     *
     * Notes
     * - Duty-cycle is given as a value between 0 and 1.
     */
    CUK_HW_IF_GET_PWM_DUTY,

    /**
     * Command
     *  ---------------------------------------------------
     *  | SET PWM DEAD TIME (uint32) | DEAD TIME (uint32) |
     *  ---------------------------------------------------
     *
     * Response
     * No response
     *
     * Notes
     * - DEAD TIME is given in nanoseconds, as a floating point value.
     *   Internally, the PWM counter is set according to the following
     *   relation:
     *
     *          COUNTER = DEADTIME * PWM_CLOCK
     *
     *   COUNTER must be a integer, and is truncated, which limits the
     *   resolution of the dead time. Dead time resolution can be determined as
     *   multiples of the clock period. For instance, if PWM_CLOCK is 100 MHz,
     *   resulting in a period of 10 ns, the dead time can only be multiples of
     *   10 ns.
     */
    CUK_HW_IF_SET_PWM_DEAD_TIME,

    /**
     * Command
     *  -----------------------------
     *  | GET PWM DEAD TIME(uint32) |
     *  -----------------------------
     *
     * Response
     *  ---------------------
     *  | DEAD TIME (float) |
     *  ---------------------
     *
     * Notes
     * - PWM dead time, in nanoseconds.
     */
    CUK_HW_IF_GET_PWM_DEAD_TIME,

    /**
     * Command
     *  ---------------------------------------------
     *  | SET ADC ENABLE (uint32) | ENABLE (uint32) |
     *  ---------------------------------------------
     *
     * Response
     * No response
     *
     * Notes
     * - The value of ENABLE should be either 0 or 1. A value of 1 disables the
     *   ADC, and a value of 1 enables it. However, a conversion only takes
     *   place when the ADC is triggered, either by the PWM overflow signal, or
     *   by a manual trigger.
     */
    CUK_HW_IF_SET_ADC_ENABLE,

    /**
     * Command
     *  ---------------------------
     *  | GET ADC ENABLE (uint32) |
     *  ---------------------------
     *
     * Response
     *  -------------------
     *  | ENABLE (uint32) |
     *  -------------------
     *
     * Notes
     * - DUTY is given as a value between 0 and 1.
     */
    CUK_HW_IF_GET_ADC_ENABLE,

    /**
     * Command
     *  ------------------------------------------------------
     *  | SET ADC MANUAL TRIGGER (uint32) | TRIGGER (uint32) |
     *  ------------------------------------------------------
     *
     * Response
     * No response
     *
     * Notes
     * - TRIGGER should be either 0 or 1.
     * - Setting the ADC manual trigger will trigger one conversion (if the
     *   ADC is enabled). TRIGGER must be cleared, and set again for another
     *   conversion. If TRIGGER is not cleared, no further conversions take
     *   place, even if the PWM overflow signal is enabled.
     */
    CUK_HW_IF_SET_ADC_MANUAL_TRIG,

    /**
     * Command
     *  -----------------------------------
     *  | GET ADC MANUAL TRIGGER (uint32) |
     *  -----------------------------------
     *
     * Response
     *  --------------------
     *  | TRIGGER (uint32) |
     *  --------------------
     */
    CUK_HW_IF_GET_ADC_MANUAL_TRIG,

    /**
     * Command
     *  -------------------------------------------------------
     *  | SET ADC INTERRUPT ENABLE (uint32) | ENABLE (uint32) |
     *  -------------------------------------------------------
     *
     * Response
     * No response
     *
     * Notes
     * - ENABLE should be either 0 or 1. A value of 1 enables generation of an
     *   interrupt signal, and a value of 0 disables it.
     */
    CUK_HW_IF_SET_ADC_INT_ENABLE,

    /**
     * Command
     *  -------------------------------------
     *  | GET ADC INTERRUPT ENABLE (uint32) |
     *  -------------------------------------
     *
     * Response
     *  -------------------
     *  | ENABLE (uint32) |
     *  -------------------
     */
    CUK_HW_IF_GET_ADC_INT_ENABLE,

    /**
     * Command
     *  -------------------------------------------------------
     *  | SET ADC SPI FREQUENCY (uint32) | FREQUENCY (uint32) |
     *  -------------------------------------------------------
     *
     * Response
     * No response
     *
     * Notes
     * - FREQUENCY is given in Hz. Internally, a counter that generates the
     *   SPI clock signals is computed by:
     *
     *          COUNTER = ADC_CLOCK / (2 * FREQUENCY)
     *
     *   COUNTER is truncated; therefore, arbitrary SPI frequencies cannot be
     *   generated.
     */
    CUK_HW_IF_SET_ADC_SPI_FREQ,

    /**
     * Command
     *  ----------------------------------
     *  | GET ADC SPI FREQUENCY (uint32) |
     *  ----------------------------------
     *
     * Response
     *  ----------------------
     *  | FREQUENCY (uint32) |
     *  ----------------------
     *
     * Notes
     * - FREQUENCY is given in Hz.
     */
    CUK_HW_IF_GET_ADC_SPI_FREQ,

    CUK_HW_IF_END
}cukHwIfCommands_t;

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t cukHwIfInitialize(void);
//-----------------------------------------------------------------------------
int32_t cukHwIf(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* CUK_HW_IF_H_ */
