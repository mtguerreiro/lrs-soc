
#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "boostSbpHw.h"

#include "xparameters.h"
#include "zynqAxiFsPwm.h"
#include "zynqAxiAdc.h"

#include "boostSbpConfig.h"

#include "zynqConfig.h"

#include "xgpio.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define BOOST_SBP_HW_CONFIG_ADC_SPI_FREQ_HZ      ((uint32_t)16666666)
#define BOOST_SBP_HW_CONFIG_PWM_FREQ_HZ          ((uint32_t) 100000 )
#define BOOST_SBP_HW_CONFIG_PWM_DEAD_TIME_NS     ((float) 100e-9 )
//#define BOOST_SBP_HW_CONFIG_PWM_BASE             XPAR_AXI_PWM_BUCKBOOST_0_S00_AXI_BASEADDR//XPAR_AXI_PWM_1_S00_AXI_BASEADDR
#define BOOST_SBP_HW_CONFIG_PWM_BASE             XPAR_AXI_PWM_0_S00_AXI_BASEADDR//XPAR_AXI_PWM_1_S00_AXI_BASEADDR
#define BOOST_SBP_HW_CONFIG_ADC_BASE             XPAR_ADC_PSCTL_0_S00_AXI_BASEADDR//XPAR_ADC_PSCTL_1_S00_AXI_BASEADDR

#define BOOST_SBP_HW_CONFIG_IRQ_PL_CPU1          ZYNQ_CONFIG_IRQ_PL_TO_CPU1
#define BOOST_SBP_HW_CONFIG_IRQ_PL_CPU1_PRIO     ZYNQ_CONFIG_IRQ_PL_TO_CPU1_PRIO
#define BOOST_SBP_HW_CONFIG_ADC_BUFFER           ( ZYNQ_CONFIG_MEM_PL_TO_CPU1_ADR )

#define BOOST_SBP_HW_CONFIG_GPIO_ID              XPAR_AXI_GPIO_0_DEVICE_ID
#define BOOST_SBP_HW_CONFIG_GPIO_CHANNEL         1
#define BOOST_SBP_HW_CONFIG_GPIO_MASK            0b11

#define BOOST_SBP_HW_CONFIG_GPIO_IN_PRECH_OFFS   (0U)
#define BOOST_SBP_HW_CONFIG_GPIO_IN_PRECH        (1 << BOOST_SBP_HW_CONFIG_GPIO_IN_PRECH_OFFS)

#define BOOST_SBP_HW_CONFIG_GPIO_IN_DISCH_OFFS   (1U)
#define BOOST_SBP_HW_CONFIG_GPIO_IN_DISCH        (1 << BOOST_SBP_HW_CONFIG_GPIO_IN_DISCH_OFFS)

#define BOOST_SBP_HW_CONFIG_GPIO_IN_SSR_P_OFFS   (2U)
#define BOOST_SBP_HW_CONFIG_GPIO_IN_SSR_P        (1 << BOOST_SBP_HW_CONFIG_GPIO_IN_SSR_P_OFFS)

#define BOOST_SBP_HW_CONFIG_GPIO_IN_SSR_N_OFFS   (3U)
#define BOOST_SBP_HW_CONFIG_GPIO_IN_SSR_N        (1 << BOOST_SBP_HW_CONFIG_GPIO_IN_SSR_N_OFFS)

#define BOOST_SBP_HW_CONFIG_GPIO_OUT_PRECH_OFFS  (4U)
#define BOOST_SBP_HW_CONFIG_GPIO_OUT_PRECH       (1 << BOOST_SBP_HW_CONFIG_GPIO_OUT_PRECH_OFFS)

#define BOOST_SBP_HW_CONFIG_GPIO_OUT_DISCH_OFFS  (5U)
#define BOOST_SBP_HW_CONFIG_GPIO_OUT_DISCH       (1 << BOOST_SBP_HW_CONFIG_GPIO_OUT_DISCH_OFFS)

#define BOOST_SBP_HW_CONFIG_GPIO_OUT_SSR_P_OFFS  (6U)
#define BOOST_SBP_HW_CONFIG_GPIO_OUT_SSR_P       (1 << BOOST_SBP_HW_CONFIG_GPIO_OUT_SSR_P_OFFS)

