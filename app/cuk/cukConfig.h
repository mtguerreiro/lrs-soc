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
    
}cukConfigMeasurements_t;

typedef struct{
    float u;

    float sw_o;
    float sw_l;
}cukConfigControl_t;

typedef struct{
    float v_o;
}cukConfigReferences_t;

#define CUK_CONFIG_ADC_GAIN_INV             (5.0f / 4095.0f)

#define CUK_CONFIG_VSENS_GAIN_INV           (60.0f / 5.0f)

#define CUK_CONFIG_ISENS_ACS730_GAIN_INV    (20.0f)
#define CUK_CONFIG_ISENS_ACS730_OFFS        (2.5f)

#define CUK_CONFIG_ISENS_ACS712_GAIN_INV    (10.0f)
#define CUK_CONFIG_ISENS_ACS712_OFFS        (2.5f)

#define CUK_CONFIG_TF_N2N1                  (5.0f / 3.0f)
//=============================================================================

#endif /* CUK_CONFIG_H_ */
