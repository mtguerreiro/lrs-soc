/*
 * boostHw.c
 *
 */

#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "boostHw.h"

#include "xparameters.h"
#include "zynqAxiPwm.h"
#include "zynqAxiAdc.h"

#include "boostConfig.h"

#include "zynqConfig.h"

#include "xgpio.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define BOOST_HW_CONFIG_ADC_SPI_FREQ_HZ      ((uint32_t)16666666)
#define BOOST_HW_CONFIG_PWM_FREQ_HZ          ((uint32_t) 100000 )
#define BOOST_HW_CONFIG_PWM_DEAD_TIME_NS     ((float) 200e-9 )
#define BOOST_HW_CONFIG_PWM_BASE              XPAR_AXI_PWM_0_S00_AXI_BASEADDR
#define BOOST_HW_CONFIG_ADC_BASE              XPAR_ADC_PSCTL_0_S00_AXI_BASEADDR

#define BOOST_HW_CONFIG_IRQ_PL_CPU1           ZYNQ_CONFIG_IRQ_PL_TO_CPU1
#define BOOST_HW_CONFIG_ADC_BUFFER            ZYNQ_CONFIG_MEM_PL_TO_CPU1_ADR

#define BOOST_HW_CONFIG_GPIO_ID               XPAR_AXI_GPIO_0_DEVICE_ID
#define BOOST_HW_CONFIG_GPIO_CHANNEL          1
#define BOOST_HW_CONFIG_GPIO_MASK             0b11

#define BOOST_HW_CONFIG_GPIO_IN_RLY_OFFS       (0U)
#define BOOST_HW_CONFIG_GPIO_IN_RLY            (1 << BOOST_HW_CONFIG_GPIO_IN_RLY_OFFS)
#define BOOST_HW_CONFIG_GPIO_OUT_RLY_OFFS      (1U)
#define BOOST_HW_CONFIG_GPIO_OUT_RLY           (1 << BOOST_HW_CONFIG_GPIO_OUT_RLY_OFFS)

/* PWM peripheral clock, in Hz */
#define BOOST_HW_PWM_CLK                      100000000
#define BOOST_HW_ADC_CLK                      100000000

