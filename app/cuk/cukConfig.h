/*
 * cukConfig.h
 *
 *  Created on: 11.09.2023
 *      Author: marco
 */

#ifndef CUK_CONFIG_H_
#define CUK_CONFIG_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{

    //---------------------------------
    // Sensor-based measurements
    //---------------------------------
    float i_i;
    float i_1;
    float v_in;
    float v_dc;
    float v_1;

    float i_o;
    float i_2;
    float v_out;
    float v_dc_out;
    float v_2;
    //---------------------------------

    //---------------------------------
    // Software-based measurements
    //---------------------------------
    float i_i_filt;
    float i_1_filt;
    float v_in_filt;
    float v_dc_filt;
    float v_1_filt;

    float i_o_filt;
    float i_2_filt;
    float v_out_filt;
    float v_dc_out_filt;
    float v_2_filt;
    
    float p_in;
    float p_out;
    //---------------------------------

}cukConfigMeasurements_t;

typedef struct{
    float u;
}cukConfigControl_t;

typedef struct{
    float v_o;
}cukConfigReferences_t;


typedef struct{
    float i_i_gain;
    float i_i_ofs;

    float i_1_gain;
    float i_1_ofs;

    float v_in_gain;
    float v_in_ofs;

    float v_dc_gain;
    float v_dc_ofs;

    float v_1_gain;
    float v_1_ofs;

    float i_o_gain;
    float i_o_ofs;

    float i_2_gain;
    float i_2_ofs;

    float v_out_gain;
    float v_out_ofs;

    float v_dc_out_gain;
    float v_dc_out_ofs;

    float v_2_gain;
    float v_2_ofs;
}cukConfigMeasGains_t;

#define CUK_CONFIG_ADC_GAIN_INV             (float)((5.0f / 4095.0f))

#define CUK_CONFIG_VSENS_GAIN_INV           (float)((60.0f / 5.0f))

#define CUK_CONFIG_ISENS_ACS730_GAIN_INV    (float)((20.0f))
#define CUK_CONFIG_ISENS_ACS730_OFFS        (float)((2.5f))

#define CUK_CONFIG_ISENS_ACS712_GAIN_INV    (float)((10.0f))
#define CUK_CONFIG_ISENS_ACS712_OFFS        (float)((2.5f))

#define CUK_CONFIG_TF_N2N1                  (float)((5.0f / 3.0f))

#define CUK_CONFIG_L_IN                     ((float)(100e-6))
#define CUK_CONFIG_L_OUT                    ((float)(150e-6))
#define CUK_CONFIG_C_C                      ((float)(9.4e-6))
#define CUK_CONFIG_C_O                      ((float)(330e-6))

#define CUK_CONFIG_I_PRIM_LIM               (15.0f)
#define CUK_CONFIG_V_PRIM_LIM               (50.0f)
#define CUK_CONFIG_I_SEC_LIM                (15.0f)
#define CUK_CONFIG_V_SEC_LIM                (50.0f)

#define CUK_CONFIG_SHUTDOWN_U_DEC           (0.00025f)

//=============================================================================

#endif /* CUK_CONFIG_H_ */
