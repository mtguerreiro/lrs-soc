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
    float ipone;
    float iptwo;
    float ipthree;
    float ipfour;
    float ipfive;
}itm3903cConfigIPAddresses_t;

typedef struct{

    float i;
    float v;
    
}itm3903cConfigAlphaValues_t;

typedef struct{

    float i;
    float v;
    float t;

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

typedef struct{
    float a1_offset_gain;
    float a1_offset_offset;

    float a1_adj_gain;
    float a1_adj_offset;

    float a2_gain;
    float a2_offset;

    float a3_gain;
    float a3_offset;
}itm3903cConfigDacGains_t;

//=============================================================================

#endif /* ITM3903C_CONFIG_H_ */
