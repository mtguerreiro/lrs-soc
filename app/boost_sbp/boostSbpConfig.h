
#ifndef BOOST_SBP_CONFIG_H_
#define BOOST_SBP_CONFIG_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{

    float vi;
    float vi_dc;

    float vo;
    float vo_dc;
    
    float ii;
    float il;
    float io;
    
    float th;
}boostSbpConfigMeasurements_t;

typedef struct{
    float u;
}boostSbpConfigControl_t;

typedef struct{
    float v_o;
}boostSbpConfigReferences_t;


#ifdef SOC_CPU1
#define BOOST_SBP_CONFIG_TRACE_ADDR             ZYNQ_CONFIG_MEM_TRACE_ADR
#define BOOST_SBP_CONFIG_TRACE_SIZE             ZYNQ_CONFIG_MEM_TRACE_SIZE_MAX
#endif

#define BOOST_SBP_CONFIG_TRACE_0_NAME_LEN       500
#define BOOST_SBP_CONFIG_TRACE_0_MAX_SIGNALS    40

#define BOOST_SBP_CONFIG_INPUT_BUF_SIZE         50
#define BOOST_SBP_CONFIG_OUTPUT_BUG_SIZE        20
#define BOOST_SBP_CONFIG_REFERENCE_BUF_SIZE     20

#define BOOST_SBP_CONFIG_CS_ID                  OCP_CS_1
#define BOOST_SBP_CONFIG_TRACE_ID               OCP_TRACE_1
//=============================================================================

#endif /* BOOST_SBP_CONFIG_H_ */
