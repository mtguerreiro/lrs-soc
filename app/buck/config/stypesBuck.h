/*
 * stypesBuck.h
 *
 *  Created on: 24 de mai de 2023
 *      Author: LRS
 */

#ifndef S_TYPES_BUCK_H_
#define S_TYPES_BUCK_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
#include "stddef.h"

//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef struct{
	float i_o;
	float i_l;

	float v_dc_out;
	float v_out;

	float v_dc_in;
	float v_in;
}stypesMeasurements_t;

typedef struct{
	float V_ref;
}stypesSimData_t;

typedef struct{
	float u;
}stypesControl_t;

typedef struct{
	float t_exec;
}stypesControllerData_t;
//=============================================================================

#endif /* S_TYPES_BUCK_H_ */
