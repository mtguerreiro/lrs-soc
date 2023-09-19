/*
 * cukControlDisabled.h
 *
 *  Created on: 11.09.2023
 *      Author: marco
 */

#ifndef CUK_CONTROL_DISABLED_H_
#define CUK_CONTROL_DISABLED_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void cukControlDisabledInitialize(void);
//-----------------------------------------------------------------------------
int32_t cukControlDisabledSetParams(void *params, uint32_t n);
//-----------------------------------------------------------------------------
int32_t cukControlDisabledGetParams(void *in, uint32_t insize, void *out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
int32_t cukControlDisabledRun(void *inputs, int32_t ninputs, void *outputs, int32_t nmaxoutputs);
//-----------------------------------------------------------------------------
void cukControlDisabledReset(void);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* CUK_CONTROL_DISABLED_H_ */