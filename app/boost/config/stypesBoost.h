/*
 * stypesBoost.h
 *
 *  Created on: 11.09.2023
 *      Author: LRS
 */

#ifndef S_TYPES_BOOST_H_
#define S_TYPES_BOOST_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
#include "stddef.h"

#include "..\boostConfig.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef boostConfigMeasurements_t stypesMeasurements_t;

typedef struct{

}stypesSimData_t;

typedef boostConfigControl_t stypesControl_t;

typedef struct{

	float input_relay;
}stypesControllerData_t;

//=============================================================================

#endif /* S_TYPES_BOOST_H_ */
