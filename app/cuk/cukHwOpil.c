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

    return 0;
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

    return 0;
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

    return cukOpilGetMeasurements(meas);
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
//=============================================================================

