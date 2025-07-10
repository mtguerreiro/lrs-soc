/*
 * boostSbpHwIf.c
 *
 */
//#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "boostSbpHwIf.h"

#include "boostSbpHw.h"

#include "boostSbpConfig.h"

#include "rp/rp.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{
    rphandle_t handles[BOOST_SBP_HW_IF_END];
    rpctx_t rp;
}boostSbpHwIf_t;

typedef struct{

    boostSbpHwIf_t interface;
}boostSbpHwIfControl_t;

static boostSbpHwIfControl_t hwControl;
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static int32_t boostSbpHwIfSetPwmReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostSbpHwIfGetPwmReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostSbpHwIfSetPwmOutputEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostSbpHwIfGetPwmOutputEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostSbpHwIfSetPwmOvfTriggerEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostSbpHwIfGetPwmOvfTriggerEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostSbpHwIfSetPwmInv(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostSbpHwIfGetPwmInv(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostSbpHwIfSetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostSbpHwIfGetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostSbpHwIfSetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostSbpHwIfGetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostSbpHwIfSetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostSbpHwIfGetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostSbpHwIfSetAdcEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostSbpHwIfGetAdcEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostSbpHwIfSetAdcManualTrig(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostSbpHwIfGetAdcManualTrig(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostSbpHwIfSetAdcInterruptEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostSbpHwIfGetAdcInterruptEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostSbpHwIfSetAdcSpiFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostSbpHwIfGetAdcSpiFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostSbpHwIfSetInputPrech(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostSbpHwIfGetInputPrech(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostSbpHwIfSetInputDisch(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostSbpHwIfGetInputDisch(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostSbpHwIfSetInputSsrP(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostSbpHwIfGetInputSsrP(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostSbpHwIfSetInputSsrN(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostSbpHwIfGetInputSsrN(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostSbpHwIfSetOutputPrech(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostSbpHwIfGetOutputPrech(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostSbpHwIfSetOutputDisch(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostSbpHwIfGetOutputDisch(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostSbpHwIfSetOutputSsrP(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostSbpHwIfGetOutputSsrP(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostSbpHwIfSetOutputSsrN(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostSbpHwIfGetOutputSsrN(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostSbpHwIfSetInputSsr(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostSbpHwIfSetOutputSsr(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//static int32_t boostSbpHwIfSetOutputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//static int32_t boostSbpHwIfGetOutputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//
//static int32_t boostSbpHwIfSetLoadSwitch(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//static int32_t boostSbpHwIfGetLoadSwitch(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//
//static int32_t boostSbpHwIfSetMeasGains(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//static int32_t boostSbpHwIfGetMeasGains(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostSbpHwIfClearStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostSbpHwIfGetStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
int32_t boostSbpHwIfInitialize(void){

    /* Initializes the request processor */
    rpInitialize(&hwControl.interface.rp, BOOST_SBP_HW_IF_END, hwControl.interface.handles);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_SET_PWM_RESET, boostSbpHwIfSetPwmReset);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_GET_PWM_RESET, boostSbpHwIfGetPwmReset);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_SET_PWM_OUTPUT_ENABLE, boostSbpHwIfSetPwmOutputEnable);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_GET_PWM_OUTPUT_ENABLE, boostSbpHwIfGetPwmOutputEnable);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_SET_PWM_OVF_TRIGGER_ENABLE, boostSbpHwIfSetPwmOvfTriggerEnable);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_GET_PWM_OVF_TRIGGER_ENABLE, boostSbpHwIfGetPwmOvfTriggerEnable);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_SET_PWM_INV, boostSbpHwIfSetPwmInv);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_GET_PWM_INV, boostSbpHwIfGetPwmInv);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_SET_PWM_FREQ, boostSbpHwIfSetPwmFrequency);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_GET_PWM_FREQ, boostSbpHwIfGetPwmFrequency);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_SET_PWM_DUTY, boostSbpHwIfSetPwmDutyCycle);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_GET_PWM_DUTY, boostSbpHwIfGetPwmDutyCycle);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_SET_PWM_DEAD_TIME, boostSbpHwIfSetPwmDeadTime);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_GET_PWM_DEAD_TIME, boostSbpHwIfGetPwmDeadTime);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_SET_ADC_ENABLE, boostSbpHwIfSetAdcEnable);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_GET_ADC_ENABLE, boostSbpHwIfGetAdcEnable);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_SET_ADC_MANUAL_TRIG, boostSbpHwIfSetAdcManualTrig);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_GET_ADC_MANUAL_TRIG, boostSbpHwIfGetAdcManualTrig);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_SET_ADC_INT_ENABLE, boostSbpHwIfSetAdcInterruptEnable);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_GET_ADC_INT_ENABLE, boostSbpHwIfGetAdcInterruptEnable);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_SET_ADC_SPI_FREQ, boostSbpHwIfSetAdcSpiFreq);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_GET_ADC_SPI_FREQ, boostSbpHwIfGetAdcSpiFreq);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_SET_INPUT_PRECH, boostSbpHwIfSetInputPrech);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_GET_INPUT_PRECH, boostSbpHwIfGetInputPrech);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_SET_INPUT_DISCH, boostSbpHwIfSetInputDisch);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_GET_INPUT_DISCH, boostSbpHwIfGetInputDisch);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_SET_INPUT_SSR_P, boostSbpHwIfSetInputSsrP);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_GET_INPUT_SSR_P, boostSbpHwIfGetInputSsrP);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_SET_INPUT_SSR_N, boostSbpHwIfSetInputSsrN);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_GET_INPUT_SSR_N, boostSbpHwIfGetInputSsrN);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_SET_OUTPUT_PRECH, boostSbpHwIfSetOutputPrech);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_GET_OUTPUT_PRECH, boostSbpHwIfGetOutputPrech);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_SET_OUTPUT_DISCH, boostSbpHwIfSetOutputDisch);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_GET_OUTPUT_DISCH, boostSbpHwIfGetOutputDisch);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_SET_OUTPUT_SSR_P, boostSbpHwIfSetOutputSsrP);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_GET_OUTPUT_SSR_P, boostSbpHwIfGetOutputSsrP);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_SET_OUTPUT_SSR_N, boostSbpHwIfSetOutputSsrN);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_GET_OUTPUT_SSR_N, boostSbpHwIfGetOutputSsrN);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_SET_INPUT_SSR, boostSbpHwIfSetInputSsr);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_SET_OUTPUT_SSR, boostSbpHwIfSetOutputSsr);
