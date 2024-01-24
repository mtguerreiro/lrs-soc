/*
 * boostHwIf.c
 *
 */
#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "boostHwIf.h"

#ifdef BOOST_HW_IF_CONFIG_OPIL
#include "boostHwOpil.h"
#else
#include "boostHw.h"
#endif

#include "boostConfig.h"

#include "../utils/rp.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{
    rphandle_t handles[BOOST_HW_IF_END];
    rpctx_t rp;
}boostHwIf_t;

typedef struct{

    boostHwIf_t interface;
}boostHwIfControl_t;

static boostHwIfControl_t hwControl;
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static int32_t boostHwIfSetPwmReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostHwIfGetPwmReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostHwIfSetPwmOutputEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostHwIfGetPwmOutputEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostHwIfSetPwmOvfTriggerEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostHwIfGetPwmOvfTriggerEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostHwIfSetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostHwIfGetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostHwIfSetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostHwIfGetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostHwIfSetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostHwIfGetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostHwIfSetAdcEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostHwIfGetAdcEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostHwIfSetAdcManualTrig(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostHwIfGetAdcManualTrig(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostHwIfSetAdcInterruptEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostHwIfGetAdcInterruptEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostHwIfSetAdcSpiFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostHwIfGetAdcSpiFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostHwIfSetInputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostHwIfGetInputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostHwIfSetOutputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostHwIfGetOutputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostHwIfSetMeasGains(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostHwIfGetMeasGains(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t boostHwIfClearStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t boostHwIfGetStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
int32_t boostHwIfInitialize(void){

    /* Initializes the request processor */
    rpInitialize(&hwControl.interface.rp, BOOST_HW_IF_END, hwControl.interface.handles);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_HW_IF_SET_PWM_RESET, boostHwIfSetPwmReset);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_HW_IF_GET_PWM_RESET, boostHwIfGetPwmReset);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_HW_IF_SET_PWM_OUTPUT_ENABLE, boostHwIfSetPwmOutputEnable);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_HW_IF_GET_PWM_OUTPUT_ENABLE, boostHwIfGetPwmOutputEnable);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_HW_IF_SET_PWM_OVF_TRIGGER_ENABLE, boostHwIfSetPwmOvfTriggerEnable);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_HW_IF_GET_PWM_OVF_TRIGGER_ENABLE, boostHwIfGetPwmOvfTriggerEnable);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_HW_IF_SET_PWM_FREQ, boostHwIfSetPwmFrequency);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_HW_IF_GET_PWM_FREQ, boostHwIfGetPwmFrequency);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_HW_IF_SET_PWM_DUTY, boostHwIfSetPwmDutyCycle);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_HW_IF_GET_PWM_DUTY, boostHwIfGetPwmDutyCycle);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_HW_IF_SET_PWM_DEAD_TIME, boostHwIfSetPwmDeadTime);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_HW_IF_GET_PWM_DEAD_TIME, boostHwIfGetPwmDeadTime);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_HW_IF_SET_ADC_ENABLE, boostHwIfSetAdcEnable);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_HW_IF_GET_ADC_ENABLE, boostHwIfGetAdcEnable);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_HW_IF_SET_ADC_MANUAL_TRIG, boostHwIfSetAdcManualTrig);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_HW_IF_GET_ADC_MANUAL_TRIG, boostHwIfGetAdcManualTrig);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_HW_IF_SET_ADC_INT_ENABLE, boostHwIfSetAdcInterruptEnable);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_HW_IF_GET_ADC_INT_ENABLE, boostHwIfGetAdcInterruptEnable);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_HW_IF_SET_ADC_SPI_FREQ, boostHwIfSetAdcSpiFreq);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_HW_IF_GET_ADC_SPI_FREQ, boostHwIfGetAdcSpiFreq);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_HW_IF_SET_INPUT_RELAY, boostHwIfSetInputRelay);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_HW_IF_GET_INPUT_RELAY, boostHwIfGetInputRelay);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_HW_IF_SET_OUTPUT_RELAY, boostHwIfSetOutputRelay);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_HW_IF_GET_OUTPUT_RELAY, boostHwIfGetOutputRelay);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_HW_IF_SET_MEAS_GAINS, boostHwIfSetMeasGains);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_HW_IF_GET_MEAS_GAINS, boostHwIfGetMeasGains);

    rpRegisterHandle(&hwControl.interface.rp, BOOST_HW_IF_CLEAR_STATUS, boostHwIfClearStatus);
    rpRegisterHandle(&hwControl.interface.rp, BOOST_HW_IF_GET_STATUS, boostHwIfGetStatus);

    return 0;
}
//-----------------------------------------------------------------------------
int32_t boostHwIf(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

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
static int32_t boostHwIfSetPwmReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t reset;

    reset = *( (uint32_t *)in ) & 0x01;

#ifdef BOOST_HW_IF_CONFIG_OPIL
    boostHwOpilSetPwmReset(reset);
#else
    boostHwSetPwmReset(reset);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostHwIfGetPwmReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t reset;

#ifdef BOOST_HW_IF_CONFIG_OPIL
    reset = boostHwOpilGetPwmReset();
#else
    reset = boostHwGetPwmReset();
#endif

    *o = reset;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostHwIfSetPwmOutputEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t enable;

    enable = *( (uint32_t *)in ) & 0x01;

#ifdef BOOST_HW_IF_CONFIG_OPIL
    boostHwOpilSetPwmOutputEnable(enable);
#else
    boostHwSetPwmOutputEnable(enable);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostHwIfGetPwmOutputEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t enable;

#ifdef BOOST_HW_IF_CONFIG_OPIL
    enable = boostHwOpilGetPwmOutputEnable();
#else
    enable = boostHwGetPwmOutputEnable();
#endif

    *o = enable;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostHwIfSetPwmOvfTriggerEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t enable;

    enable = *( (uint32_t *)in ) & 0x01;

#ifdef BOOST_HW_IF_CONFIG_OPIL
    boostHwOpilSetPwmOvfTriggerEnable(enable);
#else
    boostHwSetPwmOvfTriggerEnable(enable);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostHwIfGetPwmOvfTriggerEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t enable;

#ifdef BOOST_HW_IF_CONFIG_OPIL
    enable = boostHwOpilGetPwmOvfTriggerEnable();
#else
    enable = boostHwGetPwmOvfTriggerEnable();
#endif

    *o = enable;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostHwIfSetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t freq;

    freq = *( (uint32_t *)in );

#ifdef BOOST_HW_IF_CONFIG_OPIL
    boostHwOpilSetPwmFrequency(freq);
#else
    boostHwSetPwmFrequency(freq);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostHwIfGetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t freq;

#ifdef BOOST_HW_IF_CONFIG_OPIL
    freq = boostHwOpilGetPwmFrequency();
#else
    freq = boostHwGetPwmFrequency();
#endif

    *o = freq;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostHwIfSetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float duty;

    duty = *( (float *)in );

#ifdef BOOST_HW_IF_CONFIG_OPIL
    boostHwOpilSetPwmDuty(duty);
#else
    boostHwSetPwmDuty(duty);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostHwIfGetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float *o = (float *)*out;
    float duty;

#ifdef BOOST_HW_IF_CONFIG_OPIL
    duty = boostHwOpilGetPwmDuty();
#else
    duty = boostHwGetPwmDuty();
#endif

    *o = duty;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostHwIfSetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float deadtime;

    deadtime = *( (float *)in );

#ifdef BOOST_HW_IF_CONFIG_OPIL
    boostHwOpilSetPwmDeadTime(deadtime);
#else
    boostHwSetPwmDeadTime(deadtime);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostHwIfGetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float *o = (float *)*out;
    float deadtime;

#ifdef BOOST_HW_IF_CONFIG_OPIL
    deadtime = boostHwOpilGetPwmDeadTime();
#else
    deadtime = boostHwGetPwmDeadTime();
#endif

    *o = deadtime;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostHwIfSetAdcEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t enable;

    enable = *( (uint32_t *)in ) & 0x01;

#ifdef BOOST_HW_IF_CONFIG_OPIL
    boostHwOpilSetAdcEnable(enable);
#else
    boostHwSetAdcEnable(enable);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostHwIfGetAdcEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t enable;

#ifdef BOOST_HW_IF_CONFIG_OPIL
    enable = boostHwOpilGetAdcEnable();
#else
    enable = boostHwGetAdcEnable();
#endif

    *o = enable;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostHwIfSetAdcManualTrig(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t trigger;

    trigger = *( (uint32_t *)in ) & 0x01;

#ifdef BOOST_HW_IF_CONFIG_OPIL
    boostHwOpilSetAdcManualTrigger(trigger);
#else
    boostHwSetAdcManualTrigger(trigger);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostHwIfGetAdcManualTrig(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t trigger;

#ifdef BOOST_HW_IF_CONFIG_OPIL
    trigger = boostHwOpilGetAdcManualTrigger();
#else
    trigger = boostHwGetAdcManualTrigger();
#endif

    *o = trigger;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostHwIfSetAdcInterruptEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t enable;

    enable = *( (uint32_t *)in ) & 0x01;

#ifdef BOOST_HW_IF_CONFIG_OPIL
    boostHwOpilSetAdcInterruptEnable(enable);
#else
    boostHwSetAdcInterruptEnable(enable);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostHwIfGetAdcInterruptEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t enable;

#ifdef BOOST_HW_IF_CONFIG_OPIL
    enable = boostHwOpilGetAdcInterruptEnable();
#else
    enable = boostHwGetAdcInterruptEnable();
#endif

    *o = enable;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostHwIfSetAdcSpiFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t freq;

    freq = *( (uint32_t *)in );

#ifdef BOOST_HW_IF_CONFIG_OPIL
    boostHwOpilSetAdcSpiFreq(freq);
#else
    boostHwSetAdcSpiFreq(freq);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostHwIfGetAdcSpiFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t freq;

#ifdef BOOST_HW_IF_CONFIG_OPIL
    freq = boostHwOpilGetAdcSpiFreq();
#else
    freq = boostHwGetAdcSpiFreq();
#endif

    *o = freq;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostHwIfSetInputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t state;

    state = *( (uint32_t *)in );

#ifdef BOOST_HW_IF_CONFIG_OPIL
    boostHwOpilSetInputRelay(state);
#else
    boostHwSetInputRelay(state);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostHwIfGetInputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t state;

#ifdef BOOST_HW_IF_CONFIG_OPIL
    state = boostHwOpilGetInputRelay();
#else
    state = boostHwGetInputRelay();
#endif

    *o = state;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostHwIfSetOutputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t state;

    state = *( (uint32_t *)in );

#ifdef BOOST_HW_IF_CONFIG_OPIL
    boostHwOpilSetOutputRelay(state);
#else
    boostHwSetOutputRelay(state);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostHwIfGetOutputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t state;

#ifdef BOOST_HW_IF_CONFIG_OPIL
    state = boostHwOpilGetOutputRelay();
#else
    state = boostHwGetOutputRelay();
#endif

    *o = state;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t boostHwIfSetMeasGains(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    boostConfigMeasGains_t *gains;

    gains = ( (boostConfigMeasGains_t *)in );

#ifdef BOOST_HW_IF_CONFIG_OPIL
    boostHwOpilSetMeasGains(gains);
#else
    boostHwSetMeasGains(gains);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostHwIfGetMeasGains(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    boostConfigMeasGains_t *o = (boostConfigMeasGains_t *)*out;
    boostConfigMeasGains_t gains;
    uint32_t size;

#ifdef BOOST_HW_IF_CONFIG_OPIL
    size = boostHwOpilGetMeasGains(&gains);
#else
    size = boostHwGetMeasGains(&gains);
#endif

    *o = gains;

    return size;
}
//-----------------------------------------------------------------------------
static int32_t boostHwIfClearStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

#ifdef BOOST_HW_IF_CONFIG_OPIL
    boostHwOpilStatusClear();
#else
    boostHwStatusClear();
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t boostHwIfGetStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t status;
    uint32_t *o = (uint32_t *)*out;

#ifdef BOOST_HW_IF_CONFIG_OPIL
    status = boostHwOpilStatus();
#else
    status = boostHwStatus();
#endif

    *o = status;

    return 4;
}
//-----------------------------------------------------------------------------
//=============================================================================
#endif /* SOC_CPU1 */