#define BOOST_SBP_HW_CONFIG_GPIO_OUT_SSR_N_OFFS  (7U)
#define BOOST_SBP_HW_CONFIG_GPIO_OUT_SSR_N       (1 << BOOST_SBP_HW_CONFIG_GPIO_OUT_SSR_N_OFFS)

/* PWM peripheral clock, in Hz */
#define BOOST_SBP_HW_PWM_CLK                      100000000
#define BOOST_SBP_HW_ADC_CLK                      100000000

typedef struct{

    uint32_t status;

    uint32_t pwmPeriod;

    boostSbpConfigMeasurements_t meas;
    boostSbpConfigControl_t control;

    XGpio gpio;

//    boostSbpConfigMeasGains_t gains;

    float alpha;

}boostSbpHwControl_t;
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static void boostSbpHwInitializeAdc(void *intc, boostSbpHwAdcIrqHandle_t irqhandle);
static void boostSbpHwInitializePwm(void);
static void boostSbpHwInitializeGpio(void);
static void boostSbpHwInitializeMeasGains(void);
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static boostSbpHwControl_t hwControl = {.pwmPeriod = 0, .status = 0, .alpha = 0.2f};
//static float i_i_filt = 0.0f, i_1_filt = 0.0f, i_o_filt = 0.0f, i_2_filt = 0.0f;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t boostSbpHwInitialize(boostSbpHwInitConfig_t *config){

    boostSbpHwInitializeAdc(config->intc, config->irqhandle);
    boostSbpHwInitializePwm();
    boostSbpHwInitializeGpio();
    boostSbpHwInitializeMeasGains();

    return 0;
}
//-----------------------------------------------------------------------------
int32_t boostSbpHwStatus(void){

    return hwControl.status;
}
//-----------------------------------------------------------------------------
void boostSbpHwStatusClear(void){

    hwControl.status = 0;
}
//-----------------------------------------------------------------------------
void boostSbpHwSetPwmReset(uint32_t reset){

    zynqAxiFsPwmResetWrite(BOOST_SBP_HW_CONFIG_PWM_BASE, reset);
}
//-----------------------------------------------------------------------------
uint32_t boostSbpHwGetPwmReset(void){

    return zynqAxiFsPwmResetRead(BOOST_SBP_HW_CONFIG_PWM_BASE);
}
//-----------------------------------------------------------------------------
void boostSbpHwSetPwmOutputEnable(uint32_t enable){

    zynqAxiFsPwmOutputEnableWrite(BOOST_SBP_HW_CONFIG_PWM_BASE, enable);
}
//-----------------------------------------------------------------------------
uint32_t boostSbpHwGetPwmOutputEnable(void){

    return zynqAxiFsPwmOutputEnableRead(BOOST_SBP_HW_CONFIG_PWM_BASE);
}
//-----------------------------------------------------------------------------
void boostSbpHwSetPwmOvfTriggerEnable(uint32_t enable){

    zynqAxiFsPwmOvfTriggerEnableWrite(BOOST_SBP_HW_CONFIG_PWM_BASE, enable);
}
//-----------------------------------------------------------------------------
uint32_t boostSbpHwGetPwmOvfTriggerEnable(void){

    return zynqAxiFsPwmOvfTriggerEnableRead(BOOST_SBP_HW_CONFIG_PWM_BASE);
}
//-----------------------------------------------------------------------------
void boostSbpHwSetPwmInv(uint32_t enable){

    zynqAxiFsPwmInvWrite(BOOST_SBP_HW_CONFIG_PWM_BASE, enable);
}
//-----------------------------------------------------------------------------
uint32_t boostSbpHwGetPwmInv(void){

    return zynqAxiFsPwmInvRead(BOOST_SBP_HW_CONFIG_PWM_BASE);
}
//-----------------------------------------------------------------------------
void boostSbpHwSetPwmMode(uint32_t mode){

    zynqAxiFsPwmModeWrite(BOOST_SBP_HW_CONFIG_PWM_BASE, mode);
}
//-----------------------------------------------------------------------------
uint32_t boostSbpHwGetPwmMode(void){

    return zynqAxiFsPwmModeRead(BOOST_SBP_HW_CONFIG_PWM_BASE);
}
//-----------------------------------------------------------------------------
void boostSbpHwSetPwmLsSw(uint32_t state){

    zynqAxiFsPwmLsSwWrite(BOOST_SBP_HW_CONFIG_PWM_BASE, state);
}
//-----------------------------------------------------------------------------
uint32_t boostSbpHwGetPwmLsSw(void){

    return zynqAxiFsPwmLsSwRead(BOOST_SBP_HW_CONFIG_PWM_BASE);
}
//-----------------------------------------------------------------------------
void boostSbpHwSetPwmHsSw(uint32_t state){

    zynqAxiFsPwmHsSwWrite(BOOST_SBP_HW_CONFIG_PWM_BASE, state);
}
//-----------------------------------------------------------------------------
uint32_t boostSbpHwGetPwmHsSw(void){

    return zynqAxiFsPwmHsSwRead(BOOST_SBP_HW_CONFIG_PWM_BASE);
}
//-----------------------------------------------------------------------------
void boostSbpHwSetPwmFrequency(uint32_t freq){

    uint32_t period;

    period = BOOST_SBP_HW_PWM_CLK / (freq << 1);

    hwControl.pwmPeriod = period;

    zynqAxiFsPwmPeriodWrite(BOOST_SBP_HW_CONFIG_PWM_BASE, period);
}
//-----------------------------------------------------------------------------
uint32_t boostSbpHwGetPwmFrequency(void){

    uint32_t freq, period;

    period = zynqAxiFsPwmPeriodRead(BOOST_SBP_HW_CONFIG_PWM_BASE);

    freq = BOOST_SBP_HW_PWM_CLK / (period << 1);

    return freq;
}
//-----------------------------------------------------------------------------
void boostSbpHwSetPwmDuty(float duty){

    uint32_t dutyInt;

    dutyInt = (uint32_t)( duty * ((float)hwControl.pwmPeriod ) );

    zynqAxiFsPwmDutyWrite(BOOST_SBP_HW_CONFIG_PWM_BASE, dutyInt);
}
//-----------------------------------------------------------------------------
float boostSbpHwGetPwmDuty(void){

    uint32_t dutyInt;
    float duty;

    dutyInt = zynqAxiFsPwmDutyRead(BOOST_SBP_HW_CONFIG_PWM_BASE);
    duty = ( (float)dutyInt ) / ( (float)hwControl.pwmPeriod );

    return duty;
}
//-----------------------------------------------------------------------------
void boostSbpHwSetPwmDeadTime(float deadtime){

    uint32_t deadtimeInt;

    deadtimeInt = (uint32_t)( deadtime * ((float)BOOST_SBP_HW_PWM_CLK) );

    zynqAxiFsPwmDeadTimeWrite(BOOST_SBP_HW_CONFIG_PWM_BASE, deadtimeInt);
}
//-----------------------------------------------------------------------------
float boostSbpHwGetPwmDeadTime(void){

    uint32_t deadtimeInt;
    float deadtime;

    deadtimeInt = zynqAxiFsPwmDeadTimeRead(BOOST_SBP_HW_CONFIG_PWM_BASE);
    deadtime = ( (float)deadtimeInt ) / ( (float)BOOST_SBP_HW_PWM_CLK );

    return deadtime;
}
//-----------------------------------------------------------------------------
void boostSbpHwSetAdcEnable(uint32_t enable){

    zynqAxiAdcEnableWrite(BOOST_SBP_HW_CONFIG_ADC_BASE, enable);
}
//-----------------------------------------------------------------------------
uint32_t boostSbpHwGetAdcEnable(void){

    return zynqAxiAdcEnableRead(BOOST_SBP_HW_CONFIG_ADC_BASE);
}
//-----------------------------------------------------------------------------
void boostSbpHwSetAdcManualTrigger(uint32_t trigger){

    zynqAxiAdcManualTriggerWrite(BOOST_SBP_HW_CONFIG_ADC_BASE, trigger);
}
//-----------------------------------------------------------------------------
uint32_t boostSbpHwGetAdcManualTrigger(void){

    return zynqAxiAdcManualTriggerRead(BOOST_SBP_HW_CONFIG_ADC_BASE);
}
//-----------------------------------------------------------------------------
void boostSbpHwSetAdcInterruptEnable(uint32_t enable){

    zynqAxiAdcInterruptEnableWrite(BOOST_SBP_HW_CONFIG_ADC_BASE, enable);
}
//-----------------------------------------------------------------------------
uint32_t boostSbpHwGetAdcInterruptEnable(void){

    return zynqAxiAdcInterruptEnableRead(BOOST_SBP_HW_CONFIG_ADC_BASE);
}
//-----------------------------------------------------------------------------
void boostSbpHwSetAdcSpiFreq(uint32_t freq){

    uint32_t clkdiv;

    clkdiv = BOOST_SBP_HW_ADC_CLK / (freq << 1);

    zynqAxiAdcSpiClkDivWrite(BOOST_SBP_HW_CONFIG_ADC_BASE, clkdiv);
}
//-----------------------------------------------------------------------------
uint32_t boostSbpHwGetAdcSpiFreq(void){

    uint32_t clkdiv, freq;

    clkdiv = zynqAxiAdcSpiClkDivRead(BOOST_SBP_HW_CONFIG_ADC_BASE);

    freq = BOOST_SBP_HW_ADC_CLK / (clkdiv << 1);

    return freq;
}
//-----------------------------------------------------------------------------
int32_t boostSbpHwGetMeasurements(void *meas){

    boostSbpConfigMeasurements_t *dst;
    uint16_t *src;

    src = (uint16_t *)BOOST_SBP_HW_CONFIG_ADC_BUFFER;
    dst = (boostSbpConfigMeasurements_t *)meas;

    /* Measurements */
    dst->vi = *src++;
    dst->vi_dc = *src++;
    dst->vo_dc = *src++;
    dst->vo = *src++;

    dst->ii = *src++;
    dst->il = *src++;
    dst->io = *src++;

    /* Protection */
//    if( (dst->ii > BOOST_SBP_CONFIG_I_LIM) || (dst->il > BOOST_SBP_CONFIG_I_LIM) )
//        hwControl.status = 1;
//
//    if( (dst->ii < -BOOST_SBP_CONFIG_I_LIM) || (dst->il < -BOOST_SBP_CONFIG_I_LIM) )
//        hwControl.status = 1;
//
//    if( (dst->v_in > BOOST_SBP_CONFIG_V_LIM) || (dst->v_dc_out > BOOST_SBP_CONFIG_V_LIM) || (dst->v_out > BOOST_SBP_CONFIG_V_LIM) )
//        hwControl.status = 1;
//
//    if( hwControl.status != 0 ){
//        boostSbpHwShutDown();
//        return -1;
//    }

    return sizeof(boostSbpConfigMeasurements_t);
}
//-----------------------------------------------------------------------------
int32_t boostSbpHwApplyOutputs(void *outputs, int32_t size){

    boostSbpConfigControl_t *control;

    control = (boostSbpConfigControl_t *)outputs;

    boostSbpHwSetPwmDuty(control->u);

    return 0;
}
//-----------------------------------------------------------------------------
void boostSbpHwDisable(void){

    boostSbpHwSetPwmDuty(0.0f);
    boostSbpHwSetPwmOutputEnable(0);
    boostSbpHwSetPwmOvfTriggerEnable(0);
}
//-----------------------------------------------------------------------------
void boostSbpHwEnable(void){

    boostSbpHwSetPwmDuty(0.0f);
    boostSbpHwSetPwmOvfTriggerEnable(1);
    //boostSbpHwSetPwmOutputEnable(1);
}
//-----------------------------------------------------------------------------
void boostSbpHwControllerDisable(void){

    boostSbpHwSetPwmOutputEnable(0);
}
//-----------------------------------------------------------------------------
void boostSbpHwControllerEnable(void){

    boostSbpHwSetPwmOutputEnable(1);
}
//-----------------------------------------------------------------------------
void boostSbpHwSetInputPrech(uint32_t state){

    uint32_t gpio;

    state = (state & 0x01) << BOOST_SBP_HW_CONFIG_GPIO_IN_PRECH_OFFS;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, BOOST_SBP_HW_CONFIG_GPIO_CHANNEL) & (~BOOST_SBP_HW_CONFIG_GPIO_IN_PRECH);

    gpio = gpio | state;

    XGpio_DiscreteWrite(&hwControl.gpio, BOOST_SBP_HW_CONFIG_GPIO_CHANNEL, gpio);
}
//-----------------------------------------------------------------------------
uint32_t boostSbpHwGetInputPrech(void){

    uint32_t gpio;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, BOOST_SBP_HW_CONFIG_GPIO_CHANNEL) & (BOOST_SBP_HW_CONFIG_GPIO_IN_PRECH);

    gpio = gpio >> BOOST_SBP_HW_CONFIG_GPIO_IN_PRECH_OFFS;

    return gpio;
}
//-----------------------------------------------------------------------------
void boostSbpHwSetInputDisch(uint32_t state){

    uint32_t gpio;

    state = (state & 0x01) << BOOST_SBP_HW_CONFIG_GPIO_IN_DISCH_OFFS;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, BOOST_SBP_HW_CONFIG_GPIO_CHANNEL) & (~BOOST_SBP_HW_CONFIG_GPIO_IN_DISCH);

    gpio = gpio | state;

    XGpio_DiscreteWrite(&hwControl.gpio, BOOST_SBP_HW_CONFIG_GPIO_CHANNEL, gpio);
}
//-----------------------------------------------------------------------------
uint32_t boostSbpHwGetInputDisch(void){

    uint32_t gpio;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, BOOST_SBP_HW_CONFIG_GPIO_CHANNEL) & (BOOST_SBP_HW_CONFIG_GPIO_IN_DISCH);

    gpio = gpio >> BOOST_SBP_HW_CONFIG_GPIO_IN_DISCH_OFFS;

    return gpio;
}
//-----------------------------------------------------------------------------
void boostSbpHwSetInputSsrP(uint32_t state){

    uint32_t gpio;

    state = (state & 0x01) << BOOST_SBP_HW_CONFIG_GPIO_IN_SSR_P_OFFS;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, BOOST_SBP_HW_CONFIG_GPIO_CHANNEL) & (~BOOST_SBP_HW_CONFIG_GPIO_IN_SSR_P);

    gpio = gpio | state;

    XGpio_DiscreteWrite(&hwControl.gpio, BOOST_SBP_HW_CONFIG_GPIO_CHANNEL, gpio);
}
//-----------------------------------------------------------------------------
uint32_t boostSbpHwGetInputSsrP(void){

    uint32_t gpio;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, BOOST_SBP_HW_CONFIG_GPIO_CHANNEL) & (BOOST_SBP_HW_CONFIG_GPIO_IN_SSR_P);

    gpio = gpio >> BOOST_SBP_HW_CONFIG_GPIO_IN_SSR_P;

    return gpio;
}
//-----------------------------------------------------------------------------
void boostSbpHwSetInputSsrN(uint32_t state){

    uint32_t gpio;

    state = (state & 0x01) << BOOST_SBP_HW_CONFIG_GPIO_IN_SSR_N_OFFS;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, BOOST_SBP_HW_CONFIG_GPIO_CHANNEL) & (~BOOST_SBP_HW_CONFIG_GPIO_IN_SSR_N);

    gpio = gpio | state;

    XGpio_DiscreteWrite(&hwControl.gpio, BOOST_SBP_HW_CONFIG_GPIO_CHANNEL, gpio);
}
//-----------------------------------------------------------------------------
uint32_t boostSbpHwGetInputSsrN(void){

    uint32_t gpio;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, BOOST_SBP_HW_CONFIG_GPIO_CHANNEL) & (BOOST_SBP_HW_CONFIG_GPIO_IN_SSR_N);

    gpio = gpio >> BOOST_SBP_HW_CONFIG_GPIO_IN_SSR_N;

    return gpio;
}
//-----------------------------------------------------------------------------
void boostSbpHwSetOutputPrech(uint32_t state){

    uint32_t gpio;

    state = (state & 0x01) << BOOST_SBP_HW_CONFIG_GPIO_OUT_PRECH_OFFS;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, BOOST_SBP_HW_CONFIG_GPIO_CHANNEL) & (~BOOST_SBP_HW_CONFIG_GPIO_OUT_PRECH);

    gpio = gpio | state;

    XGpio_DiscreteWrite(&hwControl.gpio, BOOST_SBP_HW_CONFIG_GPIO_CHANNEL, gpio);
}
//-----------------------------------------------------------------------------
uint32_t boostSbpHwGetOutputPrech(void){

    uint32_t gpio;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, BOOST_SBP_HW_CONFIG_GPIO_CHANNEL) & (BOOST_SBP_HW_CONFIG_GPIO_OUT_PRECH);

    gpio = gpio >> BOOST_SBP_HW_CONFIG_GPIO_OUT_PRECH_OFFS;

    return gpio;
}
//-----------------------------------------------------------------------------
void boostSbpHwSetOutputDisch(uint32_t state){

    uint32_t gpio;

    state = (state & 0x01) << BOOST_SBP_HW_CONFIG_GPIO_OUT_DISCH_OFFS;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, BOOST_SBP_HW_CONFIG_GPIO_CHANNEL) & (~BOOST_SBP_HW_CONFIG_GPIO_OUT_DISCH);

    gpio = gpio | state;

    XGpio_DiscreteWrite(&hwControl.gpio, BOOST_SBP_HW_CONFIG_GPIO_CHANNEL, gpio);
}
//-----------------------------------------------------------------------------
uint32_t boostSbpHwGetOutputDisch(void){

    uint32_t gpio;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, BOOST_SBP_HW_CONFIG_GPIO_CHANNEL) & (BOOST_SBP_HW_CONFIG_GPIO_OUT_DISCH);

    gpio = gpio >> BOOST_SBP_HW_CONFIG_GPIO_OUT_DISCH_OFFS;

    return gpio;
}
//-----------------------------------------------------------------------------
void boostSbpHwSetOutputSsrP(uint32_t state){

    uint32_t gpio;

    state = (state & 0x01) << BOOST_SBP_HW_CONFIG_GPIO_OUT_SSR_P_OFFS;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, BOOST_SBP_HW_CONFIG_GPIO_CHANNEL) & (~BOOST_SBP_HW_CONFIG_GPIO_OUT_SSR_P);

    gpio = gpio | state;

    XGpio_DiscreteWrite(&hwControl.gpio, BOOST_SBP_HW_CONFIG_GPIO_CHANNEL, gpio);
}
//-----------------------------------------------------------------------------
uint32_t boostSbpHwGetOutputSsrP(void){

    uint32_t gpio;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, BOOST_SBP_HW_CONFIG_GPIO_CHANNEL) & (BOOST_SBP_HW_CONFIG_GPIO_OUT_SSR_P);

    gpio = gpio >> BOOST_SBP_HW_CONFIG_GPIO_OUT_SSR_P_OFFS;

    return gpio;
}
//-----------------------------------------------------------------------------
void boostSbpHwSetOutputSsrN(uint32_t state){

    uint32_t gpio;

    state = (state & 0x01) << BOOST_SBP_HW_CONFIG_GPIO_OUT_SSR_N_OFFS;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, BOOST_SBP_HW_CONFIG_GPIO_CHANNEL) & (~BOOST_SBP_HW_CONFIG_GPIO_OUT_SSR_N);

    gpio = gpio | state;

    XGpio_DiscreteWrite(&hwControl.gpio, BOOST_SBP_HW_CONFIG_GPIO_CHANNEL, gpio);
}
//-----------------------------------------------------------------------------
uint32_t boostSbpHwGetOutputSsrN(void){

    uint32_t gpio;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, BOOST_SBP_HW_CONFIG_GPIO_CHANNEL) & (BOOST_SBP_HW_CONFIG_GPIO_OUT_SSR_N);

    gpio = gpio >> BOOST_SBP_HW_CONFIG_GPIO_OUT_SSR_N_OFFS;

    return gpio;
}
//-----------------------------------------------------------------------------
//void boostSbpHwSetMeasGains(boostSbpConfigMeasGains_t *gains){
//
//    hwControl.gains = *gains;
//}
//-----------------------------------------------------------------------------
//uint32_t boostSbpHwGetMeasGains(boostSbpConfigMeasGains_t *gains){
//
//    *gains = hwControl.gains;
//
//    return sizeof(boostSbpConfigMeasGains_t);
//}
//-----------------------------------------------------------------------------
void boostSbpHwShutDown(void){

    boostSbpHwSetPwmOutputEnable(0);

    boostSbpHwSetOutputSsrP(0);
    boostSbpHwSetOutputSsrN(0);

    boostSbpHwSetInputSsrP(0);
    boostSbpHwSetInputSsrN(0);
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static void boostSbpHwInitializeAdc(void *intc, boostSbpHwAdcIrqHandle_t irqhandle){

    uint32_t clkdiv;

    clkdiv = BOOST_SBP_HW_ADC_CLK / (BOOST_SBP_HW_CONFIG_ADC_SPI_FREQ_HZ << 1);

    zynqAxiAdcEnableWrite(BOOST_SBP_HW_CONFIG_ADC_BASE, 0);

    zynqAxiAdcInterruptEnableWrite(BOOST_SBP_HW_CONFIG_ADC_BASE, 1);

    zynqAxiAdcSpiClkDivWrite(BOOST_SBP_HW_CONFIG_ADC_BASE, clkdiv);

    zynqAxiAdcBufferAddressWrite(BOOST_SBP_HW_CONFIG_ADC_BASE, BOOST_SBP_HW_CONFIG_ADC_BUFFER);

    zynqAxiAdcInterruptConfig(intc, BOOST_SBP_HW_CONFIG_IRQ_PL_CPU1, BOOST_SBP_HW_CONFIG_IRQ_PL_CPU1_PRIO, irqhandle);

    zynqAxiAdcEnableWrite(BOOST_SBP_HW_CONFIG_ADC_BASE, 1);
}
//-----------------------------------------------------------------------------
static void boostSbpHwInitializePwm(void){

    boostSbpHwSetPwmReset(1);

    boostSbpHwSetPwmFrequency(BOOST_SBP_HW_CONFIG_PWM_FREQ_HZ);
    boostSbpHwSetPwmDuty(0.0f);
    boostSbpHwSetPwmDeadTime(BOOST_SBP_HW_CONFIG_PWM_DEAD_TIME_NS);

    boostSbpHwSetPwmOvfTriggerEnable(0);
    boostSbpHwSetPwmOutputEnable(0);

    boostSbpHwSetPwmMode(0);
    boostSbpHwSetPwmHsSw(0);
    boostSbpHwSetPwmLsSw(0);

    boostSbpHwSetPwmReset(0);
}
//-----------------------------------------------------------------------------
static void boostSbpHwInitializeGpio(void){

    XGpio_Config *cfg_ptr = 0;

    /* Initializes GPIOs */
    cfg_ptr = XGpio_LookupConfig(BOOST_SBP_HW_CONFIG_GPIO_ID);
    XGpio_CfgInitialize(&hwControl.gpio, cfg_ptr, cfg_ptr->BaseAddress);
    XGpio_SetDataDirection(&hwControl.gpio, BOOST_SBP_HW_CONFIG_GPIO_CHANNEL, 0);

    boostSbpHwSetInputPrech(0);
    boostSbpHwSetInputDisch(0);
    boostSbpHwSetInputSsrP(0);
    boostSbpHwSetInputSsrN(0);

    boostSbpHwSetOutputPrech(0);
    boostSbpHwSetOutputDisch(0);
    boostSbpHwSetOutputSsrP(0);
    boostSbpHwSetOutputSsrN(0);

//    boostSbpHwSetInputRelay(0);
//    boostSbpHwSetOutputRelay(0);
//    boostSbpHwSetLoadSwitch(0);
}
//-----------------------------------------------------------------------------
static void boostSbpHwInitializeMeasGains(void){

//    hwControl.gains.ii_gain = BOOST_SBP_CFG_II_GAIN;
//    hwControl.gains.ii_ofs =  BOOST_SBP_CFG_II_OFFS;
//
//    hwControl.gains.il_gain = BOOST_SBP_CFG_IL_GAIN;
//    hwControl.gains.il_ofs =  BOOST_SBP_CFG_IL_OFFS;
//
//    hwControl.gains.io_gain = BOOST_SBP_CFG_IO_GAIN;
//    hwControl.gains.io_ofs =  BOOST_SBP_CFG_IO_OFFS;
//
//    hwControl.gains.v_in_gain = BOOST_SBP_CFG_V_IN_GAIN;
//    hwControl.gains.v_in_ofs =  BOOST_SBP_CFG_V_IN_OFFS;
//
//    hwControl.gains.v_dc_out_gain = BOOST_SBP_CFG_V_DC_OUT_GAIN;
//    hwControl.gains.v_dc_out_ofs =  BOOST_SBP_CFG_V_DC_OUT_OFFS;
//
//    hwControl.gains.v_out_gain = BOOST_SBP_CFG_V_OUT_GAIN;
//    hwControl.gains.v_out_ofs = BOOST_SBP_CFG_V_OUT_OFFS;
}
//-----------------------------------------------------------------------------
//=============================================================================
#endif /* SOC_CPU1 */
