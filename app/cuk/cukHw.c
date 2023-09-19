/*
 * cukHw.c
 *
 */

#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "cukHw.h"

#include "xparameters.h"
#include "zynqAxiPwm.h"
#include "zynqAxiAdc.h"

#include "cukConfig.h"

#include "zynqConfig.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define CUK_HW_CONFIG_PWM_FREQ_HZ          ((uint32_t) 100000 )
#define CUK_HW_CONFIG_PWM_DEAD_TIME_NS     ((float) 100e-9 )
#define CUK_HW_CONFIG_PWM_BASE              XPAR_AXI_PWM_0_S00_AXI_BASEADDR
#define CUK_HW_CONFIG_ADC_BASE              XPAR_ADC_PSCTL_0_S00_AXI_BASEADDR

#define CUK_HW_CONFIG_IRQ_PL_CPU1           ZYNQ_CONFIG_IRQ_PL_TO_CPU1
#define CUK_HW_CONFIG_ADC_BUFFER            ZYNQ_CONFIG_MEM_PL_TO_CPU1_ADR

/* PWM peripheral clock, in Hz */
#define CUK_HW_PWM_CLK                      100000000
#define CUK_HW_ADC_CLK                      100000000

typedef struct{

    uint32_t pwmPeriod;

    cukConfigMeasurements_t meas;
    cukConfigControlSignals_t control;
}cukHwControl_t;
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static void cukHwInitializeAdc(void *intc, cukHwAdcIrqHandle_t irqhandle);
static void cukHwInitializePwm(void);
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
cukHwControl_t hwControl = {.pwmPeriod = 0};
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t cukHwInitialize(cukHwInitConfig_t *config){

    cukHwInitializeAdc(config->intc, config->irqhandle);
    cukHwInitializePwm();

    return 0;
}
//-----------------------------------------------------------------------------
int32_t cukHwStatus(void){

    return 0;
}
//-----------------------------------------------------------------------------
void cukHwSetPwmReset(uint32_t reset){

    zynqAxiPwmResetWrite(CUK_HW_CONFIG_PWM_BASE, reset);
}
//-----------------------------------------------------------------------------
void cukHwSetPwmOutputEnable(uint32_t enable){

    zynqAxiPwmOutputEnableWrite(CUK_HW_CONFIG_PWM_BASE, enable);
}
//-----------------------------------------------------------------------------
void cukHwSetPwmOvfTriggerEnable(uint32_t enable){

    zynqAxiPwmOvfTriggerEnableWrite(CUK_HW_CONFIG_PWM_BASE, enable);
}
//-----------------------------------------------------------------------------
void cukHwSetPwmFrequency(uint32_t freq){

    uint32_t period;

    period = CUK_HW_PWM_CLK / (freq << 1);

    hwControl.pwmPeriod = period;

    zynqAxiPwmPeriodWrite(CUK_HW_CONFIG_PWM_BASE, period);
}
//-----------------------------------------------------------------------------
void cukHwSetPwmDuty(float duty){

    uint32_t dutyInt;

    dutyInt = (uint32_t)( duty * ((float)hwControl.pwmPeriod ) );

    zynqAxiPwmDutyWrite(CUK_HW_CONFIG_PWM_BASE, dutyInt);
}
//-----------------------------------------------------------------------------
void cukHwSetPwmDeadTime(float deadtime){

    uint32_t deadtimeInt;

    deadtimeInt = (uint32_t)( deadtime * ((float)CUK_HW_PWM_CLK) );

    zynqAxiPwmDeadTimeWrite(CUK_HW_CONFIG_PWM_BASE, deadtimeInt);
}
//-----------------------------------------------------------------------------
void cukHwSetAdcEnable(uint32_t enable){

    zynqAxiAdcEnableWrite(CUK_HW_CONFIG_ADC_BASE, enable);
}
//-----------------------------------------------------------------------------
void cukHwSetAdcManualTrigger(uint32_t trigger){

    zynqAxiAdcManualTriggerWrite(CUK_HW_CONFIG_ADC_BASE, trigger);
}
//-----------------------------------------------------------------------------
void cukHwSetAdcInterruptEnable(uint32_t enable){

    zynqAxiAdcInterruptEnableWrite(CUK_HW_CONFIG_ADC_BASE, enable);
}
//-----------------------------------------------------------------------------
void cukHwSetAdcSpiFreq(uint32_t freq){

    uint32_t clkdiv;

    clkdiv = CUK_HW_ADC_CLK / (freq << 1);

    zynqAxiAdcSpiClkDivWrite(CUK_HW_CONFIG_ADC_BASE, clkdiv);
}
//-----------------------------------------------------------------------------
int32_t cukHwGetMeasurements(void *meas){

    uint16_t *dst;
    uint16_t *src;
    uint32_t k;

    src = (uint16_t *)CUK_HW_CONFIG_ADC_BUFFER;
    dst = (uint16_t *)meas;

    src++;

    for(k = 0; k < 5; k++){
        *dst++ = *src++;
    }

    return 5;
}
//-----------------------------------------------------------------------------
int32_t cukHwProcInputs(void *inputs, void *procinputs, int32_t size){

    uint16_t *src;
    cukConfigMeasurements_t *meas;

    src = (uint16_t *)inputs;
    meas = (cukConfigMeasurements_t *)procinputs;

    meas->i_o =  ((float)(*src++)) / 4095.0f * 3.3f;
    meas->i_L =  ((float)(*src++)) / 4095.0f * 3.3f;
    meas->v_io = ((float)(*src++)) / 4095.0f * 3.3f;
    meas->v_dc = ((float)(*src++)) / 4095.0f * 3.3f;
    meas->v_c =  ((float)(*src++)) / 4095.0f * 3.3f;

    return sizeof(cukConfigMeasurements_t);
}
//-----------------------------------------------------------------------------
int32_t cukHwProcOutputs(void *outputs, void *procoutputs, int32_t size){

    uint8_t *dst;
    uint8_t *src;
    uint32_t k;

    src = (uint8_t *)outputs;
    dst = (uint8_t *)procoutputs;

    for(k = 0; k < size; k++){
        *dst++ = *src++;
    }

    return size;
}
//-----------------------------------------------------------------------------
int32_t cukHwApplyOutputs(void *outputs, int32_t size){

    cukConfigControlSignals_t *control;

    control = (cukConfigControlSignals_t *)outputs;

    return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static void cukHwInitializeAdc(void *intc, cukHwAdcIrqHandle_t irqhandle){

    zynqAxiAdcEnableWrite(CUK_HW_CONFIG_ADC_BASE, 0);

    zynqAxiAdcInterruptEnableWrite(CUK_HW_CONFIG_ADC_BASE, 1);

    zynqAxiAdcBufferAddressWrite(CUK_HW_CONFIG_ADC_BASE, CUK_HW_CONFIG_ADC_BUFFER);

    zynqAxiAdcInterruptConfig(intc, CUK_HW_CONFIG_IRQ_PL_CPU1, irqhandle);
}
//-----------------------------------------------------------------------------
static void cukHwInitializePwm(void){

    cukHwSetPwmReset(1);

    cukHwSetPwmFrequency(CUK_HW_CONFIG_PWM_FREQ_HZ);
    cukHwSetPwmDuty(0.0f);
    cukHwSetPwmDeadTime(CUK_HW_CONFIG_PWM_DEAD_TIME_NS);

    cukHwSetPwmOvfTriggerEnable(1);
    cukHwSetPwmOutputEnable(1);

}
//-----------------------------------------------------------------------------
//=============================================================================
#endif /* SOC_CPU1 */