typedef struct{

    uint32_t status;

    uint32_t pwmPeriod;

    boostConfigMeasurements_t meas;
    boostConfigControl_t control;

    XGpio gpio;

    boostConfigMeasGains_t gains;

    float alpha;

}boostHwControl_t;
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static void boostHwInitializeAdc(void *intc, boostHwAdcIrqHandle_t irqhandle);
static void boostHwInitializePwm(void);
static void boostHwInitializeGpio(void);
static void boostHwInitializeMeasGains(void);
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static boostHwControl_t hwControl = {.pwmPeriod = 0, .status = 0, .alpha = 0.2f};
static float i_i_filt = 0.0f, i_1_filt = 0.0f, i_o_filt = 0.0f, i_2_filt = 0.0f;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t boostHwInitialize(boostHwInitConfig_t *config){

    boostHwInitializeAdc(config->intc, config->irqhandle);
    boostHwInitializePwm();
    boostHwInitializeGpio();
    boostHwInitializeMeasGains();

    return 0;
}
//-----------------------------------------------------------------------------
int32_t boostHwStatus(void){

    return hwControl.status;
}
//-----------------------------------------------------------------------------
void boostHwStatusClear(void){

    hwControl.status = 0;
}
//-----------------------------------------------------------------------------
void boostHwSetPwmReset(uint32_t reset){

    zynqAxiPwmResetWrite(BOOST_HW_CONFIG_PWM_BASE, reset);
}
//-----------------------------------------------------------------------------
uint32_t boostHwGetPwmReset(void){

    return zynqAxiPwmResetRead(BOOST_HW_CONFIG_PWM_BASE);
}
//-----------------------------------------------------------------------------
void boostHwSetPwmOutputEnable(uint32_t enable){

    zynqAxiPwmOutputEnableWrite(BOOST_HW_CONFIG_PWM_BASE, enable);
}
//-----------------------------------------------------------------------------
uint32_t boostHwGetPwmOutputEnable(void){

    return zynqAxiPwmOutputEnableRead(BOOST_HW_CONFIG_PWM_BASE);
}
//-----------------------------------------------------------------------------
void boostHwSetPwmOvfTriggerEnable(uint32_t enable){

    zynqAxiPwmOvfTriggerEnableWrite(BOOST_HW_CONFIG_PWM_BASE, enable);
}
//-----------------------------------------------------------------------------
uint32_t boostHwGetPwmOvfTriggerEnable(void){

    return zynqAxiPwmOvfTriggerEnableRead(BOOST_HW_CONFIG_PWM_BASE);
}
//-----------------------------------------------------------------------------
void boostHwSetPwmFrequency(uint32_t freq){

    uint32_t period;

    period = BOOST_HW_PWM_CLK / (freq << 1);

    hwControl.pwmPeriod = period;

    zynqAxiPwmPeriodWrite(BOOST_HW_CONFIG_PWM_BASE, period);
}
//-----------------------------------------------------------------------------
uint32_t boostHwGetPwmFrequency(void){

    uint32_t freq, period;

    period = zynqAxiPwmPeriodRead(BOOST_HW_CONFIG_PWM_BASE);

    freq = BOOST_HW_PWM_CLK / (period << 1);

    return freq;
}
//-----------------------------------------------------------------------------
void boostHwSetPwmDuty(float duty){

    uint32_t dutyInt;

    dutyInt = (uint32_t)( duty * ((float)hwControl.pwmPeriod ) );

    zynqAxiPwmDutyWrite(BOOST_HW_CONFIG_PWM_BASE, dutyInt);
}
//-----------------------------------------------------------------------------
float boostHwGetPwmDuty(void){

    uint32_t dutyInt;
    float duty;

    dutyInt = zynqAxiPwmDutyRead(BOOST_HW_CONFIG_PWM_BASE);
    duty = ( (float)dutyInt ) / ( (float)hwControl.pwmPeriod );

    return duty;
}
//-----------------------------------------------------------------------------
void boostHwSetPwmDeadTime(float deadtime){

    uint32_t deadtimeInt;

    deadtimeInt = (uint32_t)( deadtime * ((float)BOOST_HW_PWM_CLK) );

    zynqAxiPwmDeadTimeWrite(BOOST_HW_CONFIG_PWM_BASE, deadtimeInt);
}
//-----------------------------------------------------------------------------
float boostHwGetPwmDeadTime(void){

    uint32_t deadtimeInt;
    float deadtime;

    deadtimeInt = zynqAxiPwmDeadTimeRead(BOOST_HW_CONFIG_PWM_BASE);
    deadtime = ( (float)deadtimeInt ) / ( (float)BOOST_HW_PWM_CLK );

    return deadtime;
}
//-----------------------------------------------------------------------------
void boostHwSetAdcEnable(uint32_t enable){

    zynqAxiAdcEnableWrite(BOOST_HW_CONFIG_ADC_BASE, enable);
}
//-----------------------------------------------------------------------------
uint32_t boostHwGetAdcEnable(void){

    return zynqAxiAdcEnableRead(BOOST_HW_CONFIG_ADC_BASE);
}
//-----------------------------------------------------------------------------
void boostHwSetAdcManualTrigger(uint32_t trigger){

    zynqAxiAdcManualTriggerWrite(BOOST_HW_CONFIG_ADC_BASE, trigger);
}
//-----------------------------------------------------------------------------
uint32_t boostHwGetAdcManualTrigger(void){

    return zynqAxiAdcManualTriggerRead(BOOST_HW_CONFIG_ADC_BASE);
}
//-----------------------------------------------------------------------------
void boostHwSetAdcInterruptEnable(uint32_t enable){

    zynqAxiAdcInterruptEnableWrite(BOOST_HW_CONFIG_ADC_BASE, enable);
}
//-----------------------------------------------------------------------------
uint32_t boostHwGetAdcInterruptEnable(void){

    return zynqAxiAdcInterruptEnableRead(BOOST_HW_CONFIG_ADC_BASE);
}
//-----------------------------------------------------------------------------
void boostHwSetAdcSpiFreq(uint32_t freq){

    uint32_t clkdiv;

    clkdiv = BOOST_HW_ADC_CLK / (freq << 1);

    zynqAxiAdcSpiClkDivWrite(BOOST_HW_CONFIG_ADC_BASE, clkdiv);
}
//-----------------------------------------------------------------------------
uint32_t boostHwGetAdcSpiFreq(void){

    uint32_t clkdiv, freq;

    clkdiv = zynqAxiAdcSpiClkDivRead(BOOST_HW_CONFIG_ADC_BASE);

    freq = BOOST_HW_ADC_CLK / (clkdiv << 1);

    return freq;
}
//-----------------------------------------------------------------------------
int32_t boostHwGetMeasurements(void *meas){

    boostConfigMeasurements_t *dst;
    uint16_t *src;

    src = (uint16_t *)BOOST_HW_CONFIG_ADC_BUFFER;
    dst = (boostConfigMeasurements_t *)meas;

    /* Measurements */
    dst->v_dc_in = hwControl.gains.v_dc_in_gain * ((float)(*src++)) + hwControl.gains.v_dc_in_ofs;
    dst->v_dc_out = hwControl.gains.v_dc_out_gain * ((float)(*src++)) + hwControl.gains.v_dc_out_ofs;
    dst->v_out  = hwControl.gains.v_out_gain * ((float)(*src++)) + hwControl.gains.v_out_ofs;

    dst->i_l =  hwControl.gains.i_l_gain * ((float)(*src++)) + hwControl.gains.i_l_ofs;
    dst->i_l_avg =  hwControl.gains.i_l_avg_gain * ((float)(*src++)) + hwControl.gains.i_l_avg_ofs;
    dst->i_o =  hwControl.gains.i_o_gain * ((float)(*src++)) + hwControl.gains.i_o_ofs;


    /* Protection */
    if( (dst->i_l > BOOST_CONFIG_I_LIM) || (dst->i_o > BOOST_CONFIG_I_LIM) ) hwControl.status = 1;
    if( (dst->i_l < -BOOST_CONFIG_I_LIM) || (dst->i_o < -BOOST_CONFIG_I_LIM) ) hwControl.status = 1;

    if( (dst->v_dc_in > BOOST_CONFIG_V_LIM) || (dst->v_dc_out > BOOST_CONFIG_V_LIM) || (dst->v_out > BOOST_CONFIG_V_LIM) ) hwControl.status = 1;

    if( hwControl.status != 0 ){
        //boostHwSetPwmOutputEnable(0);
        boostHwShutDown();
        return -1;
    }
    else
        return sizeof(boostConfigMeasurements_t);
}
//-----------------------------------------------------------------------------
int32_t boostHwApplyOutputs(void *outputs, int32_t size){

    boostConfigControl_t *control;

    control = (boostConfigControl_t *)outputs;

    boostHwSetPwmDuty(control->u);

    //control->u = 0.5f;

    return 0;
}
//-----------------------------------------------------------------------------
void boostHwDisable(void){

    boostHwSetPwmDuty(0.0f);
    boostHwSetPwmOutputEnable(0);
    boostHwSetPwmOvfTriggerEnable(0);
}
//-----------------------------------------------------------------------------
void boostHwEnable(void){

    boostHwSetPwmDuty(0.0f);
    boostHwSetPwmOvfTriggerEnable(1);
    //boostHwSetPwmOutputEnable(1);
}
//-----------------------------------------------------------------------------
void boostHwControllerDisable(void){

    boostHwSetPwmOutputEnable(0);
}
//-----------------------------------------------------------------------------
void boostHwControllerEnable(void){

    boostHwSetPwmOutputEnable(1);
}
//-----------------------------------------------------------------------------
void boostHwSetInputRelay(uint32_t state){

    uint32_t gpio;

    state = (state & 0x01) << BOOST_HW_CONFIG_GPIO_IN_RLY_OFFS;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, BOOST_HW_CONFIG_GPIO_CHANNEL) & (~BOOST_HW_CONFIG_GPIO_IN_RLY);

    gpio = gpio | state;

    XGpio_DiscreteWrite(&hwControl.gpio, BOOST_HW_CONFIG_GPIO_CHANNEL, gpio);
}
//-----------------------------------------------------------------------------
uint32_t boostHwGetInputRelay(void){

    uint32_t gpio;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, BOOST_HW_CONFIG_GPIO_CHANNEL) & (BOOST_HW_CONFIG_GPIO_IN_RLY);

    gpio = gpio >> BOOST_HW_CONFIG_GPIO_IN_RLY_OFFS;

    return gpio;
}
//-----------------------------------------------------------------------------
void boostHwSetOutputRelay(uint32_t state){

    uint32_t gpio;

    state = (state & 0x01) << BOOST_HW_CONFIG_GPIO_OUT_RLY_OFFS;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, BOOST_HW_CONFIG_GPIO_CHANNEL) & (~BOOST_HW_CONFIG_GPIO_OUT_RLY);

    gpio = gpio | state;

    XGpio_DiscreteWrite(&hwControl.gpio, BOOST_HW_CONFIG_GPIO_CHANNEL, gpio);
}
//-----------------------------------------------------------------------------
uint32_t boostHwGetOutputRelay(void){

    uint32_t gpio;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, BOOST_HW_CONFIG_GPIO_CHANNEL) & (BOOST_HW_CONFIG_GPIO_OUT_RLY);

    gpio = gpio >> BOOST_HW_CONFIG_GPIO_OUT_RLY_OFFS;

    return gpio;
}
//-----------------------------------------------------------------------------
void boostHwSetMeasGains(boostConfigMeasGains_t *gains){

    hwControl.gains = *gains;
}
//-----------------------------------------------------------------------------
uint32_t boostHwGetMeasGains(boostConfigMeasGains_t *gains){

    *gains = hwControl.gains;

    return sizeof(boostConfigMeasGains_t);
}
//-----------------------------------------------------------------------------
void boostHwShutDown(void){

    float u;

    u = boostHwGetPwmDuty();
    boostHwSetInputRelay(0);
    boostHwSetOutputRelay(0);

    u = u - BOOST_CONFIG_SHUTDOWN_U_DEC;
    if( u < 0.0f ){
        //boostHwSetPwmOutputEnable(0);
        u = 0.0f;
    }

    boostHwSetPwmDuty(u);
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static void boostHwInitializeAdc(void *intc, boostHwAdcIrqHandle_t irqhandle){

    uint32_t clkdiv;

    clkdiv = BOOST_HW_ADC_CLK / (BOOST_HW_CONFIG_ADC_SPI_FREQ_HZ << 1);

    zynqAxiAdcEnableWrite(BOOST_HW_CONFIG_ADC_BASE, 0);

    zynqAxiAdcInterruptEnableWrite(BOOST_HW_CONFIG_ADC_BASE, 1);

    zynqAxiAdcSpiClkDivWrite(BOOST_HW_CONFIG_ADC_BASE, clkdiv);

    zynqAxiAdcBufferAddressWrite(BOOST_HW_CONFIG_ADC_BASE, BOOST_HW_CONFIG_ADC_BUFFER);

    zynqAxiAdcInterruptConfig(intc, BOOST_HW_CONFIG_IRQ_PL_CPU1, irqhandle);

    zynqAxiAdcEnableWrite(BOOST_HW_CONFIG_ADC_BASE, 1);
}
//-----------------------------------------------------------------------------
static void boostHwInitializePwm(void){

    boostHwSetPwmReset(1);

    boostHwSetPwmFrequency(BOOST_HW_CONFIG_PWM_FREQ_HZ);
    boostHwSetPwmDuty(0.0f);
    boostHwSetPwmDeadTime(BOOST_HW_CONFIG_PWM_DEAD_TIME_NS);

    boostHwSetPwmOvfTriggerEnable(0);
    boostHwSetPwmOutputEnable(0);

    boostHwSetPwmReset(0);
}
//-----------------------------------------------------------------------------
static void boostHwInitializeGpio(void){

    XGpio_Config *cfg_ptr = 0;

    /* Initializes GPIOs */
    cfg_ptr = XGpio_LookupConfig(BOOST_HW_CONFIG_GPIO_ID);
    XGpio_CfgInitialize(&hwControl.gpio, cfg_ptr, cfg_ptr->BaseAddress);
    XGpio_SetDataDirection(&hwControl.gpio, BOOST_HW_CONFIG_GPIO_CHANNEL, 0);
}
//-----------------------------------------------------------------------------
static void boostHwInitializeMeasGains(void){

    hwControl.gains.i_l_gain = BOOST_CONFIG_IL_GAIN;
    hwControl.gains.i_l_ofs =  BOOST_CONFIG_IL_OFFS;

    hwControl.gains.i_l_avg_gain = BOOST_CONFIG_IL_AVG_GAIN;
    hwControl.gains.i_l_avg_ofs = BOOST_CONFIG_IL_AVG_OFFS;
    
    hwControl.gains.i_o_gain = BOOST_CONFIG_IO_AVG_GAIN;
    hwControl.gains.i_o_ofs =  BOOST_CONFIG_IO_AVG_OFFS;

    hwControl.gains.v_dc_in_gain = BOOST_CONFIG_V_DC_IN_GAIN;
    hwControl.gains.v_dc_in_ofs =  BOOST_CONFIG_V_DC_IN_OFFS;

    hwControl.gains.v_dc_out_gain = BOOST_CONFIG_V_DC_OUT_GAIN;
    hwControl.gains.v_dc_out_ofs =  BOOST_CONFIG_V_DC_OUT_OFFS;

    hwControl.gains.v_out_gain = BOOST_CONFIG_V_OUT_GAIN;
    hwControl.gains.v_out_ofs = BOOST_CONFIG_V_OUT_OFFS;
}
//-----------------------------------------------------------------------------
//=============================================================================
#endif /* SOC_CPU1 */
