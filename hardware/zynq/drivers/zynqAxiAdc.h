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
typedef enum{

    SINGLE_COMPARATOR_INSTANTIATION_1,

	SINGLE_COMPARATOR_INSTANTIATION_2,

	SINGLE_COMPARATOR_INSTANTIATION_3,

	SINGLE_COMPARATOR_INSTANTIATION_4,

	SINGLE_COMPARATOR_INSTANTIATION_5,

	SINGLE_COMPARATOR_INSTANTIATION_6,

	SINGLE_COMPARATOR_END,
}SingleComparatorInst;
//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void zynqAxiAdcInterrupt1Config(void *intcInst, uint32_t intId, zynqAxiAdcIrqHandle_t handle);
//------------------------------------------------------------------------------
void zynqAxiAdcInterrupt2Config(void *intcInst, uint32_t intId, zynqAxiAdcIrqHandle_t handle);
//------------------------------------------------------------------------------
void zynqAxiAdcInterrupt3Config(void *intcInst, uint32_t intId, zynqAxiAdcIrqHandle_t handle);
//------------------------------------------------------------------------------
void zynqAxiAdcInterrupt4Config(void *intcInst, uint32_t intId, zynqAxiAdcIrqHandle_t handle);
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
void zynqAxiAdcCompEnablebitWrite(uint32_t adcBase, uint32_t enable, uint32_t instantiation);
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcCompEnablebitRead(uint32_t adcBase, SingleComparatorInst instantiation);
//-----------------------------------------------------------------------------
void zynqAxiAdcTripWrite(uint32_t adcBase, uint32_t data, SingleComparatorInst instantiation);
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcTripRead(uint32_t adcBase, SingleComparatorInst instantiation);
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcCompResultRead(uint32_t adcBase);
//-----------------------------------------------------------------------------


//=============================================================================

#endif /* ZYNQ_AXI_ADC_H_ */
