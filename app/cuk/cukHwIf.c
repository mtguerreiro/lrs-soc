/*
 * cukHwIf.c
 *
 */

#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "cukHwIf.h"

#include "cukHw.h"

#include "../utils/rp.h"

#include "xparameters.h"
//#include "zynqAxiPwm.h"
#include "zynqAxiAdc.h"

//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
//#define CUK_HW_CONFIG_PWM_BASE     XPAR_AXI_PWM_0_S00_AXI_BASEADDR
#define CUK_HW_CONFIG_ADC_BASE     XPAR_ADC_PSCTL_0_S00_AXI_BASEADDR
//#define CUK_HW_CONFIG_PWM_BASE     0

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
static int32_t cukHwIfSetPwmOutputEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfSetPwmOvfTriggerEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfSetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfSetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfSetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfSetAdcEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfSetAdcManualTrig(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfSetAdcInterruptEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfSetAdcSpiFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
int32_t cukHwIfInitialize(void){

    /* Initializes the request processor */
    rpInitialize(&hwControl.interface.rp, CUK_HW_IF_END, hwControl.interface.handles);

    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_PWM_RESET, cukHwIfSetPwmReset);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_PWM_OUTPUT_ENABLE, cukHwIfSetPwmOutputEnable);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_PWM_OVF_TRIGGER_ENABLE, cukHwIfSetPwmOvfTriggerEnable);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_PWM_FREQ, cukHwIfSetPwmFrequency);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_PWM_DUTY, cukHwIfSetPwmDutyCycle);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_PWM_DEAD_TIME, cukHwIfSetPwmDeadTime);

    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_ADC_ENABLE, cukHwIfSetAdcEnable);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_ADC_MANUAL_TRIG, cukHwIfSetAdcManualTrig);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_ADC_INT_ENABLE, cukHwIfSetAdcInterruptEnable);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_ADC_SPI_FREQ, cukHwIfSetAdcSpiFreq);

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

    cukHwSetPwmReset(reset);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetPwmOutputEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t enable;

    enable = *( (uint32_t *)in ) & 0x01;

    cukHwSetPwmOutputEnable(enable);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetPwmOvfTriggerEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t enable;

    enable = *( (uint32_t *)in ) & 0x01;

    cukHwSetPwmOvfTriggerEnable(enable);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetPwmFrequency(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t freq;

    freq = *( (uint32_t *)in );

    cukHwSetPwmFrequency(freq);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float duty;

    duty = *( (float *)in );

    cukHwSetPwmDuty(duty);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float deadtime;

    deadtime = *( (float *)in );

    cukHwSetPwmDeadTime(deadtime);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetAdcEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t enable;

    enable = *( (uint32_t *)in ) & 0x01;

    cukHwSetAdcEnable(enable);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetAdcManualTrig(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t trigger;

    trigger = *( (uint32_t *)in ) & 0x01;

    cukHwSetAdcManualTrigger(trigger);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetAdcInterruptEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t enable;

    enable = *( (uint32_t *)in ) & 0x01;

    cukHwSetAdcInterruptEnable(enable);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetAdcSpiFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t freq;

    freq = *( (uint32_t *)in );

    cukHwSetAdcSpiFreq(freq);

    return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
#endif /* SOC_CPU1 */
