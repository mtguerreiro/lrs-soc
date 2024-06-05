/*
 * boostHwWin.c
 *
 *  Created on: 06.10.2023 
 *      Author: mguerreiro
 */


//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "boostHwOpil.h"

#include "boostOpil.h"

#include "boostConfig.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{

    uint32_t status;

    float alpha;

}boostHwControl_t;
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static boostHwControl_t hwControl = {.status = 0, .alpha = 0.2f};
static float i_i_filt = 0.0f, i_1_filt = 0.0f, i_o_filt = 0.0f, i_2_filt = 0.0f;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t boostHwOpilInitialize(void){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t boostHwOpilStatus(void){

    return hwControl.status;
}
//-----------------------------------------------------------------------------
void boostHwOpilStatusClear(void){

    hwControl.status = 0;
}
//-----------------------------------------------------------------------------
void boostHwOpilSetPwmReset(uint32_t reset){

}
//-----------------------------------------------------------------------------
uint32_t boostHwOpilGetPwmReset(void){

    return 0;
}
//-----------------------------------------------------------------------------
void boostHwOpilSetPwmOutputEnable(uint32_t enable){

}
//-----------------------------------------------------------------------------
uint32_t boostHwOpilGetPwmOutputEnable(void){

    return 0;
}
//-----------------------------------------------------------------------------
void boostHwOpilSetPwmOvfTriggerEnable(uint32_t enable){

}
//-----------------------------------------------------------------------------
uint32_t boostHwOpilGetPwmOvfTriggerEnable(void){

    return 0;
}
//-----------------------------------------------------------------------------
void boostHwOpilSetPwmInv(uint32_t enable){

}
//-----------------------------------------------------------------------------
uint32_t boostHwOpilGetPwmInv(void){

    return 0;
}
//-----------------------------------------------------------------------------
void boostHwOpilSetPwmFrequency(uint32_t freq){

}
//-----------------------------------------------------------------------------
uint32_t boostHwOpilGetPwmFrequency(void){

    return 100000;
}
//-----------------------------------------------------------------------------
void boostHwOpilSetPwmDuty(float duty){

    boostOpilSetPwmDuty(duty);
}
//-----------------------------------------------------------------------------
float boostHwOpilGetPwmDuty(void){

    return boostOpilGetPwmDuty();
}
//-----------------------------------------------------------------------------
void boostHwOpilSetPwmDeadTime(float deadtime){

}
//-----------------------------------------------------------------------------
float boostHwOpilGetPwmDeadTime(void){

    return 0.0f;
}
//-----------------------------------------------------------------------------
void boostHwOpilSetAdcEnable(uint32_t enable){

}
//-----------------------------------------------------------------------------
uint32_t boostHwOpilGetAdcEnable(void){

    return 0;
}
//-----------------------------------------------------------------------------
void boostHwOpilSetAdcManualTrigger(uint32_t trigger){

}
//-----------------------------------------------------------------------------
uint32_t boostHwOpilGetAdcManualTrigger(void){

    return 0;
}
//-----------------------------------------------------------------------------
void boostHwOpilSetAdcInterruptEnable(uint32_t enable){

}
//-----------------------------------------------------------------------------
uint32_t boostHwOpilGetAdcInterruptEnable(void){

    return 0;
}
//-----------------------------------------------------------------------------
void boostHwOpilSetAdcCompReset(uint32_t enable){

}
//-----------------------------------------------------------------------------
uint32_t boostHwOpilGetAdcCompReset(void){

    return 0;
}
//-----------------------------------------------------------------------------
void boostHwOpilSetAdcDoneIntFactor(uint8_t factor){

}
//-----------------------------------------------------------------------------
uint32_t boostHwOpilGetAdcDoneIntFactor(void){

    return 0;
}

//-----------------------------------------------------------------------------
void boostHwOpilSetAdcSpiFreq(uint32_t freq){

}
//-----------------------------------------------------------------------------
uint32_t boostHwOpilGetAdcSpiFreq(void){

    return 0;
}
//-----------------------------------------------------------------------------
void boostHwOpilSetAdcCompEnable(uint32_t freq){

}
//-----------------------------------------------------------------------------
uint32_t boostHwOpilGetAdcCompEnable(void){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t boostHwOpilGetMeasurements(void *meas){

    int32_t meassize;

    boostConfigMeasurements_t *boostmeas;

    boostmeas = (boostConfigMeasurements_t *)meas;

    meassize = boostOpilGetMeasurements(meas);

    /* Protection */
    if( (boostmeas->i_l > BOOST_CONFIG_I_LIM) || (boostmeas->i_o > BOOST_CONFIG_I_LIM) ) hwControl.status = 1;
       if( (boostmeas->i_l < -BOOST_CONFIG_I_LIM) || (boostmeas->i_o < -BOOST_CONFIG_I_LIM) ) hwControl.status = 1;

       if( (boostmeas->v_dc_in > BOOST_CONFIG_V_LIM) || (boostmeas->v_dc_out > BOOST_CONFIG_V_LIM) || (boostmeas->v_out > BOOST_CONFIG_V_LIM) ) hwControl.status = 1;

       if( hwControl.status != 0 ){
           //boostHwSetPwmOutputEnable(0);
//           boostHwShutDown();
           return -1;
       }
       else
           return sizeof(boostConfigMeasurements_t);
}
//-----------------------------------------------------------------------------
int32_t boostHwOpilApplyOutputs(void *outputs, int32_t size){

    return boostOpilUpdateControl(outputs, size);
}
//-----------------------------------------------------------------------------
void boostHwOpilDisable(void){

}
//-----------------------------------------------------------------------------
void boostHwOpilEnable(void){

}
//-----------------------------------------------------------------------------
void boostHwOpilControllerDisable(void){

}
//-----------------------------------------------------------------------------
void boostHwOpilControllerEnable(void){

}
//-----------------------------------------------------------------------------
void boostHwOpilSetLoadSwitch(uint32_t state){

    float statef = 0.0f;

    if( state ) statef = 1.0f;

    boostOpilSetLoadSwitch(statef);
}
//-----------------------------------------------------------------------------
uint32_t boostHwOpilGetLoadSwitch(void){

    uint32_t state = 0;
    float statef;

    statef = boostOpilGetLoadSwitch();

    if( statef ) state = 1;

    return state;
}
//-----------------------------------------------------------------------------
void boostHwOpilSetOutputSwitch(uint32_t state){

    float statef = 0.0f;

    if( state ) statef = 1.0f;

    boostOpilSetOutputSwitch(statef);
}
//-----------------------------------------------------------------------------
uint32_t boostHwOpilGetOutputSwitch(void){

    uint32_t state = 0;
    float statef;

    statef = boostOpilGetOutputSwitch();

    if( statef ) state = 1;

    return state;
}
//-----------------------------------------------------------------------------
void boostHwOpilSetMeasGains(boostConfigMeasGains_t *gains){


}
//-----------------------------------------------------------------------------
uint32_t boostHwOpilGetMeasGains(boostConfigMeasGains_t *gains){

    return 0;
}
//-----------------------------------------------------------------------------
void boostHwOpilSetFilterCoef(float alpha){

    hwControl.alpha = alpha;
}
//-----------------------------------------------------------------------------
float boostHwOpilGetFilterCoef(void){

    return hwControl.alpha;
}
//-----------------------------------------------------------------------------
float boostHwOpilExpMovAvg(float sample, float average){

    return hwControl.alpha * sample + (1.0f - hwControl.alpha) * average;
}
//-----------------------------------------------------------------------------
void boostHwOpilShutDown(void){

    float u;

    u = boostHwOpilGetPwmDuty();

    u = u - BOOST_CONFIG_SHUTDOWN_U_DEC;
    if( u < 0.0f ) u = 0.0f;

    boostHwOpilSetPwmDuty(u);
}
//-----------------------------------------------------------------------------
//=============================================================================

