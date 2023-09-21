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
#define ZYNQ_AXI_ADC_SPI_CLK_DIV_REG_OFS	        4
#define ZYNQ_AXI_ADC_BUFFER_ADDRESS_REG_OFS	        8

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void zynqAxiAdcInterruptConfig(void *intcInst, uint32_t intId, zynqAxiAdcIrqHandle_t handle){

    XScuGic_SetPriorityTriggerType(intcInst, intId, 0x00, 0x3);
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
void zynqAxiAdcControlWrite(uint32_t adcBase, uint32_t data){
    
    Xil_Out32(adcBase + ZYNQ_AXI_ADC_CONTROL_REG_OFS, data);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcControlRead(uint32_t adcBase){
    
    return Xil_In32(adcBase + ZYNQ_AXI_ADC_CONTROL_REG_OFS);
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
//=============================================================================
