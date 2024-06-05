/*
 * buckHw.c
 *
 */

#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "buckHw.h"

#include "xparameters.h"
#include "zynqAxiPwm.h"
#include "zynqAxiAdc.h"

#include "buckConfig.h"

#include "zynqConfig.h"

#include "xgpio.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define BUCK_HW_CONFIG_ADC_SPI_FREQ_HZ      ((uint32_t)16666666)
#define BUCK_HW_CONFIG_PWM_FREQ_HZ          ((uint32_t) 100000 )
#define BUCK_HW_CONFIG_PWM_DEAD_TIME_NS     ((float) 200e-9 )
#define BUCK_HW_CONFIG_PWM_BASE              XPAR_AXI_PWM_1_S00_AXI_BASEADDR
#define BUCK_HW_CONFIG_ADC_BASE              XPAR_ADC_PSCTL_1_S00_AXI_BASEADDR

#define BUCK_HW_CONFIG_IRQ_PL_CPU1           ZYNQ_CONFIG_IRQ_PL_TO_CPU1_2
#define BUCK_HW_CONFIG_ADC_BUFFER            ( ZYNQ_CONFIG_MEM_PL_TO_CPU1_ADR + (ZYNQ_CONFIG_MEM_PL_TO_CPU1_SIZE >> 1) )

#define BUCK_HW_CONFIG_GPIO_ID               XPAR_AXI_GPIO_1_DEVICE_ID
#define BUCK_HW_CONFIG_GPIO_CHANNEL          1
#define BUCK_HW_CONFIG_GPIO_MASK             0b11

#define BUCK_HW_CONFIG_GPIO_IN_RLY_OFFS       (0U)
#define BUCK_HW_CONFIG_GPIO_IN_RLY            (1 << BUCK_HW_CONFIG_GPIO_IN_RLY_OFFS)
#define BUCK_HW_CONFIG_GPIO_OUT_RLY_OFFS      (1U)
#define BUCK_HW_CONFIG_GPIO_OUT_RLY           (1 << BUCK_HW_CONFIG_GPIO_OUT_RLY_OFFS)

/* PWM peripheral clock, in Hz */
#define BUCK_HW_PWM_CLK                      100000000
#define BUCK_HW_ADC_CLK                      100000000

typedef struct{

    uint32_t status;

    uint32_t pwmPeriod;

    buckConfigMeasurements_t meas;
    buckConfigControl_t control;

    XGpio gpio;

    buckConfigMeasGains_t gains;

    float alpha;

}buckHwControl_t;
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static void buckHwInitializeAdc(void *intc, buckHwAdcIrqHandle_t irqhandle);
static void buckHwInitializePwm(void);
static void buckHwInitializeGpio(void);
static void buckHwInitializeMeasGains(void);
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static buckHwControl_t hwControl = {.pwmPeriod = 0, .status = 0, .alpha = 0.2f};
static float i_i_filt = 0.0f, i_1_filt = 0.0f, i_o_filt = 0.0f, i_2_filt = 0.0f;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t buckHwInitialize(buckHwInitConfig_t *config){

    buckHwInitializeAdc(config->intc, config->irqhandle);
    buckHwInitializePwm();
    buckHwInitializeGpio();
    buckHwInitializeMeasGains();

    return 0;
}
//-----------------------------------------------------------------------------
int32_t buckHwStatus(void){

    return hwControl.status;
}
//-----------------------------------------------------------------------------
void buckHwStatusClear(void){

    hwControl.status = 0;
}
//-----------------------------------------------------------------------------
void buckHwSetPwmReset(uint32_t reset){

    zynqAxiPwmResetWrite(BUCK_HW_CONFIG_PWM_BASE, reset);
}
//-----------------------------------------------------------------------------
uint32_t buckHwGetPwmReset(void){

    return zynqAxiPwmResetRead(BUCK_HW_CONFIG_PWM_BASE);
}
//-----------------------------------------------------------------------------
void buckHwSetPwmOutputEnable(uint32_t enable){

    zynqAxiPwmOutputEnableWrite(BUCK_HW_CONFIG_PWM_BASE, enable);
}
//-----------------------------------------------------------------------------
uint32_t buckHwGetPwmOutputEnable(void){

    return zynqAxiPwmOutputEnableRead(BUCK_HW_CONFIG_PWM_BASE);
}
//-----------------------------------------------------------------------------
void buckHwSetPwmOvfTriggerEnable(uint32_t enable){

    zynqAxiPwmOvfTriggerEnableWrite(BUCK_HW_CONFIG_PWM_BASE, enable);
}
//-----------------------------------------------------------------------------
uint32_t buckHwGetPwmOvfTriggerEnable(void){

    return zynqAxiPwmOvfTriggerEnableRead(BUCK_HW_CONFIG_PWM_BASE);
}
//-----------------------------------------------------------------------------
void buckHwSetPwmInv(uint32_t enable){

    zynqAxiPwmInvWrite(BUCK_HW_CONFIG_PWM_BASE, enable);
}
//-----------------------------------------------------------------------------
uint32_t buckHwGetPwmInv(void){

    return zynqAxiPwmInvRead(BUCK_HW_CONFIG_PWM_BASE);
}
//-----------------------------------------------------------------------------

