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

#include "xgpio.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define CUK_HW_CONFIG_ADC_SPI_FREQ_HZ      ((uint32_t)16666666)
#define CUK_HW_CONFIG_PWM_FREQ_HZ          ((uint32_t) 100000 )
#define CUK_HW_CONFIG_PWM_DEAD_TIME_NS     ((float) 200e-9 )
#define CUK_HW_CONFIG_PWM_BASE              XPAR_AXI_PWM_0_S00_AXI_BASEADDR
#define CUK_HW_CONFIG_ADC_BASE              XPAR_ADC_PSCTL_0_S00_AXI_BASEADDR

#define CUK_HW_CONFIG_IRQ_PL_CPU1           ZYNQ_CONFIG_IRQ_PL_TO_CPU1
#define CUK_HW_CONFIG_ADC_BUFFER            ZYNQ_CONFIG_MEM_PL_TO_CPU1_ADR

#define CUK_HW_CONFIG_GPIO_ID               XPAR_AXI_GPIO_0_DEVICE_ID
#define CUK_HW_CONFIG_GPIO_CHANNEL          1
#define CUK_HW_CONFIG_GPIO_MASK             0b11

#define CUK_HW_CONFIG_GPIO_OUTPUT_OFFS      (0U)
#define CUK_HW_CONFIG_GPIO_OUTPUT           (1 << CUK_HW_CONFIG_GPIO_OUTPUT_OFFS)
#define CUK_HW_CONFIG_GPIO_LOAD_OFFS        (1U)
#define CUK_HW_CONFIG_GPIO_LOAD             (1 << CUK_HW_CONFIG_GPIO_LOAD_OFFS)

/* PWM peripheral clock, in Hz */
#define CUK_HW_PWM_CLK                      100000000
#define CUK_HW_ADC_CLK                      100000000

