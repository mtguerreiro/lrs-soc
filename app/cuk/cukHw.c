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
#define CUK_HW_CONFIG_ADC_SPI_FREQ_HZ      ((uint32_t)10000000)
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
    cukConfigControl_t control;
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
uint32_t cukHwGetPwmReset(void){

    return zynqAxiPwmResetRead(CUK_HW_CONFIG_PWM_BASE);
}
//-----------------------------------------------------------------------------
void cukHwSetPwmOutputEnable(uint32_t enable){

    zynqAxiPwmOutputEnableWrite(CUK_HW_CONFIG_PWM_BASE, enable);
}
//-----------------------------------------------------------------------------
uint32_t cukHwGetPwmOutputEnable(void){

    return zynqAxiPwmOutputEnableRead(CUK_HW_CONFIG_PWM_BASE);
}
//-----------------------------------------------------------------------------
void cukHwSetPwmOvfTriggerEnable(uint32_t enable){

    zynqAxiPwmOvfTriggerEnableWrite(CUK_HW_CONFIG_PWM_BASE, enable);
}
//-----------------------------------------------------------------------------
uint32_t cukHwGetPwmOvfTriggerEnable(void){

    return zynqAxiPwmOvfTriggerEnableRead(CUK_HW_CONFIG_PWM_BASE);
}
//-----------------------------------------------------------------------------
void cukHwSetPwmFrequency(uint32_t freq){

    uint32_t period;

    period = CUK_HW_PWM_CLK / (freq << 1);

    hwControl.pwmPeriod = period;

    zynqAxiPwmPeriodWrite(CUK_HW_CONFIG_PWM_BASE, period);
}
//-----------------------------------------------------------------------------
uint32_t cukHwGetPwmFrequency(void){

    uint32_t freq, period;

    period = zynqAxiPwmPeriodRead(CUK_HW_CONFIG_PWM_BASE);

    freq = CUK_HW_PWM_CLK / (period << 1);

    return freq;
}
//-----------------------------------------------------------------------------
void cukHwSetPwmDuty(float duty){

    uint32_t dutyInt;

    dutyInt = (uint32_t)( duty * ((float)hwControl.pwmPeriod ) );

    zynqAxiPwmDutyWrite(CUK_HW_CONFIG_PWM_BASE, dutyInt);
}
//-----------------------------------------------------------------------------
float cukHwGetPwmDuty(void){

    uint32_t dutyInt;
    float duty;

    dutyInt = zynqAxiPwmDutyRead(CUK_HW_CONFIG_PWM_BASE);
    duty = ( (float)dutyInt ) / ( (float)hwControl.pwmPeriod );

    return duty;
}
//-----------------------------------------------------------------------------
void cukHwSetPwmDeadTime(float deadtime){

    uint32_t deadtimeInt;

    deadtimeInt = (uint32_t)( deadtime * ((float)CUK_HW_PWM_CLK) );

    zynqAxiPwmDeadTimeWrite(CUK_HW_CONFIG_PWM_BASE, deadtimeInt);
}
//-----------------------------------------------------------------------------
float cukHwGetPwmDeadTime(void){

    uint32_t deadtimeInt;
    float deadtime;

    deadtimeInt = zynqAxiPwmDeadTimeRead(CUK_HW_CONFIG_PWM_BASE);
    deadtime = ( (float)deadtimeInt ) / ( (float)CUK_HW_PWM_CLK );

    return deadtime;
}
//-----------------------------------------------------------------------------
void cukHwSetAdcEnable(uint32_t enable){

    zynqAxiAdcEnableWrite(CUK_HW_CONFIG_ADC_BASE, enable);
}
//-----------------------------------------------------------------------------
uint32_t cukHwGetAdcEnable(void){

    return zynqAxiAdcEnableRead(CUK_HW_CONFIG_ADC_BASE);
}
//-----------------------------------------------------------------------------
void cukHwSetAdcManualTrigger(uint32_t trigger){

    zynqAxiAdcManualTriggerWrite(CUK_HW_CONFIG_ADC_BASE, trigger);
}
//-----------------------------------------------------------------------------
uint32_t cukHwGetAdcManualTrigger(void){

    return zynqAxiAdcManualTriggerRead(CUK_HW_CONFIG_ADC_BASE);
}
//-----------------------------------------------------------------------------
void cukHwSetAdcInterruptEnable(uint32_t enable){

    zynqAxiAdcInterruptEnableWrite(CUK_HW_CONFIG_ADC_BASE, enable);
}
//-----------------------------------------------------------------------------
uint32_t cukHwGetAdcInterruptEnable(void){

    return zynqAxiAdcInterruptEnableRead(CUK_HW_CONFIG_ADC_BASE);
}
//-----------------------------------------------------------------------------
void cukHwSetAdcSpiFreq(uint32_t freq){

    uint32_t clkdiv;

    clkdiv = CUK_HW_ADC_CLK / (freq << 1);

    zynqAxiAdcSpiClkDivWrite(CUK_HW_CONFIG_ADC_BASE, clkdiv);
}
//-----------------------------------------------------------------------------
uint32_t cukHwGetAdcSpiFreq(void){

    uint32_t clkdiv, freq;

    clkdiv = zynqAxiAdcSpiClkDivRead(CUK_HW_CONFIG_ADC_BASE);

    freq = CUK_HW_ADC_CLK / (clkdiv << 1);

    return freq;
}
//-----------------------------------------------------------------------------
int32_t cukHwGetMeasurements(void *meas){

    cukConfigMeasurements_t *dst;
    uint16_t *src;

    src = (uint16_t *)CUK_HW_CONFIG_ADC_BUFFER;
    dst = (cukConfigMeasurements_t *)meas;

    /* Skips the first adc channel */
    src++;

    dst->i_o_1 =  ((float)(*src++)) / 4095.0f * 3.3f;
    dst->i_l_1 =  ((float)(*src++)) / 4095.0f * 3.3f;
    dst->v_io_1 = ((float)(*src++)) / 4095.0f * 3.3f;
    dst->v_dc_1 = ((float)(*src++)) / 4095.0f * 3.3f;
    dst->v_c_1 =  ((float)(*src++)) / 4095.0f * 3.3f;

    dst->i_o_1_filt =  0.0f;
    dst->i_l_1_filt =  0.0f;
    dst->v_io_1_filt = 0.0f;
    dst->v_dc_1_filt = 0.0f;
    dst->v_c_1_filt =  0.0f;

    dst->i_o_2 =  0.0f;
    dst->i_l_2 =  0.0f;
    dst->v_io_2 = 0.0f;
    dst->v_dc_2 = 0.0f;
    dst->v_c_2 =  0.0f;

    dst->i_o_2_filt =  0.0f;
    dst->i_l_2_filt =  0.0f;
    dst->v_io_2_filt = 0.0f;
    dst->v_dc_2_filt = 0.0f;
    dst->v_c_2_filt =  0.0f;

    return sizeof(cukConfigMeasurements_t);
}
//-----------------------------------------------------------------------------
int32_t cukHwApplyOutputs(void *outputs, int32_t size){

    cukConfigControl_t *control;

    control = (cukConfigControl_t *)outputs;

    //control->u = 0.5f;

    return 0;
}
//-----------------------------------------------------------------------------
void cukHwDisable(void){

    cukHwSetPwmDuty(0.0f);
    cukHwSetPwmOutputEnable(0);
    cukHwSetPwmOvfTriggerEnable(0);
}
//-----------------------------------------------------------------------------
void cukHwEnable(void){

    cukHwSetPwmDuty(0.0f);
    cukHwSetPwmOvfTriggerEnable(1);
    //cukHwSetPwmOutputEnable(1);
}
//-----------------------------------------------------------------------------
void cukHwControllerDisable(void){

    cukHwSetPwmOutputEnable(0);
}
//-----------------------------------------------------------------------------
void cukHwControllerEnable(void){

    cukHwSetPwmOutputEnable(1);
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static void cukHwInitializeAdc(void *intc, cukHwAdcIrqHandle_t irqhandle){

    uint32_t clkdiv;

    clkdiv = CUK_HW_ADC_CLK / (CUK_HW_CONFIG_ADC_SPI_FREQ_HZ << 1);

    zynqAxiAdcEnableWrite(CUK_HW_CONFIG_ADC_BASE, 0);

    zynqAxiAdcInterruptEnableWrite(CUK_HW_CONFIG_ADC_BASE, 1);

    zynqAxiAdcSpiClkDivWrite(CUK_HW_CONFIG_ADC_BASE, clkdiv);

    zynqAxiAdcBufferAddressWrite(CUK_HW_CONFIG_ADC_BASE, CUK_HW_CONFIG_ADC_BUFFER);

    zynqAxiAdcInterruptConfig(intc, CUK_HW_CONFIG_IRQ_PL_CPU1, irqhandle);

    zynqAxiAdcEnableWrite(CUK_HW_CONFIG_ADC_BASE, 1);
}
//-----------------------------------------------------------------------------
static void cukHwInitializePwm(void){

    cukHwSetPwmReset(1);

    cukHwSetPwmFrequency(CUK_HW_CONFIG_PWM_FREQ_HZ);
    cukHwSetPwmDuty(0.0f);
    cukHwSetPwmDeadTime(CUK_HW_CONFIG_PWM_DEAD_TIME_NS);

    cukHwSetPwmOvfTriggerEnable(0);
    cukHwSetPwmOutputEnable(0);

    cukHwSetPwmReset(0);
}
//-----------------------------------------------------------------------------
//=============================================================================
#endif /* SOC_CPU1 */
