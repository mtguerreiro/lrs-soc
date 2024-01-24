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

    float v_dc_in;
    float v_dc_out;
    float v_out;
    
    float i_l;
    float i_l_avg;
    float i_o;
}boostConfigMeasurements_t;

typedef struct{
    float u;
}boostConfigControl_t;

typedef struct{
    float v_o;
}boostConfigReferences_t;

typedef struct{

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

}boostConfigMeasGains_t;

#define BOOST_CONFIG_V_DC_IN_GAIN      (3.3f * (16.0f * 1.5f / 1.6416f) / 4095.0f)
#define BOOST_CONFIG_V_DC_IN_OFFS      (0.0f)

#define BOOST_CONFIG_V_DC_OUT_GAIN     (3.3f * (5.83f / 0.6f) / 4095.0f)
#define BOOST_CONFIG_V_DC_OUT_OFFS     (0.0f)

#define BOOST_CONFIG_V_OUT_GAIN        (3.3f * (5.83f / 0.5927f) / 4095.0f)
#define BOOST_CONFIG_V_OUT_OFFS        (0.0f)

#define BOOST_CONFIG_IL_GAIN           (3.3f * (5.9f / 3.9f) / 4095.0f / 50e-3)
#define BOOST_CONFIG_IL_OFFS           (-(2.49f / 50e-3 + 1.4165f))

#define BOOST_CONFIG_IL_AVG_GAIN       (3.3f * (5.9f / 3.9f) / 4095.0f / 50e-3)
#define BOOST_CONFIG_IL_AVG_OFFS       (-(2.49f / 50e-3 + 0.8603f))

#define BOOST_CONFIG_IO_AVG_GAIN       (0.0f)
#define BOOST_CONFIG_IO_AVG_OFFS       (0.0f)

#define BOOST_CONFIG_L                 ((float)(47e-6))
#define BOOST_CONFIG_C                 ((float)(150e-6))

#define BOOST_CONFIG_I_LIM             (10.0f)
#define BOOST_CONFIG_V_LIM             (30.0f)

#define BOOST_CONFIG_SHUTDOWN_U_DEC     0.005
//=============================================================================

#endif /* BOOST_CONFIG_H_ */
