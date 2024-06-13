/*
 * boostConfig.h
 *
 */

#ifndef BOOST_CONFIG_H_
#define BOOST_CONFIG_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{

    float i_o;
    float i_l;

    float v_dc_in;
    float v_in;
    float v_dc_out;
    float v_out;
}boostConfigMeasurements_t;

typedef struct{
    float u;
    float v_o_reference;
    float e;
    float e_dot;
    float e_reference;
    float i_o_filt;
    float rho;
    float control_s;

}boostConfigControl_t;


typedef struct{
    float v_o;
}boostConfigReferences_t;

typedef struct{

    float v_in_gain;
    float v_in_ofs;

    float v_dc_in_gain;
    float v_dc_in_ofs;

    float v_dc_out_gain;
    float v_dc_out_ofs;

    float v_out_gain;
    float v_out_ofs;
    
    float i_l_gain;
    float i_l_ofs;

    float i_l_avg_gain;
    float i_l_avg_ofs;

    float i_o_gain;
    float i_o_ofs;

    float unit;

}boostConfigMeasGains_t;


#define BOOST_CONFIG_V_IN_GAIN          (float)((0.008047f)) // still needs calibration
#define BOOST_CONFIG_V_IN_OFFS          (float)((0.002216f)) // still needs calibration

#define BOOST_CONFIG_V_DC_IN_GAIN       (float)((0.008047f))
#define BOOST_CONFIG_V_DC_IN_OFFS       (float)((0.002216f))

#define BOOST_CONFIG_V_DC_OUT_GAIN      (float)((0.01216f))
#define BOOST_CONFIG_V_DC_OUT_OFFS     (float)((0.004624f))

#define BOOST_CONFIG_V_OUT_GAIN         (float)((0.01209f))
#define BOOST_CONFIG_V_OUT_OFFS        (float)((0.0003478f))

#define BOOST_CONFIG_IL_GAIN            (float)((-0.02472f))
#define BOOST_CONFIG_IL_OFFS           (float)((50.44f))

#define BOOST_CONFIG_IL_AVG_GAIN        (float)((1.0f))
#define BOOST_CONFIG_IL_AVG_OFFS       (float)((0.0f))

#define BOOST_CONFIG_IO_AVG_GAIN        (float)((0.01177f))
#define BOOST_CONFIG_IO_AVG_OFFS       (float)((-24.19f))

#define BOOST_CONFIG_L                 ((float)(47e-6))
#define BOOST_CONFIG_C                 ((float)(156.6e-6))

#define BOOST_CONFIG_I_LIM             (8.0f)
#define BOOST_CONFIG_V_LIM             (30.0f)

#define BOOST_CONFIG_SHUTDOWN_U_DEC     0.005
//=============================================================================

#endif /* BOOST_CONFIG_H_ */
