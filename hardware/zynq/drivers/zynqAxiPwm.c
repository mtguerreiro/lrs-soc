/*
 * @file zynqAxiPwm.c
 *
 */


//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "zynqAxiPwm.h"

#include "xil_io.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define ZYNQ_AXI_PWM_CONTROL_REG_OFS	    0
#define ZYNQ_AXI_PWM_PERIOD_REG_OFS	        4
#define ZYNQ_AXI_PWM_DUTY_REG_OFS	        8
#define ZYNQ_AXI_PWM_DEADTIME_REG_OFS	    12

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void zynqAxiPwmResetWrite(uint32_t pwmBase, uint32_t reset){

    uint32_t control;

    reset = ( reset & 0x01 ) << ZYNQ_AXI_PWM_RESET_OFS;

    control = zynqAxiPwmControlRead(pwmBase) & (~ZYNQ_AXI_PWM_RESET_MASK);

    control = control | reset;

    zynqAxiPwmControlWrite(pwmBase, control);
}
//-----------------------------------------------------------------------------
void zynqAxiPwmOutputEnableWrite(uint32_t pwmBase, uint32_t enable){

    uint32_t control;

    enable = (enable & 0x01) << ZYNQ_AXI_PWM_OUTPUT_ENABLE_OFS;

    control = zynqAxiPwmControlRead(pwmBase) & (~ZYNQ_AXI_PWM_OUTPUT_ENABLE_MASK);

    control = control | enable;

    zynqAxiPwmControlWrite(pwmBase, control);
}
//-----------------------------------------------------------------------------
void zynqAxiPwmOvfTriggerEnableWrite(uint32_t pwmBase, uint32_t enable){

    uint32_t control;

    enable = (enable & 0x01) << ZYNQ_AXI_PWM_OVF_TRIGGER_ENABLE_OFS;

    control = zynqAxiPwmControlRead(pwmBase) & (~ZYNQ_AXI_PWM_OVF_TRIGGER_ENABLE_MASK);

    control = control | enable;

    zynqAxiPwmControlWrite(pwmBase, control);
}
//-----------------------------------------------------------------------------
void zynqAxiPwmControlWrite(uint32_t pwmBase, uint32_t data){
    
    Xil_Out32(pwmBase + ZYNQ_AXI_PWM_CONTROL_REG_OFS, data);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiPwmControlRead(uint32_t pwmBase){
    
    return Xil_In32(pwmBase + ZYNQ_AXI_PWM_CONTROL_REG_OFS);
}
//-----------------------------------------------------------------------------
void zynqAxiPwmPeriodWrite(uint32_t pwmBase, uint32_t period){
    
    Xil_Out32(pwmBase + ZYNQ_AXI_PWM_PERIOD_REG_OFS, period);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiPwmPeriodRead(uint32_t pwmBase){

    return Xil_In32(pwmBase + ZYNQ_AXI_PWM_PERIOD_REG_OFS);
}
//-----------------------------------------------------------------------------
void zynqAxiPwmDutyWrite(uint32_t pwmBase, uint32_t duty){
    
    Xil_Out32(pwmBase + ZYNQ_AXI_PWM_DUTY_REG_OFS, duty);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiPwmDutyRead(uint32_t pwmBase){

    return Xil_In32(pwmBase + ZYNQ_AXI_PWM_DUTY_REG_OFS);
}
//-----------------------------------------------------------------------------
void zynqAxiPwmDeadTimeWrite(uint32_t pwmBase, uint32_t deadtime){
    
    Xil_Out32(pwmBase + ZYNQ_AXI_PWM_DEADTIME_REG_OFS, deadtime);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiPwmDeadTimeRead(uint32_t pwmBase){

    return Xil_In32(pwmBase + ZYNQ_AXI_PWM_DEADTIME_REG_OFS);
}
//-----------------------------------------------------------------------------
//=============================================================================
