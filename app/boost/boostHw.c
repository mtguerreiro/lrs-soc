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
#define BOOST_HW_CONFIG_PWM_BASE              XPAR_AXI_PWM_V1_0_0_BASEADDR //XPAR_AXI_PWM_0_S00_AXI_BASEADDR //XPAR_LRSSOC_BD_AXI_PWM_1_0_BASEADDR
#define BOOST_HW_CONFIG_ADC_BASE              XPAR_ADC_PSCTL_V5_0_BASEADDR //XPAR_ADC_PSCTL_V2_0_0_BASEADDR //XPAR_ADC_PSCTL_0_S00_AXI_BASEADDR

#define BOOST_HW_CONFIG_IRQ_PL_CPU1_0         ZYNQ_CONFIG_IRQ_PL_TO_CPU1_0
#define BOOST_HW_CONFIG_IRQ_PL_CPU1_1         ZYNQ_CONFIG_IRQ_PL_TO_CPU1_1
#define BOOST_HW_CONFIG_IRQ_PL_CPU1_3         ZYNQ_CONFIG_IRQ_PL_TO_CPU1_3
#define BOOST_HW_CONFIG_IRQ_PL_CPU1_4         ZYNQ_CONFIG_IRQ_PL_TO_CPU1_4
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

//enabling adc protections
#define BOOST_HW_CONFIG_ADC_COMP_ENABLE 	 0x0
//protections
#define BOOST_HW_CONFIG_ADC_LIMIT1           0x0E920000  //Io inst.0
#define BOOST_HW_CONFIG_ADC_LIMIT2           0x09A10000  //IL inst.1
#define BOOST_HW_CONFIG_ADC_LIMIT3           0x03DB0269  //Vdcin inst.2
#define BOOST_HW_CONFIG_ADC_LIMIT4           0x03DB0269  //Vin inst.3
#define BOOST_HW_CONFIG_ADC_LIMIT5           0x09960678  //Vdcout inst.4
#define BOOST_HW_CONFIG_ADC_LIMIT6           0x0B7C04C4  //Vout inst.5



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
static void boostHwInitializeAdc(void *intc, boostHwAdcIrqHandle_t irqhandle0, boostHwAdcIrqHandle_t irqhandle1, boostHwAdcIrqHandle_t irqhandle3, boostHwAdcIrqHandle_t irqhandle4);
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

    boostHwInitializeAdc(config->intc, config->irqhandle0, config->irqhandle1, config->irqhandle3, config->irqhandle4);
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
void boostHwSetPwmInv(uint32_t enable){

    zynqAxiPwmInvWrite(BOOST_HW_CONFIG_PWM_BASE, enable);
}
//-----------------------------------------------------------------------------
uint32_t boostHwGetPwmInv(void){

    return zynqAxiPwmInvRead(BOOST_HW_CONFIG_PWM_BASE);
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
void boostHwSetAdcCompReset(uint32_t enable){

    zynqAxiAdcCompResetWrite(BOOST_HW_CONFIG_ADC_BASE, enable);
}
//-----------------------------------------------------------------------------
uint32_t boostHwGetAdcCompReset(void){

    return zynqAxiAdcCompResetRead(BOOST_HW_CONFIG_ADC_BASE);
}

//-----------------------------------------------------------------------------
void boostHwSetAdcDoneIntFactor(uint8_t factor){



    zynqAxiAdcDoneIntFactorWrite(BOOST_HW_CONFIG_ADC_BASE, factor);
}
//-----------------------------------------------------------------------------
uint8_t boostHwGetAdcDoneIntFactor(void){

    uint8_t factor;

    factor = zynqAxiAdcDoneIntFactorRead(BOOST_HW_CONFIG_ADC_BASE);

    return factor;
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
void boostHwSetAdcCompEnable(uint32_t enable){

    zynqAxiAdcCompEnableWrite(BOOST_HW_CONFIG_ADC_BASE, enable);
}
//-----------------------------------------------------------------------------
uint32_t boostHwGetAdcCompEnable(void){

    return zynqAxiAdcCompEnableRead(BOOST_HW_CONFIG_ADC_BASE);
}
//-----------------------------------------------------------------------------
void boostHwSetAdcCompEnableBit(uint32_t enable, uint32_t instantiation){

	zynqAxiAdcCompEnablebitWrite(BOOST_HW_CONFIG_ADC_BASE, enable, instantiation);
}
//-----------------------------------------------------------------------------
uint32_t boostHwGetAdcCompEnableBit(SingleComparatorInst instantiation){

    return zynqAxiAdcCompEnablebitRead(BOOST_HW_CONFIG_ADC_BASE, instantiation);
}


//------------------------------------------------------------------------------
void boostHwSetAdcCompTripLimits(float limit_max, float limit_min, uint32_t instantiation){

	uint32_t limit_raw = 0;
	float offset = 0;
	float gain = 0;
    switch (instantiation) {
        case SINGLE_COMPARATOR_INSTANTIATION_1: //Io
            offset = BOOST_CONFIG_IO_AVG_OFFS;
            gain = BOOST_CONFIG_IO_AVG_GAIN;
            break;
        case SINGLE_COMPARATOR_INSTANTIATION_2: //IL
            offset = BOOST_CONFIG_IL_OFFS;
            gain = BOOST_CONFIG_IL_GAIN;
            break;
        case SINGLE_COMPARATOR_INSTANTIATION_3: //Vdcin
            offset = BOOST_CONFIG_V_DC_IN_OFFS;
            gain = BOOST_CONFIG_V_DC_IN_GAIN;
            break;
        case SINGLE_COMPARATOR_INSTANTIATION_4: //Vin
            offset = BOOST_CONFIG_V_IN_OFFS;
            gain = BOOST_CONFIG_V_IN_GAIN;
            break;
        case SINGLE_COMPARATOR_INSTANTIATION_5: //Vdcout
            offset = BOOST_CONFIG_V_DC_OUT_OFFS;
            gain = BOOST_CONFIG_V_DC_OUT_GAIN;
            break;
        case SINGLE_COMPARATOR_INSTANTIATION_6: //Vout
            offset = BOOST_CONFIG_V_OUT_OFFS;
            gain = BOOST_CONFIG_V_OUT_GAIN;
            break;
    }
    float upper_limit_dec = (limit_max - offset) / gain;
    float lower_limit_dec = (limit_min - offset) / gain;
    uint16_t upper_limit = (uint16_t)(upper_limit_dec);
    uint16_t lower_limit = (uint16_t)(lower_limit_dec);

    if(upper_limit >= lower_limit){
	// Concatenate
	limit_raw = ((uint32_t)upper_limit << 16) | (uint32_t)lower_limit;
    }
    else { //for iL, where slope is negative
    limit_raw = ((uint32_t)lower_limit << 16) | (uint32_t)upper_limit;
    }
	zynqAxiAdcTripWrite(BOOST_HW_CONFIG_ADC_BASE, limit_raw, instantiation);

}
//-----------------------------------------------------------------------------
float boostHwGetAdcCompTripLimits(SingleComparatorInst instantiation, int max){

	uint32_t limit_raw = zynqAxiAdcTripRead(BOOST_HW_CONFIG_ADC_BASE, instantiation);
	uint16_t lower_limit = (uint16_t)(limit_raw & 0xFFFF);
	uint16_t upper_limit = (uint16_t)((limit_raw >> 16) & 0xFFFF);


	float offset, gain;
    switch (instantiation) {
        case SINGLE_COMPARATOR_INSTANTIATION_1: //Io
            offset = BOOST_CONFIG_IO_AVG_OFFS;
            gain = BOOST_CONFIG_IO_AVG_GAIN;
            break;
        case SINGLE_COMPARATOR_INSTANTIATION_2: //IL
            offset = BOOST_CONFIG_IL_OFFS;
            gain = BOOST_CONFIG_IL_GAIN;
            break;
        case SINGLE_COMPARATOR_INSTANTIATION_3: //Vdcin
            offset = BOOST_CONFIG_V_DC_IN_OFFS;
            gain = BOOST_CONFIG_V_DC_IN_GAIN;
            break;
        case SINGLE_COMPARATOR_INSTANTIATION_4: //Vin
            offset = BOOST_CONFIG_V_IN_OFFS;
            gain = BOOST_CONFIG_V_IN_GAIN;
            break;
        case SINGLE_COMPARATOR_INSTANTIATION_5: //Vdcout
            offset = BOOST_CONFIG_V_DC_OUT_OFFS;
            gain = BOOST_CONFIG_V_DC_OUT_GAIN;
            break;
        case SINGLE_COMPARATOR_INSTANTIATION_6: //Vout
            offset = BOOST_CONFIG_V_OUT_OFFS;
            gain = BOOST_CONFIG_V_OUT_GAIN;
            break;
    }

    float limit_max = ((float)(upper_limit))*gain + offset;
	float limit_min = ((float)(lower_limit))*gain + offset;


	if (max)
		return  limit_max;
	else
		return  limit_min;


}

//------------------------------------------------------------------------------
uint32_t boostHwGetAdcCompResult(void){

    return zynqAxiAdcCompResultRead(BOOST_HW_CONFIG_ADC_BASE);
}


//-----------------------------------------------------------------------------
int32_t boostHwGetMeasurements(void *meas){

    boostConfigMeasurements_t *dst;
    uint16_t *src;

    src = (uint16_t *)BOOST_HW_CONFIG_ADC_BUFFER;
    dst = (boostConfigMeasurements_t *)meas;

    /* Measurements */
    dst->i_o =  hwControl.gains.i_o_gain * ((float)(*src++)) + hwControl.gains.i_o_ofs;
    dst->i_l =  hwControl.gains.i_l_gain * ((float)(*src++)) + hwControl.gains.i_l_ofs;
    dst->v_dc_in = hwControl.gains.v_dc_in_gain * ((float)(*src++)) + hwControl.gains.v_dc_in_ofs;
    dst->v_in = hwControl.gains.v_in_gain * ((float)(*src++)) + hwControl.gains.v_in_ofs;
    dst->v_dc_out = hwControl.gains.v_dc_out_gain * ((float)(*src++)) + hwControl.gains.v_dc_out_ofs;
    dst->v_out  = hwControl.gains.v_out_gain * ((float)(*src++)) + hwControl.gains.v_out_ofs;

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
static void boostHwInitializeAdc(void *intc, boostHwAdcIrqHandle_t irqhandle0, boostHwAdcIrqHandle_t irqhandle1, boostHwAdcIrqHandle_t irqhandle3, boostHwAdcIrqHandle_t irqhandle4){

    uint32_t clkdiv;

    clkdiv = BOOST_HW_ADC_CLK / (BOOST_HW_CONFIG_ADC_SPI_FREQ_HZ << 1);

    zynqAxiAdcEnableWrite(BOOST_HW_CONFIG_ADC_BASE, 0);

    zynqAxiAdcInterruptEnableWrite(BOOST_HW_CONFIG_ADC_BASE, 1);

    zynqAxiAdcSpiClkDivWrite(BOOST_HW_CONFIG_ADC_BASE, clkdiv);

    zynqAxiAdcBufferAddressWrite(BOOST_HW_CONFIG_ADC_BASE, BOOST_HW_CONFIG_ADC_BUFFER);

    zynqAxiAdcInterrupt0Config(intc, BOOST_HW_CONFIG_IRQ_PL_CPU1_0, irqhandle0);
    zynqAxiAdcInterrupt1Config(intc, BOOST_HW_CONFIG_IRQ_PL_CPU1_1, irqhandle1);
    zynqAxiAdcInterrupt3Config(intc, BOOST_HW_CONFIG_IRQ_PL_CPU1_3, irqhandle3);
    zynqAxiAdcInterrupt4Config(intc, BOOST_HW_CONFIG_IRQ_PL_CPU1_4, irqhandle4);

    zynqAxiAdcEnableWrite(BOOST_HW_CONFIG_ADC_BASE, 1);
    boostHwSetAdcDoneIntFactor(0); //done_int_factor set 0 by default

    zynqAxiAdcCompResetWrite(BOOST_HW_CONFIG_ADC_BASE, 1);
    zynqAxiAdcCompResetWrite(BOOST_HW_CONFIG_ADC_BASE, 0);
    zynqAxiAdcCompEnableWrite(BOOST_HW_CONFIG_ADC_BASE, BOOST_HW_CONFIG_ADC_COMP_ENABLE);


    zynqAxiAdcTripWrite(BOOST_HW_CONFIG_ADC_BASE, BOOST_HW_CONFIG_ADC_LIMIT1, SINGLE_COMPARATOR_INSTANTIATION_1); //Io
    zynqAxiAdcTripWrite(BOOST_HW_CONFIG_ADC_BASE, BOOST_HW_CONFIG_ADC_LIMIT2, SINGLE_COMPARATOR_INSTANTIATION_2); //IL
    zynqAxiAdcTripWrite(BOOST_HW_CONFIG_ADC_BASE, BOOST_HW_CONFIG_ADC_LIMIT3, SINGLE_COMPARATOR_INSTANTIATION_3); //Vdcin
    zynqAxiAdcTripWrite(BOOST_HW_CONFIG_ADC_BASE, BOOST_HW_CONFIG_ADC_LIMIT4, SINGLE_COMPARATOR_INSTANTIATION_4); //Vin
    zynqAxiAdcTripWrite(BOOST_HW_CONFIG_ADC_BASE, BOOST_HW_CONFIG_ADC_LIMIT5, SINGLE_COMPARATOR_INSTANTIATION_5); //Vdcout
    zynqAxiAdcTripWrite(BOOST_HW_CONFIG_ADC_BASE, BOOST_HW_CONFIG_ADC_LIMIT6, SINGLE_COMPARATOR_INSTANTIATION_6); //Vout




    //--------------------------------------------------------------------------
}
//-----------------------------------------------------------------------------
static void boostHwInitializePwm(void){

    boostHwSetPwmReset(1);

    boostHwSetPwmInv(1);


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

    hwControl.gains.i_o_gain = BOOST_CONFIG_IO_AVG_GAIN;
    hwControl.gains.i_o_ofs =  BOOST_CONFIG_IO_AVG_OFFS;

    hwControl.gains.i_l_gain = BOOST_CONFIG_IL_GAIN;
    hwControl.gains.i_l_ofs =  BOOST_CONFIG_IL_OFFS;

    hwControl.gains.i_l_avg_gain = BOOST_CONFIG_IL_AVG_GAIN;
    hwControl.gains.i_l_avg_ofs =  BOOST_CONFIG_IL_AVG_OFFS;
    
    hwControl.gains.v_dc_in_gain = BOOST_CONFIG_V_DC_IN_GAIN;
    hwControl.gains.v_dc_in_ofs =  BOOST_CONFIG_V_DC_IN_OFFS;

    hwControl.gains.v_in_gain = BOOST_CONFIG_V_IN_GAIN;
    hwControl.gains.v_in_ofs =  BOOST_CONFIG_V_IN_OFFS;

    hwControl.gains.v_dc_out_gain = BOOST_CONFIG_V_DC_OUT_GAIN;
    hwControl.gains.v_dc_out_ofs =  BOOST_CONFIG_V_DC_OUT_OFFS;

    hwControl.gains.v_out_gain = BOOST_CONFIG_V_OUT_GAIN;
    hwControl.gains.v_out_ofs = BOOST_CONFIG_V_OUT_OFFS;
}
//-----------------------------------------------------------------------------
//=============================================================================
#endif /* SOC_CPU1 */
