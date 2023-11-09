/*
 * cukHwWin.c
 *
 *  Created on: 06.10.2023
 *      Author: mguerreiro
 */


//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "cukHwOpil.h"

#include "cukOpil.h"

#include "cukConfig.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{

    uint32_t status;

}cukHwControl_t;
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static cukHwControl_t hwControl = {.status = 0};
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t cukHwOpilInitialize(void){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t cukHwOpilStatus(void){

    return hwControl.status;
}
//-----------------------------------------------------------------------------
void cukHwOpilStatusClear(void){

    hwControl.status = 0;
}
//-----------------------------------------------------------------------------
void cukHwOpilSetPwmReset(uint32_t reset){

}
//-----------------------------------------------------------------------------
uint32_t cukHwOpilGetPwmReset(void){

    return 0;
}
//-----------------------------------------------------------------------------
void cukHwOpilSetPwmOutputEnable(uint32_t enable){

}
//-----------------------------------------------------------------------------
uint32_t cukHwOpilGetPwmOutputEnable(void){

    return 0;
}
//-----------------------------------------------------------------------------
void cukHwOpilSetPwmOvfTriggerEnable(uint32_t enable){

}
//-----------------------------------------------------------------------------
uint32_t cukHwOpilGetPwmOvfTriggerEnable(void){

    return 0;
}
//-----------------------------------------------------------------------------
void cukHwOpilSetPwmFrequency(uint32_t freq){

}
//-----------------------------------------------------------------------------
uint32_t cukHwOpilGetPwmFrequency(void){

    return 100000;
}
//-----------------------------------------------------------------------------
void cukHwOpilSetPwmDuty(float duty){

    cukOpilSetPwmDuty(duty);
}
//-----------------------------------------------------------------------------
float cukHwOpilGetPwmDuty(void){

    return cukOpilGetPwmDuty();
}
//-----------------------------------------------------------------------------
void cukHwOpilSetPwmDeadTime(float deadtime){

}
//-----------------------------------------------------------------------------
float cukHwOpilGetPwmDeadTime(void){

    return 0.0f;
}
//-----------------------------------------------------------------------------
void cukHwOpilSetAdcEnable(uint32_t enable){

}
//-----------------------------------------------------------------------------
uint32_t cukHwOpilGetAdcEnable(void){

    return 0;
}
//-----------------------------------------------------------------------------
void cukHwOpilSetAdcManualTrigger(uint32_t trigger){

}
//-----------------------------------------------------------------------------
uint32_t cukHwOpilGetAdcManualTrigger(void){

    return 0;
}
//-----------------------------------------------------------------------------
void cukHwOpilSetAdcInterruptEnable(uint32_t enable){

}
//-----------------------------------------------------------------------------
uint32_t cukHwOpilGetAdcInterruptEnable(void){

    return 0;
}
//-----------------------------------------------------------------------------
void cukHwOpilSetAdcSpiFreq(uint32_t freq){

}
//-----------------------------------------------------------------------------
uint32_t cukHwOpilGetAdcSpiFreq(void){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t cukHwOpilGetMeasurements(void *meas){

    int32_t meassize;

    cukConfigMeasurements_t *cukmeas;

    cukmeas = (cukConfigMeasurements_t *)meas;

    meassize = cukOpilGetMeasurements(meas);

    /* Protection */
    if( (cukmeas->i_i > CUK_CONFIG_I_PRIM_LIM) || (cukmeas->i_1 > CUK_CONFIG_I_PRIM_LIM) ) hwControl.status = 1;
    if( (cukmeas->i_i < -CUK_CONFIG_I_PRIM_LIM) || (cukmeas->i_1 < -CUK_CONFIG_I_PRIM_LIM) ) hwControl.status = 1;

    if( (cukmeas->v_in > CUK_CONFIG_V_PRIM_LIM) || (cukmeas->v_dc > CUK_CONFIG_V_PRIM_LIM) ) hwControl.status = 1;

    if( (cukmeas->i_o > CUK_CONFIG_I_SEC_LIM) || (cukmeas->i_2 > CUK_CONFIG_I_SEC_LIM) ) hwControl.status = 1;
    if( (cukmeas->i_o < -CUK_CONFIG_I_SEC_LIM) || (cukmeas->i_2 < -CUK_CONFIG_I_SEC_LIM) ) hwControl.status = 1;

    if( (cukmeas->v_out > CUK_CONFIG_V_SEC_LIM) || (cukmeas->v_dc_out > CUK_CONFIG_V_SEC_LIM) ) hwControl.status = 1;

    if( hwControl.status != 0 ){
        cukOpilSetPwmDuty(0.0f);
        return -1;
    }
    else
        return meassize;
}
//-----------------------------------------------------------------------------
int32_t cukHwOpilApplyOutputs(void *outputs, int32_t size){

    return cukOpilUpdateControl(outputs, size);
}
//-----------------------------------------------------------------------------
void cukHwOpilDisable(void){

}
//-----------------------------------------------------------------------------
void cukHwOpilEnable(void){

}
//-----------------------------------------------------------------------------
void cukHwOpilControllerDisable(void){

}
//-----------------------------------------------------------------------------
void cukHwOpilControllerEnable(void){

}
//-----------------------------------------------------------------------------
void cukHwOpilSetLoadSwitch(uint32_t state){

    float statef = 0.0f;

    if( state ) statef = 1.0f;

    cukOpilSetLoadSwitch(statef);
}
//-----------------------------------------------------------------------------
uint32_t cukHwOpilGetLoadSwitch(void){

    uint32_t state = 0;
    float statef;

    statef = cukOpilGetLoadSwitch();

    if( statef ) state = 1;

    return state;
}
//-----------------------------------------------------------------------------
void cukHwOpilSetOutputSwitch(uint32_t state){

    float statef = 0.0f;

    if( state ) statef = 1.0f;

    cukOpilSetOutputSwitch(statef);
}
//-----------------------------------------------------------------------------
uint32_t cukHwOpilGetOutputSwitch(void){

    uint32_t state = 0;
    float statef;

    statef = cukOpilGetOutputSwitch();

    if( statef ) state = 1;

    return state;
}
//-----------------------------------------------------------------------------
void cukHwOpilSetMeasGains(cukConfigMeasGains_t *gains){


}
//-----------------------------------------------------------------------------
uint32_t cukHwOpilGetMeasGains(cukConfigMeasGains_t *gains){

    return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

