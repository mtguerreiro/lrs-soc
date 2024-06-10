#ifndef BOOST_CONTROL_ENERGYCINTFPGA_H_
#define BOOST_CONTROL_ENERGYCINTFPGA_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define max_i			40.0f
#define min_i			-40.0f
#define range_i			(max_i - min_i)
#define adc_res_i		4095.0f
#define max_v			30.0f
#define min_v			0.0f
#define range_v			(max_v - min_v)
#define adc_res_v		4095.0f
#define ts 				1.0e-5f

// Used for simulating a slow controller; Controller works well up until FREQ_SPLIT_FACTOR = 8
#define FREQ_SPLIT_FACTOR 1

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void boostControlEnergycintFPGAInitialize(void);
//-----------------------------------------------------------------------------
int32_t boostControlEnergycintFPGASetParams(void *params, uint32_t n);
//-----------------------------------------------------------------------------
int32_t boostControlEnergycintFPGAGetParams(void *in, uint32_t insize, void *out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
int32_t boostControlEnergycintFPGARun(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs);
//-----------------------------------------------------------------------------
void updateLinearizationInput(float controllerOut);
//-----------------------------------------------------------------------------
void boostControlEnergycintFPGAReset(void);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* BOOST_CONTROL_ENERGYCINTFPGA_H_ */
