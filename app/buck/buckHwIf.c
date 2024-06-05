/*
 * buckHwIf.c
 *
 */
#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "buckHwIf.h"

#ifdef BUCK_HW_IF_CONFIG_OPIL
#include "buckHwOpil.h"
#else
#include "buckHw.h"
#endif

#include "buckConfig.h"

#include "../utils/rp.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{
    rphandle_t handles[BUCK_HW_IF_END];
    rpctx_t rp;
}buckHwIf_t;

typedef struct{

    buckHwIf_t interface;
}buckHwIfControl_t;

static buckHwIfControl_t hwControl;
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static int32_t buckHwIfSetPwmReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckHwIfGetPwmReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t buckHwIfSetPwmOutputEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckHwIfGetPwmOutputEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t buckHwIfSetPwmOvfTriggerEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckHwIfGetPwmOvfTriggerEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t buckHwIfSetPwmInv(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckHwIfGetPwmInv(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t buckHwIfSetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckHwIfGetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t buckHwIfSetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckHwIfGetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t buckHwIfSetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckHwIfGetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t buckHwIfSetAdcEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckHwIfGetAdcEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t buckHwIfSetAdcManualTrig(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckHwIfGetAdcManualTrig(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t buckHwIfSetAdcInterruptEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckHwIfGetAdcInterruptEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t buckHwIfSetAdcSpiFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckHwIfGetAdcSpiFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t buckHwIfSetInputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckHwIfGetInputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t buckHwIfSetOutputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckHwIfGetOutputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t buckHwIfSetMeasGains(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckHwIfGetMeasGains(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t buckHwIfClearStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t buckHwIfGetStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
int32_t buckHwIfInitialize(void){

    /* Initializes the request processor */
    rpInitialize(&hwControl.interface.rp, BUCK_HW_IF_END, hwControl.interface.handles);

    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_SET_PWM_RESET, buckHwIfSetPwmReset);
    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_GET_PWM_RESET, buckHwIfGetPwmReset);

    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_SET_PWM_OUTPUT_ENABLE, buckHwIfSetPwmOutputEnable);
    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_GET_PWM_OUTPUT_ENABLE, buckHwIfGetPwmOutputEnable);

    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_SET_PWM_OVF_TRIGGER_ENABLE, buckHwIfSetPwmOvfTriggerEnable);
    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_GET_PWM_OVF_TRIGGER_ENABLE, buckHwIfGetPwmOvfTriggerEnable);

    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_SET_PWM_INV, buckHwIfSetPwmInv);
    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_GET_PWM_INV, buckHwIfGetPwmInv);

    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_SET_PWM_FREQ, buckHwIfSetPwmFrequency);
    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_GET_PWM_FREQ, buckHwIfGetPwmFrequency);

    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_SET_PWM_DUTY, buckHwIfSetPwmDutyCycle);
    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_GET_PWM_DUTY, buckHwIfGetPwmDutyCycle);

    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_SET_PWM_DEAD_TIME, buckHwIfSetPwmDeadTime);
    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_GET_PWM_DEAD_TIME, buckHwIfGetPwmDeadTime);

    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_SET_ADC_ENABLE, buckHwIfSetAdcEnable);
    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_GET_ADC_ENABLE, buckHwIfGetAdcEnable);

    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_SET_ADC_MANUAL_TRIG, buckHwIfSetAdcManualTrig);
    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_GET_ADC_MANUAL_TRIG, buckHwIfGetAdcManualTrig);

    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_SET_ADC_INT_ENABLE, buckHwIfSetAdcInterruptEnable);
    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_GET_ADC_INT_ENABLE, buckHwIfGetAdcInterruptEnable);

    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_SET_ADC_SPI_FREQ, buckHwIfSetAdcSpiFreq);
    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_GET_ADC_SPI_FREQ, buckHwIfGetAdcSpiFreq);

    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_SET_INPUT_RELAY, buckHwIfSetInputRelay);
    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_GET_INPUT_RELAY, buckHwIfGetInputRelay);

    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_SET_OUTPUT_RELAY, buckHwIfSetOutputRelay);
    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_GET_OUTPUT_RELAY, buckHwIfGetOutputRelay);

    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_SET_MEAS_GAINS, buckHwIfSetMeasGains);
    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_GET_MEAS_GAINS, buckHwIfGetMeasGains);

    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_CLEAR_STATUS, buckHwIfClearStatus);
    rpRegisterHandle(&hwControl.interface.rp, BUCK_HW_IF_GET_STATUS, buckHwIfGetStatus);

    return 0;
}
//-----------------------------------------------------------------------------
int32_t buckHwIf(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

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
static int32_t buckHwIfSetPwmReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t reset;

    reset = *( (uint32_t *)in ) & 0x01;

#ifdef BUCK_HW_IF_CONFIG_OPIL
    buckHwOpilSetPwmReset(reset);
#else
    buckHwSetPwmReset(reset);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfGetPwmReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t reset;

#ifdef BUCK_HW_IF_CONFIG_OPIL
    reset = buckHwOpilGetPwmReset();
#else
    reset = buckHwGetPwmReset();
#endif

    *o = reset;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfSetPwmOutputEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t enable;

    enable = *( (uint32_t *)in ) & 0x01;

#ifdef BUCK_HW_IF_CONFIG_OPIL
    buckHwOpilSetPwmOutputEnable(enable);
#else
    buckHwSetPwmOutputEnable(enable);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfGetPwmOutputEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t enable;

#ifdef BUCK_HW_IF_CONFIG_OPIL
    enable = buckHwOpilGetPwmOutputEnable();
#else
    enable = buckHwGetPwmOutputEnable();
#endif

    *o = enable;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfSetPwmOvfTriggerEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t enable;

    enable = *( (uint32_t *)in ) & 0x01;

#ifdef BUCK_HW_IF_CONFIG_OPIL
    buckHwOpilSetPwmOvfTriggerEnable(enable);
#else
    buckHwSetPwmOvfTriggerEnable(enable);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfGetPwmOvfTriggerEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t enable;

#ifdef BUCK_HW_IF_CONFIG_OPIL
    enable = buckHwOpilGetPwmOvfTriggerEnable();
#else
    enable = buckHwGetPwmOvfTriggerEnable();
#endif

    *o = enable;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfSetPwmInv(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t enable;

    enable = *( (uint32_t *)in ) & 0x01;

#ifdef BUCK_HW_IF_CONFIG_OPIL
    buckHwOpilSetPwmInv(enable);
#else
    buckHwSetPwmInv(enable);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfGetPwmInv(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t enable;

#ifdef BUCK_HW_IF_CONFIG_OPIL
    enable = buckHwOpilGetPwmInv();
#else
    enable = buckHwGetPwmInv();
#endif

    *o = enable;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfSetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t freq;

    freq = *( (uint32_t *)in );

#ifdef BUCK_HW_IF_CONFIG_OPIL
    buckHwOpilSetPwmFrequency(freq);
#else
    buckHwSetPwmFrequency(freq);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfGetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t freq;

#ifdef BUCK_HW_IF_CONFIG_OPIL
    freq = buckHwOpilGetPwmFrequency();
#else
    freq = buckHwGetPwmFrequency();
#endif

    *o = freq;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfSetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float duty;

    duty = *( (float *)in );

#ifdef BUCK_HW_IF_CONFIG_OPIL
    buckHwOpilSetPwmDuty(duty);
#else
    buckHwSetPwmDuty(duty);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfGetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float *o = (float *)*out;
    float duty;

#ifdef BUCK_HW_IF_CONFIG_OPIL
    duty = buckHwOpilGetPwmDuty();
#else
    duty = buckHwGetPwmDuty();
#endif

    *o = duty;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfSetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float deadtime;

    deadtime = *( (float *)in );

#ifdef BUCK_HW_IF_CONFIG_OPIL
    buckHwOpilSetPwmDeadTime(deadtime);
#else
    buckHwSetPwmDeadTime(deadtime);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfGetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float *o = (float *)*out;
    float deadtime;

#ifdef BUCK_HW_IF_CONFIG_OPIL
    deadtime = buckHwOpilGetPwmDeadTime();
#else
    deadtime = buckHwGetPwmDeadTime();
#endif

    *o = deadtime;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfSetAdcEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t enable;

    enable = *( (uint32_t *)in ) & 0x01;

#ifdef BUCK_HW_IF_CONFIG_OPIL
    buckHwOpilSetAdcEnable(enable);
#else
    buckHwSetAdcEnable(enable);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfGetAdcEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t enable;

#ifdef BUCK_HW_IF_CONFIG_OPIL
    enable = buckHwOpilGetAdcEnable();
#else
    enable = buckHwGetAdcEnable();
#endif

    *o = enable;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfSetAdcManualTrig(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t trigger;

    trigger = *( (uint32_t *)in ) & 0x01;

#ifdef BUCK_HW_IF_CONFIG_OPIL
    buckHwOpilSetAdcManualTrigger(trigger);
#else
    buckHwSetAdcManualTrigger(trigger);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfGetAdcManualTrig(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t trigger;

#ifdef BUCK_HW_IF_CONFIG_OPIL
    trigger = buckHwOpilGetAdcManualTrigger();
#else
    trigger = buckHwGetAdcManualTrigger();
#endif

    *o = trigger;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfSetAdcInterruptEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t enable;

    enable = *( (uint32_t *)in ) & 0x01;

#ifdef BUCK_HW_IF_CONFIG_OPIL
    buckHwOpilSetAdcInterruptEnable(enable);
#else
    buckHwSetAdcInterruptEnable(enable);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfGetAdcInterruptEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t enable;

#ifdef BUCK_HW_IF_CONFIG_OPIL
    enable = buckHwOpilGetAdcInterruptEnable();
#else
    enable = buckHwGetAdcInterruptEnable();
#endif

    *o = enable;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfSetAdcSpiFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t freq;

    freq = *( (uint32_t *)in );

#ifdef BUCK_HW_IF_CONFIG_OPIL
    buckHwOpilSetAdcSpiFreq(freq);
#else
    buckHwSetAdcSpiFreq(freq);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfGetAdcSpiFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t freq;

#ifdef BUCK_HW_IF_CONFIG_OPIL
    freq = buckHwOpilGetAdcSpiFreq();
#else
    freq = buckHwGetAdcSpiFreq();
#endif

    *o = freq;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfSetInputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t state;

    state = *( (uint32_t *)in );

#ifdef BUCK_HW_IF_CONFIG_OPIL
    buckHwOpilSetInputRelay(state);
#else
    buckHwSetInputRelay(state);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfGetInputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t state;

#ifdef BUCK_HW_IF_CONFIG_OPIL
    state = buckHwOpilGetInputRelay();
#else
    state = buckHwGetInputRelay();
#endif

    *o = state;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfSetOutputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t state;

    state = *( (uint32_t *)in );

#ifdef BUCK_HW_IF_CONFIG_OPIL
    buckHwOpilSetOutputRelay(state);
#else
    buckHwSetOutputRelay(state);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfGetOutputRelay(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t state;

#ifdef BUCK_HW_IF_CONFIG_OPIL
    state = buckHwOpilGetOutputRelay();
#else
    state = buckHwGetOutputRelay();
#endif

    *o = state;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfSetMeasGains(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    buckConfigMeasGains_t *gains;

    gains = ( (buckConfigMeasGains_t *)in );

#ifdef BUCK_HW_IF_CONFIG_OPIL
    buckHwOpilSetMeasGains(gains);
#else
    buckHwSetMeasGains(gains);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfGetMeasGains(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    buckConfigMeasGains_t *o = (buckConfigMeasGains_t *)*out;
    buckConfigMeasGains_t gains;
    uint32_t size;

#ifdef BUCK_HW_IF_CONFIG_OPIL
    size = buckHwOpilGetMeasGains(&gains);
#else
    size = buckHwGetMeasGains(&gains);
#endif

    *o = gains;

    return size;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfClearStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

#ifdef BUCK_HW_IF_CONFIG_OPIL
    buckHwOpilStatusClear();
#else
    buckHwStatusClear();
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t buckHwIfGetStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t status;
    uint32_t *o = (uint32_t *)*out;

#ifdef BUCK_HW_IF_CONFIG_OPIL
    status = buckHwOpilStatus();
#else
    status = buckHwStatus();
#endif

    *o = status;

    return 4;
}
//-----------------------------------------------------------------------------
//=============================================================================
#endif /* SOC_CPU1 */
