/*
 * cukHw.c
 *
 *  Created on: 12.09.2023
 *      Author: marco
 */

#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "cukHw.h"

#include "../utils/rp.h"

#include "xparameters.h"
#include "zynqAxiPwm.h"

//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define CUK_HW_CONFIG_PWM_BASE     XPAR_AXI_PWM_0_S00_AXI_BASEADDR

typedef struct{
    rphandle_t handles[CUK_HW_IF_END];
    rpctx_t rp;
}cukHwIf_t;

typedef struct{

    cukHwIf_t interface;
}cukHwControl_t;

static cukHwControl_t hwControl;
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static int32_t cukHwIfSetPwmReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfSetPwmOutputEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfSetPwmOvfTriggerEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfSetPwmPeriod(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfSetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t cukHwIfSetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
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
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_PWM_PERIOD, cukHwIfSetPwmPeriod);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_PWM_DUTY, cukHwIfSetPwmDutyCycle);
    rpRegisterHandle(&hwControl.interface.rp, CUK_HW_IF_SET_PWM_DEAD_TIME, cukHwIfSetPwmDeadTime);

    return 0;
}
//-----------------------------------------------------------------------------
int32_t cukHwIf(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    int32_t status;

    status = rpRequest(&hwControl.interface.rp, in, insize, out, maxoutsize);

    return status;
}
//-----------------------------------------------------------------------------
int32_t cukHwStatus(void){

    return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetPwmReset(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t reset;
    uint32_t control;

    reset = *( (uint32_t *)in ) & 0x01;
    reset = reset << ZYNQ_AXI_PWM_RESET_OFS;

    control = zynqAxiPwmControlRead(CUK_HW_CONFIG_PWM_BASE) & (~ZYNQ_AXI_PWM_RESET_MASK);

    control = control | reset;

    zynqAxiPwmControlWrite(CUK_HW_CONFIG_PWM_BASE, control);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetPwmOutputEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t enable;
    uint32_t control;

    enable = *( (uint32_t *)in ) & 0x01;
    enable = enable << ZYNQ_AXI_PWM_OUTPUT_ENABLE_OFS;

    control = zynqAxiPwmControlRead(CUK_HW_CONFIG_PWM_BASE) & (~ZYNQ_AXI_PWM_OUTPUT_ENABLE_MASK);

    control = control | enable;

    zynqAxiPwmControlWrite(CUK_HW_CONFIG_PWM_BASE, control);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetPwmOvfTriggerEnable(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t enable;
    uint32_t control;

    enable = *( (uint32_t *)in ) & 0x01;
    enable = enable << ZYNQ_AXI_PWM_OVF_TRIGGER_ENABLE_OFS;

    control = zynqAxiPwmControlRead(CUK_HW_CONFIG_PWM_BASE) & (~ZYNQ_AXI_PWM_OVF_TRIGGER_ENABLE_MASK);

    control = control | enable;

    zynqAxiPwmControlWrite(CUK_HW_CONFIG_PWM_BASE, control);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetPwmPeriod(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t period;

    period = *( (uint32_t *)in );

    zynqAxiPwmPeriodWrite(CUK_HW_CONFIG_PWM_BASE, period);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetPwmDutyCycle(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t duty;

    duty = *( (uint32_t *)in );

    zynqAxiPwmDutyWrite(CUK_HW_CONFIG_PWM_BASE, duty);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t cukHwIfSetPwmDeadTime(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t deadtime;

    deadtime = *( (uint32_t *)in );

    zynqAxiPwmDeadTimeWrite(CUK_HW_CONFIG_PWM_BASE, deadtime);

    return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
#endif /* SOC_CPU1 */
