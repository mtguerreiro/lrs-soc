/*
 * buckConfig.h
 *
 */

#ifndef BUCK_CONFIG_H_
#define BUCK_CONFIG_H_

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
    
    float v_dc_out;
    float v_out;

    float v_dc_in;
    float v_in;
    
}buckConfigMeasurements_t;

typedef struct{
    float u;
}buckConfigControl_t;

typedef struct{
    float v_o;
}buckConfigReferences_t;

typedef struct{

    float i_o_gain;
    float i_o_ofs;
    
    float i_l_gain;
    float i_l_ofs;

    float v_dc_out_gain;
    float v_dc_out_ofs;

    float v_out_gain;
    float v_out_ofs;

    float v_dc_in_gain;
    float v_dc_in_ofs;

    float v_in_gain;
    float v_in_ofs;
    
    float i_l_avg_gain;
    float i_l_avg_ofs;

}buckConfigMeasGains_t;

#define BUCK_CONFIG_IO_AVG_GAIN       (1.0f)
#define BUCK_CONFIG_IO_AVG_OFFS       (0.0f)

#define BUCK_CONFIG_IL_GAIN           (0.026517892498190067f)
#define BUCK_CONFIG_IL_OFFS           (-54.574335138449435f)

#define BUCK_CONFIG_V_DC_OUT_GAIN     (0.016063399457645268f)
#define BUCK_CONFIG_V_DC_OUT_OFFS     (-0.025311388604552464f)

#define BUCK_CONFIG_V_OUT_GAIN        (0.016064253912404878f)
#define BUCK_CONFIG_V_OUT_OFFS        (-0.01993611477240416f)

#define BUCK_CONFIG_V_DC_IN_GAIN      (0.016100261686656722f)
#define BUCK_CONFIG_V_DC_IN_OFFS      (0.021674403738864884f)

#define BUCK_CONFIG_V_IN_GAIN      (0.016067964024081928f)
#define BUCK_CONFIG_V_IN_OFFS      (-0.004404536840030456f)

#define BUCK_CONFIG_IL_AVG_GAIN       (1.0f)
#define BUCK_CONFIG_IL_AVG_OFFS       (0.0f)

#define BUCK_CONFIG_I_LIM             (10.0f)
#define BUCK_CONFIG_V_LIM             (30.0f)

#define BUCK_CONFIG_SHUTDOWN_U_DEC     0.005
//=============================================================================

#endif /* BUCK_CONFIG_H_ */