typedef struct{

    uint32_t status;

    uint32_t pwmPeriod;

    cukConfigMeasurements_t meas;
    cukConfigControl_t control;

    XGpio gpio;

    cukConfigMeasGains_t gains;

    float alpha;

}cukHwControl_t;
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static void cukHwInitializeAdc(void *intc, cukHwAdcIrqHandle_t irqhandle);
static void cukHwInitializePwm(void);
static void cukHwInitializeGpio(void);
static void cukHwInitializeMeasGains(void);
static float cukHwExpMovAvg(float sample, float average);
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static cukHwControl_t hwControl = {.pwmPeriod = 0, .status = 0, .alpha = 0.2f};
static float i_i_filt = 0.0f, i_1_filt = 0.0f, i_o_filt = 0.0f, i_2_filt = 0.0f;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t cukHwInitialize(cukHwInitConfig_t *config){

    cukHwInitializeAdc(config->intc, config->irqhandle);
    cukHwInitializePwm();
    cukHwInitializeGpio();
    cukHwInitializeMeasGains();

    return 0;
}
//-----------------------------------------------------------------------------
int32_t cukHwStatus(void){

    return hwControl.status;
}
//-----------------------------------------------------------------------------
void cukHwStatusClear(void){

    hwControl.status = 0;
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


    //-------------------------------------------------------------------------
    // Sensor-based measurements
    //-------------------------------------------------------------------------
    /*
     * Skips the first adc channel of header. Each Cuk connector has five
     * measurements, but the ADC board has 6 channels. Thus, we skip the
     * first channel.
     */
    src++;

    dst->i_i =  hwControl.gains.i_i_gain * ((float)(*src++)) + hwControl.gains.i_i_ofs;
    dst->i_1 =  hwControl.gains.i_1_gain * ((float)(*src++)) + hwControl.gains.i_1_ofs;

    dst->v_in = hwControl.gains.v_in_gain * ((float)(*src++)) + hwControl.gains.v_in_ofs;
    dst->v_dc = hwControl.gains.v_dc_gain * ((float)(*src++)) + hwControl.gains.v_dc_ofs;
    dst->v_1  = hwControl.gains.v_1_gain * ((float)(*src++)) + hwControl.gains.v_1_ofs;

    /* Skips the seventh adc channel of header */
    src++;
    dst->i_o =  -( hwControl.gains.i_o_gain * ((float)(*src++)) + hwControl.gains.i_o_ofs );
    dst->i_2 =  -( hwControl.gains.i_2_gain * ((float)(*src++)) + hwControl.gains.i_2_ofs );

    dst->v_out =    hwControl.gains.v_out_gain * ((float)(*src++)) + hwControl.gains.v_out_ofs;
    dst->v_dc_out = hwControl.gains.v_dc_out_gain * ((float)(*src++)) + hwControl.gains.v_dc_out_ofs;
    dst->v_2 =      hwControl.gains.v_2_gain * ((float)(*src++)) + hwControl.gains.v_2_ofs;
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    // Software-based measurements
    //-------------------------------------------------------------------------
    dst->i_i_filt = 0.0f;
    dst->i_1_filt = 0.0f;

    dst->v_in_filt = 0.0f;
    dst->v_dc_filt = 0.0f;
    dst->v_1_filt  = 0.0f;

    dst->i_o_filt = 0.0f;
    dst->i_2_filt = 0.0f;

    dst->v_out_filt =    0.0f;
    dst->v_dc_out_filt = 0.0f;
    dst->v_2_filt =      0.0f;

    i_1_filt = cukHwExpMovAvg(dst->i_1, i_1_filt);
    dst->i_1_filt = i_1_filt;

    i_i_filt = cukHwExpMovAvg(dst->i_i, i_i_filt);
    dst->i_i_filt = i_i_filt;

    i_o_filt = cukHwExpMovAvg(dst->i_o, i_o_filt);
    dst->i_o_filt = i_o_filt;

    i_2_filt = cukHwExpMovAvg(dst->i_2, i_2_filt);
    dst->i_2_filt = i_2_filt;

    dst->p_in = dst->i_1 * dst->v_dc;
    dst->p_out = i_o_filt * dst->v_dc_out;
    //-------------------------------------------------------------------------

    /* Protection */
    if( (dst->i_i > CUK_CONFIG_I_PRIM_LIM) || (dst->i_1 > CUK_CONFIG_I_PRIM_LIM) ) hwControl.status = 1;
    if( (dst->i_i < -CUK_CONFIG_I_PRIM_LIM) || (dst->i_1 < -CUK_CONFIG_I_PRIM_LIM) ) hwControl.status = 1;

    if( (dst->v_in > CUK_CONFIG_V_PRIM_LIM) || (dst->v_dc > CUK_CONFIG_V_PRIM_LIM) || (dst->v_1 > CUK_CONFIG_V_PRIM_LIM) ) hwControl.status = 1;

    if( (dst->i_2 > CUK_CONFIG_I_SEC_LIM) ) hwControl.status = 1;
    if( (dst->i_2 < -CUK_CONFIG_I_SEC_LIM) ) hwControl.status = 1;

    if( (dst->v_out > CUK_CONFIG_V_SEC_LIM) || (dst->v_dc_out > CUK_CONFIG_V_SEC_LIM) || (dst->v_2 > CUK_CONFIG_V_SEC_LIM) ) hwControl.status = 1;

    if( hwControl.status != 0 ){
        //cukHwSetPwmOutputEnable(0);
        cukHwShutDown();
        return -1;
    }
    else
        return sizeof(cukConfigMeasurements_t);
}
//-----------------------------------------------------------------------------
int32_t cukHwApplyOutputs(void *outputs, int32_t size){

    cukConfigControl_t *control;

    control = (cukConfigControl_t *)outputs;

    cukHwSetPwmDuty(control->u);

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
void cukHwSetLoadSwitch(uint32_t state){

    uint32_t gpio;

    state = (state & 0x01) << CUK_HW_CONFIG_GPIO_LOAD_OFFS;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, CUK_HW_CONFIG_GPIO_CHANNEL) & (~CUK_HW_CONFIG_GPIO_LOAD);

    gpio = gpio | state;

    XGpio_DiscreteWrite(&hwControl.gpio, CUK_HW_CONFIG_GPIO_CHANNEL, gpio);
}
//-----------------------------------------------------------------------------
uint32_t cukHwGetLoadSwitch(void){

    uint32_t gpio;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, CUK_HW_CONFIG_GPIO_CHANNEL) & (CUK_HW_CONFIG_GPIO_LOAD);

    gpio = gpio >> CUK_HW_CONFIG_GPIO_LOAD_OFFS;

    return gpio;
}
//-----------------------------------------------------------------------------
void cukHwSetOutputSwitch(uint32_t state){

    uint32_t gpio;

    state = (state & 0x01) << CUK_HW_CONFIG_GPIO_OUTPUT_OFFS;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, CUK_HW_CONFIG_GPIO_CHANNEL) & (~CUK_HW_CONFIG_GPIO_OUTPUT);

    gpio = gpio | state;

    XGpio_DiscreteWrite(&hwControl.gpio, CUK_HW_CONFIG_GPIO_CHANNEL, gpio);
}
//-----------------------------------------------------------------------------
uint32_t cukHwGetOutputSwitch(void){

    uint32_t gpio;

    gpio = XGpio_DiscreteRead(&hwControl.gpio, CUK_HW_CONFIG_GPIO_CHANNEL) & (CUK_HW_CONFIG_GPIO_OUTPUT);

    gpio = gpio >> CUK_HW_CONFIG_GPIO_OUTPUT_OFFS;

    return gpio;
}
//-----------------------------------------------------------------------------
void cukHwSetMeasGains(cukConfigMeasGains_t *gains){

    hwControl.gains = *gains;
}
//-----------------------------------------------------------------------------
uint32_t cukHwGetMeasGains(cukConfigMeasGains_t *gains){

    *gains = hwControl.gains;

    return sizeof(cukConfigMeasGains_t);
}
//-----------------------------------------------------------------------------
void cukHwSetFilterCoef(float alpha){

    hwControl.alpha = alpha;
}
//-----------------------------------------------------------------------------
float cukHwGetFilterCoef(void){

    return hwControl.alpha;
}
//-----------------------------------------------------------------------------
void cukHwShutDown(void){

    float u;

    u = cukHwGetPwmDuty();

    u = u - CUK_CONFIG_SHUTDOWN_U_DEC;
    if( u < 0.0f ){
        //cukHwSetPwmOutputEnable(0);
        u = 0.0f;
    }

    cukHwSetPwmDuty(u);
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
static void cukHwInitializeGpio(void){

    XGpio_Config *cfg_ptr = 0;

    /* Initializes GPIOs */
    cfg_ptr = XGpio_LookupConfig(CUK_HW_CONFIG_GPIO_ID);
    XGpio_CfgInitialize(&hwControl.gpio, cfg_ptr, cfg_ptr->BaseAddress);
    XGpio_SetDataDirection(&hwControl.gpio, CUK_HW_CONFIG_GPIO_CHANNEL, 0);
}
//-----------------------------------------------------------------------------
static void cukHwInitializeMeasGains(void){

    hwControl.gains.i_i_gain = 0.012527748670796679f;
    hwControl.gains.i_i_ofs =  -25.09620948301267f;

    hwControl.gains.i_1_gain = 0.02574140041460996f;
    hwControl.gains.i_1_ofs =  -51.723993428472845f;

    hwControl.gains.v_in_gain = 0.014919542630730998f;
    hwControl.gains.v_in_ofs =  0.02983490778952813f;

    hwControl.gains.v_dc_gain = 0.01490254584622419f;
    hwControl.gains.v_dc_ofs =  0.014667383672769319f;

    hwControl.gains.v_1_gain = 0.014919540229885057f;
    hwControl.gains.v_1_ofs = 0.014919540229886508f;

    hwControl.gains.i_o_gain = 0.012403521044364872;
    hwControl.gains.i_o_ofs =  -24.842671890874133;

    hwControl.gains.i_2_gain = 0.025902038490429217;
    hwControl.gains.i_2_ofs =  -52.28905224441168;

    hwControl.gains.v_out_gain = 0.01490621920998692f;
    hwControl.gains.v_out_ofs =  0.021628758244869317f;

    hwControl.gains.v_dc_out_gain = 0.014901654476279058f;
    hwControl.gains.v_dc_out_ofs =  0.04172051494698792f;

    hwControl.gains.v_2_gain = 0.01487120334913248f;
    hwControl.gains.v_2_ofs =  0.058881268243716534f;
}
//-----------------------------------------------------------------------------
static float cukHwExpMovAvg(float sample, float average){

    return hwControl.alpha * sample + (1.0f - hwControl.alpha) * average;
}
//-----------------------------------------------------------------------------
//=============================================================================
#endif /* SOC_CPU1 */
