/*
 * @file afeZynqHwIf.c
 *
 */

#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "afeZynqhwIf.h"

#include "rp.h"

#include "soc_defs.h"

#include "xparameters.h"
#include <stdio.h>
#include "xil_io.h"
#include "xil_mmu.h"
#include "xil_cache.h"
#include "xil_exception.h"
#include "xscugic.h"
#include "sleep.h"

#include "xgpio.h"
#include "xil_types.h"

#include "axi_test.h"

#include "afeZynqHw.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{

	rphandle_t cmdHandle[AFE_ZYNQ_HW_IF_END];
	rpctx_t rp;
}afeZynqHwIfControl_t;

#define AXI_TEST_BASE_ADR			XPAR_ADC_PSCTL_0_S00_AXI_BASEADDR
#define AXI_PWM_BASE_ADR			XPAR_AXI_PWM_0_S00_AXI_BASEADDR
#define AXI_GPIO_DEBUG_BASE_ADR		XPAR_AXI_GPIO_DEBUG_BASEADDR
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
afeZynqHwIfControl_t xafeZynqHwIfControl;
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static void afeZynqHwIfRpInit(void);
//-----------------------------------------------------------------------------
static int32_t afeZynqHwIfCmdBlink(void *in, rpuint_t insize, void **out, rpuint_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t afeZynqHwIfCmdAdcEn(void *in, rpuint_t insize, void **out, rpuint_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t afeZynqHwIfCmdAdcSpiFreq(void *in, rpuint_t insize, void **out, rpuint_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t afeZynqHwIfCmdAdcSamplingFreq(void *in, rpuint_t insize, void **out, rpuint_t maxoutsize);
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void afeZynqHwIfInitialize(void){

	afeZynqHwIfRpInit();
}
//-----------------------------------------------------------------------------
int32_t afeZynqHwIf(void *in, int32_t insize, void **out, int32_t maxoutsize){

	return rpRequest(&xafeZynqHwIfControl.rp, in, insize, out, maxoutsize);
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static void afeZynqHwIfRpInit(void){

	rpInitialize(&xafeZynqHwIfControl.rp, AFE_ZYNQ_HW_IF_END, xafeZynqHwIfControl.cmdHandle);

	rpRegisterHandle(&xafeZynqHwIfControl.rp, AFE_ZYNQ_HW_IF_BLINK, afeZynqHwIfCmdBlink);
	rpRegisterHandle(&xafeZynqHwIfControl.rp, AFE_ZYNQ_HW_IF_ADC_EN, afeZynqHwIfCmdAdcEn);
	rpRegisterHandle(&xafeZynqHwIfControl.rp, AFE_ZYNQ_HW_IF_ADC_SPI_FREQ_SET, afeZynqHwIfCmdAdcSpiFreq);
	rpRegisterHandle(&xafeZynqHwIfControl.rp, AFE_ZYNQ_HW_IF_ADC_SAMPLING_FREQ_SET, afeZynqHwIfCmdAdcSamplingFreq);
}
//-----------------------------------------------------------------------------
static int32_t afeZynqHwIfCmdBlink(void *in, rpuint_t insize, void **out, rpuint_t maxoutsize){

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t afeZynqHwIfCmdAdcEn(void *in, rpuint_t insize, void **out, rpuint_t maxoutsize){

	uint32_t en;

	en = *( (uint32_t *)in );

	afeZynqHwAdcEn(en);

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t afeZynqHwIfCmdAdcSpiFreq(void *in, rpuint_t insize, void **out, rpuint_t maxoutsize){

	uint32_t freq;

	freq = *( (uint32_t *)in );

	afeZynqHwSetAdcSpiFreq(freq);

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t afeZynqHwIfCmdAdcSamplingFreq(void *in, rpuint_t insize, void **out, rpuint_t maxoutsize){

	uint32_t freq;

	freq = *( (uint32_t *)in );

	afeZynqHwSetAdcSamplingFreq(freq);

	return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* SOC_CPU1 */

