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
//for protection limits
#define ZYNQ_AXI_ADC_LIMIT0_REG_OFS	        		16
#define ZYNQ_AXI_ADC_LIMIT1_REG_OFS	        		20
#define ZYNQ_AXI_ADC_LIMIT2_REG_OFS	        		24
#define ZYNQ_AXI_ADC_LIMIT3_REG_OFS	        		28
#define ZYNQ_AXI_ADC_LIMIT4_REG_OFS	        		32
#define ZYNQ_AXI_ADC_LIMIT5_REG_OFS	        		36
#define ZYNQ_AXI_ADC_LIMIT6_REG_OFS	        		40
#define ZYNQ_AXI_ADC_LIMIT7_REG_OFS	        		44
#define ZYNQ_AXI_ADC_LIMIT8_REG_OFS	        		48
#define ZYNQ_AXI_ADC_LIMIT9_REG_OFS	        		52
#define ZYNQ_AXI_ADC_LIMIT10_REG_OFS	        	56
#define ZYNQ_AXI_ADC_LIMIT11_REG_OFS	        	60
#define ZYNQ_AXI_ADC_LIMIT12_REG_OFS	        	64
#define ZYNQ_AXI_ADC_LIMIT13_REG_OFS	        	68
#define ZYNQ_AXI_ADC_LIMIT14_REG_OFS	        	72
#define ZYNQ_AXI_ADC_LIMIT15_REG_OFS	        	76

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
void zynqAxiAdcInterrupt2Config(void *intcInst, uint32_t intId, zynqAxiAdcIrqHandle_t handle){

    XScuGic_SetPriorityTriggerType(intcInst, intId, 0x08, 0x3);
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

//------------DONE FACTOR
void zynqAxiAdcDoneIntFactorWrite(uint32_t adcBase, uint8_t factor){

    Xil_Out8(adcBase + ZYNQ_AXI_ADC_DONE_INT_FACTOR_REG_OFS, factor);
}
//-----------------------------------------------------------------------------
uint8_t zynqAxiAdcDoneIntFactorRead(uint32_t adcBase){

    return Xil_In8(adcBase + ZYNQ_AXI_ADC_DONE_INT_FACTOR_REG_OFS);
}

//-----------------------------------------------------------------------------
//------------------------------------------------------------

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


//---------------------limits---------------------------------------------
//-----------------------------------------------------------------------------
void zynqAxiAdcLimit0Write(uint32_t adcBase, uint32_t data){
    Xil_Out32(adcBase + ZYNQ_AXI_ADC_LIMIT0_REG_OFS, data);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit0Read(uint32_t adcBase){
    return Xil_In32(adcBase + ZYNQ_AXI_ADC_LIMIT0_REG_OFS);
}
//-----------------------------------------------------------------------------
void zynqAxiAdcLimit1Write(uint32_t adcBase, uint32_t data){
    Xil_Out32(adcBase + ZYNQ_AXI_ADC_LIMIT1_REG_OFS, data);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit1Read(uint32_t adcBase){
    return Xil_In32(adcBase + ZYNQ_AXI_ADC_LIMIT1_REG_OFS);
}
//-----------------------------------------------------------------------------
void zynqAxiAdcLimit2Write(uint32_t adcBase, uint32_t data){
    Xil_Out32(adcBase + ZYNQ_AXI_ADC_LIMIT2_REG_OFS, data);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit2Read(uint32_t adcBase){
    return Xil_In32(adcBase + ZYNQ_AXI_ADC_LIMIT2_REG_OFS);
}
//-----------------------------------------------------------------------------
void zynqAxiAdcLimit3Write(uint32_t adcBase, uint32_t data){
    Xil_Out32(adcBase + ZYNQ_AXI_ADC_LIMIT3_REG_OFS, data);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit3Read(uint32_t adcBase){
    return Xil_In32(adcBase + ZYNQ_AXI_ADC_LIMIT3_REG_OFS);
}
//-----------------------------------------------------------------------------
void zynqAxiAdcLimit4Write(uint32_t adcBase, uint32_t data){
    Xil_Out32(adcBase + ZYNQ_AXI_ADC_LIMIT4_REG_OFS, data);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit4Read(uint32_t adcBase){
    return Xil_In32(adcBase + ZYNQ_AXI_ADC_LIMIT4_REG_OFS);
}
//-----------------------------------------------------------------------------
void zynqAxiAdcLimit5Write(uint32_t adcBase, uint32_t data){
    Xil_Out32(adcBase + ZYNQ_AXI_ADC_LIMIT5_REG_OFS, data);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit5Read(uint32_t adcBase){
    return Xil_In32(adcBase + ZYNQ_AXI_ADC_LIMIT5_REG_OFS);
}
//-----------------------------------------------------------------------------
void zynqAxiAdcLimit6Write(uint32_t adcBase, uint32_t data){
    Xil_Out32(adcBase + ZYNQ_AXI_ADC_LIMIT6_REG_OFS, data);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit6Read(uint32_t adcBase){
    return Xil_In32(adcBase + ZYNQ_AXI_ADC_LIMIT6_REG_OFS);
}
//-----------------------------------------------------------------------------
void zynqAxiAdcLimit7Write(uint32_t adcBase, uint32_t data){
    Xil_Out32(adcBase + ZYNQ_AXI_ADC_LIMIT7_REG_OFS, data);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit7Read(uint32_t adcBase){
    return Xil_In32(adcBase + ZYNQ_AXI_ADC_LIMIT7_REG_OFS);
}
//-----------------------------------------------------------------------------
void zynqAxiAdcLimit8Write(uint32_t adcBase, uint32_t data){
    Xil_Out32(adcBase + ZYNQ_AXI_ADC_LIMIT8_REG_OFS, data);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit8Read(uint32_t adcBase){
    return Xil_In32(adcBase + ZYNQ_AXI_ADC_LIMIT8_REG_OFS);
}
//-----------------------------------------------------------------------------
void zynqAxiAdcLimit9Write(uint32_t adcBase, uint32_t data){
    Xil_Out32(adcBase + ZYNQ_AXI_ADC_LIMIT9_REG_OFS, data);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit9Read(uint32_t adcBase){
    return Xil_In32(adcBase + ZYNQ_AXI_ADC_LIMIT9_REG_OFS);
}
//-----------------------------------------------------------------------------
void zynqAxiAdcLimit10Write(uint32_t adcBase, uint32_t data){
    Xil_Out32(adcBase + ZYNQ_AXI_ADC_LIMIT10_REG_OFS, data);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit10Read(uint32_t adcBase){
    return Xil_In32(adcBase + ZYNQ_AXI_ADC_LIMIT10_REG_OFS);
}
//-----------------------------------------------------------------------------
void zynqAxiAdcLimit11Write(uint32_t adcBase, uint32_t data){
    Xil_Out32(adcBase + ZYNQ_AXI_ADC_LIMIT11_REG_OFS, data);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit11Read(uint32_t adcBase){
    return Xil_In32(adcBase + ZYNQ_AXI_ADC_LIMIT11_REG_OFS);
}
//-----------------------------------------------------------------------------
void zynqAxiAdcLimit12Write(uint32_t adcBase, uint32_t data){
    Xil_Out32(adcBase + ZYNQ_AXI_ADC_LIMIT12_REG_OFS, data);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit12Read(uint32_t adcBase){
    return Xil_In32(adcBase + ZYNQ_AXI_ADC_LIMIT12_REG_OFS);
}
//-----------------------------------------------------------------------------
void zynqAxiAdcLimit13Write(uint32_t adcBase, uint32_t data){
    Xil_Out32(adcBase + ZYNQ_AXI_ADC_LIMIT13_REG_OFS, data);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit13Read(uint32_t adcBase){
    return Xil_In32(adcBase + ZYNQ_AXI_ADC_LIMIT13_REG_OFS);
}
//-----------------------------------------------------------------------------
void zynqAxiAdcLimit14Write(uint32_t adcBase, uint32_t data){
    Xil_Out32(adcBase + ZYNQ_AXI_ADC_LIMIT14_REG_OFS, data);
}
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit14Read(uint32_t adcBase){
    return Xil_In32(adcBase + ZYNQ_AXI_ADC_LIMIT14_REG_OFS);
}
//-----------------------------------------------------------------------------
void zynqAxiAdcLimit15Write(uint32_t adcBase, uint32_t data){
    Xil_Out32(adcBase + ZYNQ_AXI_ADC_LIMIT15_REG_OFS, data);
}
//---------------------------------------------------------------
uint32_t zynqAxiAdcLimit15Read(uint32_t adcBase){
    return Xil_In32(adcBase + ZYNQ_AXI_ADC_LIMIT15_REG_OFS);
}


//-----------------------------------------------------------------------------
//=============================================================================
