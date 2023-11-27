/*
 * cukHw.h
 *
 */

#ifndef CUK_HW_H_
#define CUK_HW_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"

#include "cukConfig.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

typedef void (*cukHwAdcIrqHandle_t)(void *ref);

typedef struct{

    void *intc;
    cukHwAdcIrqHandle_t irqhandle;

}cukHwInitConfig_t;

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t cukHwInitialize(cukHwInitConfig_t *config);
//-----------------------------------------------------------------------------
int32_t cukHwStatus(void);
//-----------------------------------------------------------------------------
void cukHwStatusClear(void);
//-----------------------------------------------------------------------------
void cukHwSetPwmReset(uint32_t reset);
//-----------------------------------------------------------------------------
uint32_t cukHwGetPwmReset(void);
//-----------------------------------------------------------------------------
void cukHwSetPwmOutputEnable(uint32_t enable);
//-----------------------------------------------------------------------------
uint32_t cukHwGetPwmOutputEnable(void);
//-----------------------------------------------------------------------------
void cukHwSetPwmOvfTriggerEnable(uint32_t enable);
//-----------------------------------------------------------------------------
uint32_t cukHwGetPwmOvfTriggerEnable(void);
//-----------------------------------------------------------------------------
void cukHwSetPwmFrequency(uint32_t freq);
//-----------------------------------------------------------------------------
uint32_t cukHwGetPwmFrequency(void);
//-----------------------------------------------------------------------------
void cukHwSetPwmDuty(float duty);
//-----------------------------------------------------------------------------
float cukHwGetPwmDuty(void);
//-----------------------------------------------------------------------------
void cukHwSetPwmDeadTime(float deadtime);
//-----------------------------------------------------------------------------
float cukHwGetPwmDeadTime(void);
//-----------------------------------------------------------------------------
void cukHwSetAdcEnable(uint32_t enable);
//-----------------------------------------------------------------------------
uint32_t cukHwGetAdcEnable(void);
//-----------------------------------------------------------------------------
void cukHwSetAdcManualTrigger(uint32_t trigger);
//-----------------------------------------------------------------------------
uint32_t cukHwGetAdcManualTrigger(void);
//-----------------------------------------------------------------------------
void cukHwSetAdcInterruptEnable(uint32_t enable);
//-----------------------------------------------------------------------------
uint32_t cukHwGetAdcInterruptEnable(void);
//-----------------------------------------------------------------------------
void cukHwSetAdcSpiFreq(uint32_t freq);
//-----------------------------------------------------------------------------
uint32_t cukHwGetAdcSpiFreq(void);
//-----------------------------------------------------------------------------
int32_t cukHwGetMeasurements(void *meas);
//-----------------------------------------------------------------------------
int32_t cukHwApplyOutputs(void *outputs, int32_t size);
//-----------------------------------------------------------------------------
void cukHwDisable(void);
//-----------------------------------------------------------------------------
void cukHwEnable(void);
//-----------------------------------------------------------------------------
void cukHwControllerDisable(void);
//-----------------------------------------------------------------------------
void cukHwControllerEnable(void);
//-----------------------------------------------------------------------------
void cukHwSetLoadSwitch(uint32_t state);
//-----------------------------------------------------------------------------
uint32_t cukHwGetLoadSwitch(void);
//-----------------------------------------------------------------------------
void cukHwSetOutputSwitch(uint32_t state);
//-----------------------------------------------------------------------------
uint32_t cukHwGetOutputSwitch(void);
//-----------------------------------------------------------------------------
void cukHwSetMeasGains(cukConfigMeasGains_t *gains);
//-----------------------------------------------------------------------------
uint32_t cukHwGetMeasGains(cukConfigMeasGains_t *gains);
//-----------------------------------------------------------------------------
void cukHwSetFilterCoef(float alpha);
//-----------------------------------------------------------------------------
float cukHwGetFilterCoef(void);
//-----------------------------------------------------------------------------
void cukHwShutDown(void);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* CUK_HW_H_ */
