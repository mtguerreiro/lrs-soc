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
void zynqAxiPwmControlWrite(uint32_t pwm_base, uint32_t data){
    
    Xil_Out32(pwm_base + ZYNQ_AXI_PWM_CONTROL_REG_OFS, data);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiPwmControlRead(uint32_t pwm_base){
    
    return Xil_In32(pwm_base + ZYNQ_AXI_PWM_CONTROL_REG_OFS);
}
//-----------------------------------------------------------------------------
void zynqAxiPwmPeriodWrite(uint32_t pwm_base, uint32_t period){
    
    Xil_Out32(pwm_base + ZYNQ_AXI_PWM_PERIOD_REG_OFS, period);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiPwmPeriodRead(uint32_t pwm_base){

    return Xil_In32(pwm_base + ZYNQ_AXI_PWM_PERIOD_REG_OFS);
}
//-----------------------------------------------------------------------------
void zynqAxiPwmDutyWrite(uint32_t pwm_base, uint32_t duty){
    
    Xil_Out32(pwm_base + ZYNQ_AXI_PWM_DUTY_REG_OFS, duty);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiPwmDutyRead(uint32_t pwm_base){

    return Xil_In32(pwm_base + ZYNQ_AXI_PWM_DUTY_REG_OFS);
}
//-----------------------------------------------------------------------------
void zynqAxiPwmDeadTimeWrite(uint32_t pwm_base, uint32_t deadtime){
    
    Xil_Out32(pwm_base + ZYNQ_AXI_PWM_DEADTIME_REG_OFS, deadtime);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiPwmDeadTimeRead(uint32_t pwm_base){

    return Xil_In32(pwm_base + ZYNQ_AXI_PWM_DEADTIME_REG_OFS);
}
//-----------------------------------------------------------------------------
//=============================================================================
