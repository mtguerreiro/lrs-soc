/*
 * @file zynqAxiPwm.c
 *
 */


//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "zynqAxiAdc.h"

#include "xil_io.h"
#include "xil_exception.h"
#include "xscugic.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define ZYNQ_AXI_ADC_CONTROL_REG_OFS	            0
#define ZYNQ_AXI_ADC_DONE_INT_FACTOR_REG_OFS	    3
#define ZYNQ_AXI_ADC_SPI_CLK_DIV_REG_OFS	        4
#define ZYNQ_AXI_ADC_BUFFER_ADDRESS_REG_OFS	        8
#define ZYNQ_AXI_ADC_COMP_ENABLE_REG_OFS	        12
#define ZYNQ_AXI_ADC_TRIP_REG_OFS	        		16
#define ZYNQ_AXI_ADC_COMP_RESULT	        		80
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
void zynqAxiAdcInterrupt0Config(void *intcInst, uint32_t intId, zynqAxiAdcIrqHandle_t handle){

    XScuGic_SetPriorityTriggerType(intcInst, intId, 0x00, 0x3);
    XScuGic_Connect(intcInst, intId, (Xil_ExceptionHandler)handle, intcInst);

    XScuGic_Enable(intcInst, intId);
}
//-----------------------------------------------------------------------------
void zynqAxiAdcInterrupt1Config(void *intcInst, uint32_t intId, zynqAxiAdcIrqHandle_t handle){

    XScuGic_SetPriorityTriggerType(intcInst, intId, 0x08, 0x3);
    XScuGic_Connect(intcInst, intId, (Xil_ExceptionHandler)handle, intcInst);

    XScuGic_Enable(intcInst, intId);
}
//-----------------------------------------------------------------------------
void zynqAxiAdcInterrupt2Config(void *intcInst, uint32_t intId, zynqAxiAdcIrqHandle_t handle){

    XScuGic_SetPriorityTriggerType(intcInst, intId, 0x10, 0x3);
    XScuGic_Connect(intcInst, intId, (Xil_ExceptionHandler)handle, intcInst);

    XScuGic_Enable(intcInst, intId);
}
//-----------------------------------------------------------------------------
void zynqAxiAdcInterrupt3Config(void *intcInst, uint32_t intId, zynqAxiAdcIrqHandle_t handle){

    XScuGic_SetPriorityTriggerType(intcInst, intId, 0x20, 0x3);
    XScuGic_Connect(intcInst, intId, (Xil_ExceptionHandler)handle, intcInst);

    XScuGic_Enable(intcInst, intId);
}
//-----------------------------------------------------------------------------
void zynqAxiAdcInterrupt4Config(void *intcInst, uint32_t intId, zynqAxiAdcIrqHandle_t handle){

    XScuGic_SetPriorityTriggerType(intcInst, intId, 0x28, 0x3);
    XScuGic_Connect(intcInst, intId, (Xil_ExceptionHandler)handle, intcInst);

    XScuGic_Enable(intcInst, intId);
}
//-----------------------------------------------------------------------------
void zynqAxiAdcEnableWrite(uint32_t adcBase, uint32_t enable){

    uint32_t control;

    enable = (enable & 0x01) << ZYNQ_AXI_ADC_ENABLE_OFS;

    control = zynqAxiAdcControlRead(adcBase) & (~ZYNQ_AXI_ADC_ENABLE_MASK);

    control = control | enable;

    zynqAxiAdcControlWrite(adcBase, control);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcEnableRead(uint32_t adcBase){

    uint32_t enable;

    enable = zynqAxiAdcControlRead(adcBase) & (ZYNQ_AXI_ADC_ENABLE_MASK);

    enable = enable >> ZYNQ_AXI_ADC_ENABLE_OFS;

    return enable;
}
//-----------------------------------------------------------------------------
void zynqAxiAdcManualTriggerWrite(uint32_t adcBase, uint32_t trigger){

    uint32_t control;

    trigger = (trigger & 0x01) << ZYNQ_AXI_ADC_MAN_TRIG_OFS;

    control = zynqAxiAdcControlRead(adcBase) & (~ZYNQ_AXI_ADC_MAN_TRIG_MASK);

    control = control | trigger;

    zynqAxiAdcControlWrite(adcBase, control);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcManualTriggerRead(uint32_t adcBase){

    uint32_t trigger;

    trigger = zynqAxiAdcControlRead(adcBase) & (ZYNQ_AXI_ADC_MAN_TRIG_MASK);

    trigger = trigger >> ZYNQ_AXI_ADC_MAN_TRIG_OFS;

    return trigger;
}
//-----------------------------------------------------------------------------
void zynqAxiAdcInterruptEnableWrite(uint32_t adcBase, uint32_t enable){

    uint32_t control;

    enable = (enable & 0x01) << ZYNQ_AXI_ADC_INT_ENABLE_OFS;

    control = zynqAxiAdcControlRead(adcBase) & (~ZYNQ_AXI_ADC_INT_ENABLE_MASK);

    control = control | enable;

    zynqAxiAdcControlWrite(adcBase, control);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcInterruptEnableRead(uint32_t adcBase){

    uint32_t enable;

    enable = zynqAxiAdcControlRead(adcBase) & (ZYNQ_AXI_ADC_INT_ENABLE_MASK);

    enable = enable >> ZYNQ_AXI_ADC_INT_ENABLE_OFS;

    return enable;
}
//-----------------------------------------------------------------------------
void zynqAxiAdcCompResetWrite(uint32_t adcBase, uint32_t enable){

    uint32_t control;

    enable = (enable & 0x01) << ZYNQ_AXI_ADC_COMP_RST_OFS;

    control = zynqAxiAdcControlRead(adcBase) & (~ZYNQ_AXI_ADC_COMP_RST_MASK);

    control = control | enable;

    zynqAxiAdcControlWrite(adcBase, control);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcCompResetRead(uint32_t adcBase){

    uint32_t enable;

    enable = zynqAxiAdcControlRead(adcBase) & (ZYNQ_AXI_ADC_COMP_RST_MASK);

    enable = enable >> ZYNQ_AXI_ADC_COMP_RST_OFS;

    return enable;
}

//-----------------------------------------------------------------------------
void zynqAxiAdcControlWrite(uint32_t adcBase, uint32_t data){
    
    Xil_Out32(adcBase + ZYNQ_AXI_ADC_CONTROL_REG_OFS, data);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcControlRead(uint32_t adcBase){
    
    return Xil_In32(adcBase + ZYNQ_AXI_ADC_CONTROL_REG_OFS);
}
//-----------------------------------------------------------------------------
void zynqAxiAdcDoneIntFactorWrite(uint32_t adcBase, uint8_t factor){

    Xil_Out8(adcBase + ZYNQ_AXI_ADC_DONE_INT_FACTOR_REG_OFS, factor);
}
//-----------------------------------------------------------------------------
uint8_t zynqAxiAdcDoneIntFactorRead(uint32_t adcBase){

    return Xil_In8(adcBase + ZYNQ_AXI_ADC_DONE_INT_FACTOR_REG_OFS);
}
//-----------------------------------------------------------------------------
void zynqAxiAdcSpiClkDivWrite(uint32_t adcBase, uint32_t clkdiv){
    
    Xil_Out32(adcBase + ZYNQ_AXI_ADC_SPI_CLK_DIV_REG_OFS, clkdiv);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcSpiClkDivRead(uint32_t adcBase){

    return Xil_In32(adcBase + ZYNQ_AXI_ADC_SPI_CLK_DIV_REG_OFS);
}
//-----------------------------------------------------------------------------
void zynqAxiAdcBufferAddressWrite(uint32_t adcBase, uint32_t address){
    
    Xil_Out32(adcBase + ZYNQ_AXI_ADC_BUFFER_ADDRESS_REG_OFS, address);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcBufferAddressRead(uint32_t adcBase){

    return Xil_In32(adcBase + ZYNQ_AXI_ADC_BUFFER_ADDRESS_REG_OFS);
}
//-----------------------------------------------------------------------------
void zynqAxiAdcCompEnableWrite(uint32_t adcBase, uint32_t data){

    Xil_Out32(adcBase + ZYNQ_AXI_ADC_COMP_ENABLE_REG_OFS, data);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcCompEnableRead(uint32_t adcBase){

    return Xil_In32(adcBase + ZYNQ_AXI_ADC_COMP_ENABLE_REG_OFS);
}
//-----------------------------------------------------------------------------
void zynqAxiAdcCompEnablebitWrite(uint32_t adcBase, uint32_t enable, uint32_t instantiation){

	uint32_t control;
    enable = (enable & 0x01) << instantiation;
    uint32_t u = 1;
    u = ~(u << instantiation);
	control = zynqAxiAdcCompEnableRead(adcBase) & u;

	control = control | enable;

	zynqAxiAdcCompEnableWrite(adcBase, control);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcCompEnablebitRead(uint32_t adcBase, SingleComparatorInst instantiation){

    uint32_t enable;

    enable = zynqAxiAdcCompEnableRead(adcBase) & (1 << instantiation);

    enable = enable >> instantiation;

    return enable;
}
//-----------------------------------------------------------------------------
void zynqAxiAdcTripWrite(uint32_t adcBase, uint32_t data, SingleComparatorInst instantiation){
	if (instantiation < SINGLE_COMPARATOR_END){
		int instantiation_ofs = 0;
		instantiation_ofs = ZYNQ_AXI_ADC_TRIP_REG_OFS + (instantiation*4);
		Xil_Out32(adcBase + instantiation_ofs, data);
	}
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcTripRead(uint32_t adcBase, SingleComparatorInst instantiation){
	if (instantiation < SINGLE_COMPARATOR_END){
		int instantiation_ofs = 0;
		instantiation_ofs = ZYNQ_AXI_ADC_TRIP_REG_OFS + (instantiation*4);
		return Xil_In32(adcBase + instantiation_ofs);
	}

}
//------------------------------------------------------------------------------
uint32_t zynqAxiAdcCompResultRead(uint32_t adcBase){
    return Xil_In32(adcBase + ZYNQ_AXI_ADC_COMP_RESULT);
}
//=============================================================================
