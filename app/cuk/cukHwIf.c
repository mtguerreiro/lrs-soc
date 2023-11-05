/*
 * cukHwIf.c
 *
 */
#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "cukHwIf.h"

#ifdef CUK_HW_IF_CONFIG_OPIL
#include "cukHwOpil.h"
#else
#include "cukHw.h"
#endif


#include "../utils/rp.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{
    rphandle_t handles[CUK_HW_IF_END];
    rpctx_t rp;
}cukHwIf_t;

typedef struct{

    cukHwIf_t interface;
}cukHwIfControl_t;

static cukHwIfControl_t hwControl;
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static int32_t cukHwIfSetPwmReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfGetPwmReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t cukHwIfSetPwmOutputEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfGetPwmOutputEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t cukHwIfSetPwmOvfTriggerEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfGetPwmOvfTriggerEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t cukHwIfSetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfGetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t cukHwIfSetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfGetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t cukHwIfSetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfGetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t cukHwIfSetAdcEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfGetAdcEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t cukHwIfSetAdcManualTrig(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfGetAdcManualTrig(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t cukHwIfSetAdcInterruptEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfGetAdcInterruptEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t cukHwIfSetAdcSpiFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfGetAdcSpiFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t cukHwIfSetLoadSwitch(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfGetLoadSwitch(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t cukHwIfSetOutputSwitch(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfGetOutputSwitch(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t cukHwIfSetMeasGains(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfGetMeasGains(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
int32_t cukHwIfInitialize(void){

    /* Initializes the request processor */
    rpInitialize(&hwControl.interface.rp, CUK_HW_IF_END, hwControl.interface.handles);

    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_PWM_RESET, cukHwIfSetPwmReset);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_GET_PWM_RESET, cukHwIfGetPwmReset);

    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_PWM_OUTPUT_ENABLE, cukHwIfSetPwmOutputEnable);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_GET_PWM_OUTPUT_ENABLE, cukHwIfGetPwmOutputEnable);

    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_PWM_OVF_TRIGGER_ENABLE, cukHwIfSetPwmOvfTriggerEnable);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_GET_PWM_OVF_TRIGGER_ENABLE, cukHwIfGetPwmOvfTriggerEnable);

    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_PWM_FREQ, cukHwIfSetPwmFrequency);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_GET_PWM_FREQ, cukHwIfGetPwmFrequency);

    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_PWM_DUTY, cukHwIfSetPwmDutyCycle);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_GET_PWM_DUTY, cukHwIfGetPwmDutyCycle);

    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_PWM_DEAD_TIME, cukHwIfSetPwmDeadTime);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_GET_PWM_DEAD_TIME, cukHwIfGetPwmDeadTime);

    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_ADC_ENABLE, cukHwIfSetAdcEnable);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_GET_ADC_ENABLE, cukHwIfGetAdcEnable);

    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_ADC_MANUAL_TRIG, cukHwIfSetAdcManualTrig);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_GET_ADC_MANUAL_TRIG, cukHwIfGetAdcManualTrig);

    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_ADC_INT_ENABLE, cukHwIfSetAdcInterruptEnable);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_GET_ADC_INT_ENABLE, cukHwIfGetAdcInterruptEnable);

    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_ADC_SPI_FREQ, cukHwIfSetAdcSpiFreq);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_GET_ADC_SPI_FREQ, cukHwIfGetAdcSpiFreq);

    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_LOAD_SWITCH, cukHwIfSetLoadSwitch);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_GET_LOAD_SWITCH, cukHwIfGetLoadSwitch);

    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_OUTPUT_SWITCH, cukHwIfSetOutputSwitch);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_GET_OUTPUT_SWITCH, cukHwIfGetOutputSwitch);

    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_MEAS_GAINS, cukHwIfSetMeasGains);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_GET_MEAS_GAINS, cukHwIfGetMeasGains);

    return 0;
}
//-----------------------------------------------------------------------------
int32_t cukHwIf(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

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
static int32_t cukHwIfSetPwmReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t reset;

    reset = *( (uint32_t *)in ) & 0x01;

#ifdef CUK_HW_IF_CONFIG_OPIL
    cukHwOpilSetPwmReset(reset);
#else
    cukHwSetPwmReset(reset);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfGetPwmReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t reset;

#ifdef CUK_HW_IF_CONFIG_OPIL
    reset = cukHwOpilGetPwmReset();
#else
    reset = cukHwGetPwmReset();
#endif

    *o = reset;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetPwmOutputEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t enable;

    enable = *( (uint32_t *)in ) & 0x01;

#ifdef CUK_HW_IF_CONFIG_OPIL
    cukHwOpilSetPwmOutputEnable(enable);
#else
    cukHwSetPwmOutputEnable(enable);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfGetPwmOutputEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t enable;

#ifdef CUK_HW_IF_CONFIG_OPIL
    enable = cukHwOpilGetPwmOutputEnable();
#else
    enable = cukHwGetPwmOutputEnable();
#endif

    *o = enable;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetPwmOvfTriggerEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t enable;

    enable = *( (uint32_t *)in ) & 0x01;

#ifdef CUK_HW_IF_CONFIG_OPIL
    cukHwOpilSetPwmOvfTriggerEnable(enable);
#else
    cukHwSetPwmOvfTriggerEnable(enable);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfGetPwmOvfTriggerEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t enable;

#ifdef CUK_HW_IF_CONFIG_OPIL
    enable = cukHwOpilGetPwmOvfTriggerEnable();
#else
    enable = cukHwGetPwmOvfTriggerEnable();
#endif

    *o = enable;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t freq;

    freq = *( (uint32_t *)in );

#ifdef CUK_HW_IF_CONFIG_OPIL
    cukHwOpilSetPwmFrequency(freq);
#else
    cukHwSetPwmFrequency(freq);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfGetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t freq;

#ifdef CUK_HW_IF_CONFIG_OPIL
    freq = cukHwOpilGetPwmFrequency();
#else
    freq = cukHwGetPwmFrequency();
#endif

    *o = freq;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float duty;

    duty = *( (float *)in );

#ifdef CUK_HW_IF_CONFIG_OPIL
    cukHwOpilSetPwmDuty(duty);
#else
    cukHwSetPwmDuty(duty);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfGetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float *o = (float *)*out;
    float duty;

#ifdef CUK_HW_IF_CONFIG_OPIL
    duty = cukHwOpilGetPwmDuty();
#else
    duty = cukHwGetPwmDuty();
#endif

    *o = duty;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float deadtime;

    deadtime = *( (float *)in );

#ifdef CUK_HW_IF_CONFIG_OPIL
    cukHwOpilSetPwmDeadTime(deadtime);
#else
    cukHwSetPwmDeadTime(deadtime);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfGetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float *o = (float *)*out;
    float deadtime;

#ifdef CUK_HW_IF_CONFIG_OPIL
    deadtime = cukHwOpilGetPwmDeadTime();
#else
    deadtime = cukHwGetPwmDeadTime();
#endif

    *o = deadtime;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetAdcEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t enable;

    enable = *( (uint32_t *)in ) & 0x01;

#ifdef CUK_HW_IF_CONFIG_OPIL
    cukHwOpilSetAdcEnable(enable);
#else
    cukHwSetAdcEnable(enable);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfGetAdcEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t enable;

#ifdef CUK_HW_IF_CONFIG_OPIL
    enable = cukHwOpilGetAdcEnable();
#else
    enable = cukHwGetAdcEnable();
#endif

    *o = enable;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetAdcManualTrig(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t trigger;

    trigger = *( (uint32_t *)in ) & 0x01;

#ifdef CUK_HW_IF_CONFIG_OPIL
    cukHwOpilSetAdcManualTrigger(trigger);
#else
    cukHwSetAdcManualTrigger(trigger);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfGetAdcManualTrig(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t trigger;

#ifdef CUK_HW_IF_CONFIG_OPIL
    trigger = cukHwOpilGetAdcManualTrigger();
#else
    trigger = cukHwGetAdcManualTrigger();
#endif

    *o = trigger;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetAdcInterruptEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t enable;

    enable = *( (uint32_t *)in ) & 0x01;

#ifdef CUK_HW_IF_CONFIG_OPIL
    cukHwOpilSetAdcInterruptEnable(enable);
#else
    cukHwSetAdcInterruptEnable(enable);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfGetAdcInterruptEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t enable;

#ifdef CUK_HW_IF_CONFIG_OPIL
    enable = cukHwOpilGetAdcInterruptEnable();
#else
    enable = cukHwGetAdcInterruptEnable();
#endif

    *o = enable;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetAdcSpiFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t freq;

    freq = *( (uint32_t *)in );

#ifdef CUK_HW_IF_CONFIG_OPIL
    cukHwOpilSetAdcSpiFreq(freq);
#else
    cukHwSetAdcSpiFreq(freq);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfGetAdcSpiFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t freq;

#ifdef CUK_HW_IF_CONFIG_OPIL
    freq = cukHwOpilGetAdcSpiFreq();
#else
    freq = cukHwGetAdcSpiFreq();
#endif

    *o = freq;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetLoadSwitch(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t state;

    state = *( (uint32_t *)in );

#ifdef CUK_HW_IF_CONFIG_OPIL
    cukHwOpilSetLoadSwitch(state);
#else
    cukHwSetLoadSwitch(state);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfGetLoadSwitch(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t state;

#ifdef CUK_HW_IF_CONFIG_OPIL
    state = cukHwOpilGetLoadSwitch();
#else
    state = cukHwGetLoadSwitch();
#endif

    *o = state;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetOutputSwitch(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t state;

    state = *( (uint32_t *)in );

#ifdef CUK_HW_IF_CONFIG_OPIL
    cukHwOpilSetOutputSwitch(state);
#else
    cukHwSetOutputSwitch(state);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfGetOutputSwitch(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *)*out;
    uint32_t state;

#ifdef CUK_HW_IF_CONFIG_OPIL
    state = cukHwOpilGetOutputSwitch();
#else
    state = cukHwGetOutputSwitch();
#endif

    *o = state;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetMeasGains(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    cukHwMeasGains_t *gains;

    gains = ( (cukHwMeasGains_t *)in );

#ifdef CUK_HW_IF_CONFIG_OPIL
    cukHwSetMeasGains(gains);
#else
    cukHwSetMeasGains(gains);
#endif

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfGetMeasGains(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    cukHwMeasGains_t *o = (cukHwMeasGains_t *)*out;
    cukHwMeasGains_t gains;
    uint32_t size;

#ifdef CUK_HW_IF_CONFIG_OPIL
    size = cukHwOpilGetMeasGains(&gains);
#else
    size = cukHwGetMeasGains(&gains);
#endif

    *o = gains;

    return size;
}
//-----------------------------------------------------------------------------
//=============================================================================
#endif /* SOC_CPU1 */