void buckHwSetPwmFrequency(uint32_t freq){

    uint32_t period;

    period = BUCK_HW_PWM_CLK / (freq << 1);

    hwControl.pwmPeriod = period;

    zynqAxiPwmPeriodWrite(BUCK_HW_CONFIG_PWM_BASE, period);
}
//-----------------------------------------------------------------------------
uint32_t buckHwGetPwmFrequency(void){

    uint32_t freq, period;

    period = zynqAxiPwmPeriodRead(BUCK_HW_CONFIG_PWM_BASE);

    freq = BUCK_HW_PWM_CLK / (period << 1);

    return freq;
}
//-----------------------------------------------------------------------------
void buckHwSetPwmDuty(float duty){

    uint32_t dutyInt;

    dutyInt = (uint32_t)( duty * ((float)hwControl.pwmPeriod ) );

    zynqAxiPwmDutyWrite(BUCK_HW_CONFIG_PWM_BASE, dutyInt);
}
//-----------------------------------------------------------------------------
float buckHwGetPwmDuty(void){

    uint32_t dutyInt;
    float duty;

    dutyInt = zynqAxiPwmDutyRead(BUCK_HW_CONFIG_PWM_BASE);
    duty = ( (float)dutyInt ) / ( (float)hwControl.pwmPeriod );

    return duty;
}
//-----------------------------------------------------------------------------
void buckHwSetPwmDeadTime(float deadtime){

    uint32_t deadtimeInt;

    deadtimeInt = (uint32_t)( deadtime * ((float)BUCK_HW_PWM_CLK) );

    zynqAxiPwmDeadTimeWrite(BUCK_HW_CONFIG_PWM_BASE, deadtimeInt);
}
//-----------------------------------------------------------------------------
float buckHwGetPwmDeadTime(void){

    uint32_t deadtimeInt;
    float deadtime;

    deadtimeInt = zynqAxiPwmDeadTimeRead(BUCK_HW_CONFIG_PWM_BASE);
    deadtime = ( (float)deadtimeInt ) / ( (float)BUCK_HW_PWM_CLK );

    return deadtime;
}
//-----------------------------------------------------------------------------
void buckHwSetAdcEnable(uint32_t enable){

    zynqAxiAdcEnableWrite(BUCK_HW_CONFIG_ADC_BASE, enable);
}
//-----------------------------------------------------------------------------
uint32_t buckHwGetAdcEnable(void){

    return zynqAxiAdcEnableRead(BUCK_HW_CONFIG_ADC_BASE);
}
//-----------------------------------------------------------------------------
void buckHwSetAdcManualTrigger(uint32_t trigger){

    zynqAxiAdcManualTriggerWrite(BUCK_HW_CONFIG_ADC_BASE, trigger);
}
//-----------------------------------------------------------------------------
uint32_t buckHwGetAdcManualTrigger(void){

    return zynqAxiAdcManualTriggerRead(BUCK_HW_CONFIG_ADC_BASE);
}
//-----------------------------------------------------------------------------
void buckHwSetAdcInterruptEnable(uint32_t enable){

    zynqAxiAdcInterruptEnableWrite(BUCK_HW_CONFIG_ADC_BASE, enable);
}
//-----------------------------------------------------------------------------
uint32_t buckHwGetAdcInterruptEnable(void){

    return zynqAxiAdcInterruptEnableRead(BUCK_HW_CONFIG_ADC_BASE);
}
//-----------------------------------------------------------------------------
void buckHwSetAdcSpiFreq(uint32_t freq){

    uint32_t clkdiv;

    clkdiv = BUCK_HW_ADC_CLK / (freq << 1);

    zynqAxiAdcSpiClkDivWrite(BUCK_HW_CONFIG_ADC_BASE, clkdiv);
}
//-----------------------------------------------------------------------------
uint32_t buckHwGetAdcSpiFreq(void){

    uint32_t clkdiv, freq;

    clkdiv = zynqAxiAdcSpiClkDivRead(BUCK_HW_CONFIG_ADC_BASE);

    freq = BUCK_HW_ADC_CLK / (clkdiv << 1);

    return freq;
}
//-----------------------------------------------------------------------------
int32_t buckHwGetMeasurements(void *meas){

    buckConfigMeasurements_t *dst;
    uint16_t *src;

    src = (uint16_t *)BUCK_HW_CONFIG_ADC_BUFFER;
    dst = (buckConfigMeasurements_t *)meas;

    /* Measurements */
    dst->i_o =  hwControl.gains.i_o_gain * ((float)(*src++)) + hwControl.gains.i_o_ofs;
    dst->i_l =  hwControl.gains.i_l_gain * ((float)(*src++)) + hwControl.gains.i_l_ofs;

    dst->v_dc_out = hwControl.gains.v_dc_out_gain * ((float)(*src++)) + hwControl.gains.v_dc_out_ofs;
    dst->v_out  = hwControl.gains.v_out_gain * ((float)(*src++)) + hwControl.gains.v_out_ofs;

    dst->v_dc_in = hwControl.gains.v_dc_in_gain * ((float)(*src++)) + hwControl.gains.v_dc_in_ofs;
    dst->v_in = hwControl.gains.v_in_gain * ((float)(*src++)) + hwControl.gains.v_in_ofs;

    /* Protection */
    if( (dst->i_l > BUCK_CONFIG_I_LIM) ) hwControl.status = 1;
    if( (dst->i_l < -BUCK_CONFIG_I_LIM) ) hwControl.status = 1;

    if( (dst->v_dc_in > BUCK_CONFIG_V_LIM) || (dst->v_dc_out > BUCK_CONFIG_V_LIM) || (dst->v_out > BUCK_CONFIG_V_LIM) ) hwControl.status = 1;

    if( hwControl.status != 0 ){
        //buckHwSetPwmOutputEnable(0);
        buckHwShutDown();
        return -1;
    }
    else
        return sizeof(buckConfigMeasurements_t);

//    return sizeof(buckConfigMeasurements_t);
}
//-----------------------------------------------------------------------------
int32_t buckHwApplyOutputs(void *outputs, int32_t size){

    buckConfigControl_t *control;

    control = (buckConfigControl_t *)outputs;

    buckHwSetPwmDuty(control->u);

    //control->u = 0.5f;

    return 0;
}
//-----------------------------------------------------------------------------
void buckHwDisable(void){

    buckHwSetPwmDuty(0.0f);
    buckHwSetPwmOutputEnable(0);
    buckHwSetPwmOvfTriggerEnable(0);
}
//-----------------------------------------------------------------------------
void buckHwEnable(void){

    buckHwSetPwmDuty(0.0f);
    buckHwSetPwmOvfTriggerEnable(1);
    //buckHwSetPwmOutputEnable(1);
}
//-----------------------------------------------------------------------------
void buckHwControllerDisable(void){

    buckHwSetPwmOutputEnable(0);
}
//-----------------------------------------------------------------------------
void buckHwControllerEnable(void){

    buckHwSetPwmOutputEnable(1);
}
//-----------------------------------------------------------------------------
void buckHwSetInputRelay(uint32_t state){

    uint32_t gpio;

    state = (state & 0x01) << BUCK_HW_CONFIG_GPIO_IN_RLY_OFFS;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, BUCK_HW_CONFIG_GPIO_CHANNEL) & (~BUCK_HW_CONFIG_GPIO_IN_RLY);

    gpio = gpio | state;

    XGpio_DiscreteWrite(&hwControl.gpio, BUCK_HW_CONFIG_GPIO_CHANNEL, gpio);
}
//-----------------------------------------------------------------------------
uint32_t buckHwGetInputRelay(void){

    uint32_t gpio;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, BUCK_HW_CONFIG_GPIO_CHANNEL) & (BUCK_HW_CONFIG_GPIO_IN_RLY);

    gpio = gpio >> BUCK_HW_CONFIG_GPIO_IN_RLY_OFFS;

    return gpio;
}
//-----------------------------------------------------------------------------
void buckHwSetOutputRelay(uint32_t state){

    uint32_t gpio;

    state = (state & 0x01) << BUCK_HW_CONFIG_GPIO_OUT_RLY_OFFS;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, BUCK_HW_CONFIG_GPIO_CHANNEL) & (~BUCK_HW_CONFIG_GPIO_OUT_RLY);

    gpio = gpio | state;

    XGpio_DiscreteWrite(&hwControl.gpio, BUCK_HW_CONFIG_GPIO_CHANNEL, gpio);
}
//-----------------------------------------------------------------------------
uint32_t buckHwGetOutputRelay(void){

    uint32_t gpio;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, BUCK_HW_CONFIG_GPIO_CHANNEL) & (BUCK_HW_CONFIG_GPIO_OUT_RLY);

    gpio = gpio >> BUCK_HW_CONFIG_GPIO_OUT_RLY_OFFS;

    return gpio;
}
//-----------------------------------------------------------------------------
void buckHwSetMeasGains(buckConfigMeasGains_t *gains){

    hwControl.gains = *gains;
}
//-----------------------------------------------------------------------------
uint32_t buckHwGetMeasGains(buckConfigMeasGains_t *gains){

    *gains = hwControl.gains;

    return sizeof(buckConfigMeasGains_t);
}
//-----------------------------------------------------------------------------
void buckHwShutDown(void){

    float u;

    u = buckHwGetPwmDuty();
    buckHwSetInputRelay(0);
    buckHwSetOutputRelay(0);

    u = u - BUCK_CONFIG_SHUTDOWN_U_DEC;
    if( u < 0.0f ){
        //buckHwSetPwmOutputEnable(0);
        u = 0.0f;
    }

    buckHwSetPwmDuty(u);
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static void buckHwInitializeAdc(void *intc, buckHwAdcIrqHandle_t irqhandle){

    uint32_t clkdiv;

    clkdiv = BUCK_HW_ADC_CLK / (BUCK_HW_CONFIG_ADC_SPI_FREQ_HZ << 1);

    zynqAxiAdcEnableWrite(BUCK_HW_CONFIG_ADC_BASE, 0);

    zynqAxiAdcInterruptEnableWrite(BUCK_HW_CONFIG_ADC_BASE, 1);

    zynqAxiAdcSpiClkDivWrite(BUCK_HW_CONFIG_ADC_BASE, clkdiv);

    zynqAxiAdcBufferAddressWrite(BUCK_HW_CONFIG_ADC_BASE, BUCK_HW_CONFIG_ADC_BUFFER);

    zynqAxiAdcInterruptConfig(intc, BUCK_HW_CONFIG_IRQ_PL_CPU1, irqhandle);

    zynqAxiAdcEnableWrite(BUCK_HW_CONFIG_ADC_BASE, 1);
}
//-----------------------------------------------------------------------------
static void buckHwInitializePwm(void){

    buckHwSetPwmReset(1);

    buckHwSetPwmInv(1);

    buckHwSetPwmFrequency(BUCK_HW_CONFIG_PWM_FREQ_HZ);
    buckHwSetPwmDuty(0.0f);
    buckHwSetPwmDeadTime(BUCK_HW_CONFIG_PWM_DEAD_TIME_NS);

    buckHwSetPwmOvfTriggerEnable(0);
    buckHwSetPwmOutputEnable(0);

    buckHwSetPwmReset(0);
}
//-----------------------------------------------------------------------------
static void buckHwInitializeGpio(void){

    XGpio_Config *cfg_ptr = 0;

    /* Initializes GPIOs */
    cfg_ptr = XGpio_LookupConfig(BUCK_HW_CONFIG_GPIO_ID);
    XGpio_CfgInitialize(&hwControl.gpio, cfg_ptr, cfg_ptr->BaseAddress);
    XGpio_SetDataDirection(&hwControl.gpio, BUCK_HW_CONFIG_GPIO_CHANNEL, 0);
}
//-----------------------------------------------------------------------------
static void buckHwInitializeMeasGains(void){


    hwControl.gains.i_o_gain = BUCK_CONFIG_IO_AVG_GAIN;
    hwControl.gains.i_o_ofs =  BUCK_CONFIG_IO_AVG_OFFS;

    hwControl.gains.i_l_gain = BUCK_CONFIG_IL_GAIN;
    hwControl.gains.i_l_ofs =  BUCK_CONFIG_IL_OFFS;
   
    hwControl.gains.v_dc_out_gain = BUCK_CONFIG_V_DC_OUT_GAIN;
    hwControl.gains.v_dc_out_ofs =  BUCK_CONFIG_V_DC_OUT_OFFS;

    hwControl.gains.v_out_gain = BUCK_CONFIG_V_OUT_GAIN;
    hwControl.gains.v_out_ofs = BUCK_CONFIG_V_OUT_OFFS;
    
    hwControl.gains.v_dc_in_gain = BUCK_CONFIG_V_DC_IN_GAIN;
    hwControl.gains.v_dc_in_ofs =  BUCK_CONFIG_V_DC_IN_OFFS;

    hwControl.gains.v_in_gain = BUCK_CONFIG_V_IN_GAIN;
    hwControl.gains.v_in_ofs =  BUCK_CONFIG_V_IN_OFFS;
}
//-----------------------------------------------------------------------------
//=============================================================================
#endif /* SOC_CPU1 */
