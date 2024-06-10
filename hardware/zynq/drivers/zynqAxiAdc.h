/*
 * @file zynqAxiAdc.h
 *
 */

#ifndef ZYNQ_AXI_ADC_H_
#define ZYNQ_AXI_ADC_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
#include "stddef.h"

//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define ZYNQ_AXI_ADC_ENABLE_OFS                 0
#define ZYNQ_AXI_ADC_ENABLE_MASK                (1 << ZYNQ_AXI_ADC_ENABLE_OFS)

#define ZYNQ_AXI_ADC_MAN_TRIG_OFS               1
#define ZYNQ_AXI_ADC_MAN_TRIG_MASK              (1 << ZYNQ_AXI_ADC_MAN_TRIG_OFS)

#define ZYNQ_AXI_ADC_INT_ENABLE_OFS             2
#define ZYNQ_AXI_ADC_INT_ENABLE_MASK            (1 << ZYNQ_AXI_ADC_INT_ENABLE_OFS)

#define ZYNQ_AXI_ADC_COMP_RST_OFS             3
#define ZYNQ_AXI_ADC_COMP_RST_MASK            (1 << ZYNQ_AXI_ADC_COMP_RST_OFS)

typedef void (*zynqAxiAdcIrqHandle_t)(void *ref);
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void zynqAxiAdcInterruptConfig(void *intcInst, uint32_t intId, zynqAxiAdcIrqHandle_t handle);
//------------------------------------------------------------------------------
void zynqAxiAdcInterrupt2Config(void *intcInst, uint32_t intId, zynqAxiAdcIrqHandle_t handle);
//-----------------------------------------------------------------------------
void zynqAxiAdcEnableWrite(uint32_t adcBase, uint32_t enable);
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcEnableRead(uint32_t adcBase);
//-----------------------------------------------------------------------------
void zynqAxiAdcManualTriggerWrite(uint32_t adcBase, uint32_t trigger);
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcManualTriggerRead(uint32_t adcBase);
//-----------------------------------------------------------------------------
void zynqAxiAdcInterruptEnableWrite(uint32_t adcBase, uint32_t enable);
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcInterruptEnableRead(uint32_t adcBase);
//-----------------------------------------------------------------------------
void zynqAxiAdcCompResetWrite(uint32_t adcBase, uint32_t enable);
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcCompResetRead(uint32_t adcBase);
//-----------------------------------------------------------------------------
void zynqAxiAdcControlWrite(uint32_t adcBase, uint32_t data);
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcControlRead(uint32_t adcBase);
//-----------------------------------------------------------------------------
void zynqAxiAdcDoneIntFactorWrite(uint32_t adcBase, uint8_t factor);
//-----------------------------------------------------------------------------
uint8_t zynqAxiAdcDoneIntFactorRead(uint32_t adcBase);
//-----------------------------------------------------------------------------
void zynqAxiAdcSpiClkDivWrite(uint32_t adcBase, uint32_t clkdiv);
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcSpiClkDivRead(uint32_t adcBase);
//-----------------------------------------------------------------------------
void zynqAxiAdcBufferAddressWrite(uint32_t adcBase, uint32_t address);
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcBufferAddressRead(uint32_t adcBase);
//-----------------------------------------------------------------------------
void zynqAxiAdcCompEnableWrite(uint32_t adcBase, uint32_t data);
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcCompEnableRead(uint32_t adcBase);
//-----------------------------------------------------------------------------

//-----------------limits------------------------------------------------------
void zynqAxiAdcLimit0Write(uint32_t adcBase, uint32_t data);
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit0Read(uint32_t adcBase);
//-----------------------------------------------------------------------------
void zynqAxiAdcLimit1Write(uint32_t adcBase, uint32_t data);
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit1Read(uint32_t adcBase);
//-----------------------------------------------------------------------------
void zynqAxiAdcLimit2Write(uint32_t adcBase, uint32_t data);
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit2Read(uint32_t adcBase);

//-----------------------------------------------------------------------------
void zynqAxiAdcLimit3Write(uint32_t adcBase, uint32_t data);
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit3Read(uint32_t adcBase);

//-----------------------------------------------------------------------------
void zynqAxiAdcLimit4Write(uint32_t adcBase, uint32_t data);
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit4Read(uint32_t adcBase);

//-----------------------------------------------------------------------------
void zynqAxiAdcLimit5Write(uint32_t adcBase, uint32_t data);
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit5Read(uint32_t adcBase);

//-----------------------------------------------------------------------------
void zynqAxiAdcLimit6Write(uint32_t adcBase, uint32_t data);
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit6Read(uint32_t adcBase);

//-----------------------------------------------------------------------------
void zynqAxiAdcLimit7Write(uint32_t adcBase, uint32_t data);
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit7Read(uint32_t adcBase);

//-----------------------------------------------------------------------------
void zynqAxiAdcLimit8Write(uint32_t adcBase, uint32_t data);
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit8Read(uint32_t adcBase);

//-----------------------------------------------------------------------------
void zynqAxiAdcLimit9Write(uint32_t adcBase, uint32_t data);
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit9Read(uint32_t adcBase);

//-----------------------------------------------------------------------------
void zynqAxiAdcLimit10Write(uint32_t adcBase, uint32_t data);
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit10Read(uint32_t adcBase);

//-----------------------------------------------------------------------------
void zynqAxiAdcLimit11Write(uint32_t adcBase, uint32_t data);
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit11Read(uint32_t adcBase);

//-----------------------------------------------------------------------------
void zynqAxiAdcLimit12Write(uint32_t adcBase, uint32_t data);
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit12Read(uint32_t adcBase);

//-----------------------------------------------------------------------------
void zynqAxiAdcLimit13Write(uint32_t adcBase, uint32_t data);
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit13Read(uint32_t adcBase);

//-----------------------------------------------------------------------------
void zynqAxiAdcLimit14Write(uint32_t adcBase, uint32_t data);
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit14Read(uint32_t adcBase);

//-----------------------------------------------------------------------------
void zynqAxiAdcLimit15Write(uint32_t adcBase, uint32_t data);
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcLimit15Read(uint32_t adcBase);


//-----------------------------------------------------------------------------
//=============================================================================

#endif /* ZYNQ_AXI_ADC_H_ */
