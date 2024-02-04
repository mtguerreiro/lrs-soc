/*
 * itm3903cConfig.h
 *
 */

#ifndef ITM3903C_CONFIG_H_
#define ITM3903C_CONFIG_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{

    float i;
    float v;

}itm3903cConfigMeasurements_t;

typedef struct{
    float u;
}itm3903cConfigControl_t;

typedef struct{
    float ref;
}itm3903cConfigReferences_t;


typedef struct{
    float i_gain;
    float i_ofs;

    float v_gain;
    float v_ofs;
}itm3903cConfigMeasGains_t;

#define ITM3903C_CONFIG_ADC_GAIN_INV        (float)((3.3f / 4095.0f))
//=============================================================================

#endif /* ITM3903C_CONFIG_H_ */