//    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_SET_INPUT_RELAY, boostSbpHwIfSetInputRelay);
//    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_GET_INPUT_RELAY, boostSbpHwIfGetInputRelay);
//
//    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_SET_OUTPUT_RELAY, boostSbpHwIfSetOutputRelay);
//    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_GET_OUTPUT_RELAY, boostSbpHwIfGetOutputRelay);
//
//    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_SET_LOAD_SW, boostSbpHwIfSetLoadSwitch);
//    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_GET_LOAD_SW, boostSbpHwIfGetLoadSwitch);
//
//    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_SET_MEAS_GAINS, boostSbpHwIfSetMeasGains);
//    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_GET_MEAS_GAINS, boostSbpHwIfGetMeasGains);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_CLEAR_STATUS, boostSbpHwIfClearStatus);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_SBP_HW_IF_GET_STATUS, boostSbpHwIfGetStatus);

    return 0;
}
//-----------------------------------------------------------------------------
int32_t boostSbpHwIf(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    int32_t status;

    status = rpRequest(&hwControl.interface.rp, in, insize, out, maxoutsize);

    return status;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfSetPwmReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t reset;

    reset = *( (uint32_t *)in ) & 0x01;

    boostSbpHwSetPwmReset(reset);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfGetPwmReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t reset;

    reset = boostSbpHwGetPwmReset();

    *o = reset;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfSetPwmOutputEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t enable;

    enable = *( (uint32_t *)in ) & 0x01;

    boostSbpHwSetPwmOutputEnable(enable);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfGetPwmOutputEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t enable;

    enable = boostSbpHwGetPwmOutputEnable();

    *o = enable;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfSetPwmOvfTriggerEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t enable;

    enable = *( (uint32_t *)in ) & 0x01;

    boostSbpHwSetPwmOvfTriggerEnable(enable);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfGetPwmOvfTriggerEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t enable;

    enable = boostSbpHwGetPwmOvfTriggerEnable();

    *o = enable;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfSetPwmInv(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t enable;

    enable = *( (uint32_t *)in ) & 0x01;

    boostSbpHwSetPwmInv(enable);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfGetPwmInv(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t enable;

    enable = boostSbpHwGetPwmInv();

    *o = enable;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfSetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t freq;

    freq = *( (uint32_t *)in );

    boostSbpHwSetPwmFrequency(freq);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfGetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t freq;

    freq = boostSbpHwGetPwmFrequency();

    *o = freq;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfSetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float duty;

    duty = *( (float *)in );

    boostSbpHwSetPwmDuty(duty);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfGetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float *o = (float *)*out;
    float duty;

    duty = boostSbpHwGetPwmDuty();

    *o = duty;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfSetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float deadtime;

    deadtime = *( (float *)in );

    boostSbpHwSetPwmDeadTime(deadtime);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfGetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float *o = (float *)*out;
    float deadtime;

    deadtime = boostSbpHwGetPwmDeadTime();

    *o = deadtime;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfSetAdcEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t enable;

    enable = *( (uint32_t *)in ) & 0x01;

    boostSbpHwSetAdcEnable(enable);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfGetAdcEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t enable;

    enable = boostSbpHwGetAdcEnable();

    *o = enable;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfSetAdcManualTrig(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t trigger;

    trigger = *( (uint32_t *)in ) & 0x01;

    boostSbpHwSetAdcManualTrigger(trigger);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfGetAdcManualTrig(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t trigger;

    trigger = boostSbpHwGetAdcManualTrigger();

    *o = trigger;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfSetAdcInterruptEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t enable;

    enable = *( (uint32_t *)in ) & 0x01;

    boostSbpHwSetAdcInterruptEnable(enable);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfGetAdcInterruptEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t enable;

    enable = boostSbpHwGetAdcInterruptEnable();

    *o = enable;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfSetAdcSpiFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t freq;

    freq = *( (uint32_t *)in );

    boostSbpHwSetAdcSpiFreq(freq);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfGetAdcSpiFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t freq;

    freq = boostSbpHwGetAdcSpiFreq();

    *o = freq;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfSetInputPrech(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t state;

    state = *( (uint32_t *)in );

    boostSbpHwSetInputPrech(state);
    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfGetInputPrech(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t state;

    state = boostSbpHwGetInputPrech();

    *o = state;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfSetInputDisch(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t state;

    state = *( (uint32_t *)in );

    boostSbpHwSetInputDisch(state);
    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfGetInputDisch(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t state;

    state = boostSbpHwGetInputDisch();

    *o = state;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfSetInputSsrP(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t state;

    state = *( (uint32_t *)in );

    boostSbpHwSetInputSsrP(state);
    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfGetInputSsrP(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t state;

    state = boostSbpHwGetInputSsrP();

    *o = state;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfSetInputSsrN(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t state;

    state = *( (uint32_t *)in );

    boostSbpHwSetInputSsrN(state);
    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfGetInputSsrN(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t state;

    state = boostSbpHwGetInputSsrN();

    *o = state;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfSetOutputPrech(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t state;

    state = *( (uint32_t *)in );

    boostSbpHwSetOutputPrech(state);
    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfGetOutputPrech(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t state;

    state = boostSbpHwGetOutputPrech();

    *o = state;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfSetOutputDisch(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t state;

    state = *( (uint32_t *)in );

    boostSbpHwSetOutputDisch(state);
    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfGetOutputDisch(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t state;

    state = boostSbpHwGetOutputDisch();

    *o = state;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfSetOutputSsrP(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t state;

    state = *( (uint32_t *)in );

    boostSbpHwSetOutputSsrP(state);
    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfGetOutputSsrP(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t state;

    state = boostSbpHwGetOutputSsrP();

    *o = state;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfSetOutputSsrN(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t state;

    state = *( (uint32_t *)in );

    boostSbpHwSetOutputSsrN(state);
    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfGetOutputSsrN(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t state;

    state = boostSbpHwGetOutputSsrN();

    *o = state;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfSetInputSsr(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t state;

    state = *( (uint32_t *)in );

    boostSbpHwSetInputSsr(state);
    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfSetOutputSsr(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t state;

    state = *( (uint32_t *)in );

    boostSbpHwSetOutputSsr(state);
    return 0;
}
//-----------------------------------------------------------------------------
//static int32_t boostSbpHwIfSetMeasGains(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
//
//    boostSbpConfigMeasGains_t *gains;
//
//    gains = ( (boostSbpConfigMeasGains_t *)in );
//
//    boostSbpHwSetMeasGains(gains);
//
//    return 0;
//}
//-----------------------------------------------------------------------------
//static int32_t boostSbpHwIfGetMeasGains(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
//
//    boostSbpConfigMeasGains_t *o = (boostSbpConfigMeasGains_t *)*out;
//    boostSbpConfigMeasGains_t gains;
//    uint32_t size;
//
//    size = boostSbpHwGetMeasGains(&gains);
//
//    *o = gains;
//
//    return size;
//
//}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfClearStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    boostSbpHwStatusClear();

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostSbpHwIfGetStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t status;
    uint32_t *o = (uint32_t *)*out;

    status = boostSbpHwStatus();

    *o = status;

    return 4;
}
//-----------------------------------------------------------------------------
//=============================================================================
//#endif /* SOC_CPU1 */
