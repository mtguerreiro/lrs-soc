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

    float i_o_1;
    float i_l_1;
    float v_io_1;
    float v_dc_1;
    float v_c_1;

    float i_o_1_filt;
    float i_l_1_filt;
    float v_io_1_filt;
    float v_dc_1_filt;
    float v_c_1_filt;

    float i_o_2;
    float i_l_2;
    float v_io_2;
    float v_dc_2;
    float v_c_2;

    float i_o_2_filt;
    float i_l_2_filt;
    float v_io_2_filt;
    float v_dc_2_filt;
    float v_c_2_filt;
}cukConfigMeasurements_t;

typedef struct{
    float u;
}cukConfigControl_t;

//=============================================================================

#endif /* CUK_CONFIG_H_ */
