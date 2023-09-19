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

typedef void (*zynqAxiAdcIrqHandle_t)(void *ref);
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void zynqAxiAdcInterruptConfig(void *intcInst, uint32_t intId, zynqAxiAdcIrqHandle_t handle);
//-----------------------------------------------------------------------------
void zynqAxiAdcEnableWrite(uint32_t adcBase, uint32_t enable);
//-----------------------------------------------------------------------------
void zynqAxiAdcManualTriggerWrite(uint32_t adcBase, uint32_t trigger);
//-----------------------------------------------------------------------------
void zynqAxiAdcInterruptEnableWrite(uint32_t adcBase, uint32_t enable);
//-----------------------------------------------------------------------------
void zynqAxiAdcControlWrite(uint32_t adcBase, uint32_t data);
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcControlRead(uint32_t adcBase);
//-----------------------------------------------------------------------------
void zynqAxiAdcSpiClkDivWrite(uint32_t adcBase, uint32_t clkdiv);
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcSpiClkDivRead(uint32_t adcBase);
//-----------------------------------------------------------------------------
void zynqAxiAdcBufferAddressWrite(uint32_t adcBase, uint32_t address);
//-----------------------------------------------------------------------------
uint32_t zynqAxiAdcBufferAddressRead(uint32_t adcBase);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* ZYNQ_AXI_ADC_H_ */
