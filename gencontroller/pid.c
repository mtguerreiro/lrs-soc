/*
 * pid.c
 *
 *  Created on: 11 de set de 2022
 *      Author: marco
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "pid.h"
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void pidInitialize(pid_t *pid, pidConfig_t *params){

	if( params == 0 ){
		pid->a1 = 0;
		pid->a2 = 0;

		pid->b0 = 0;
		pid->b1 = 0;
		pid->b2 = 0;

		pid->saturate = 0;
		pid->umax = 0;
		pid->umin = 0;
	}
	else{
		pid->a1 = params->a1;
		pid->a2 = params->a2;

		pid->b0 = params->b0;
		pid->b1 = params->b1;
		pid->b2 = params->b2;

		pid->saturate = params->saturate;
		pid->umax = params->umax;
		pid->umin = params->umin;
	}

	pid->e_1 = 0;
	pid->e_2 = 0;
	pid->u_1 = 0;
	pid->u_2 = 0;
}
//-----------------------------------------------------------------------------
void pidSetParams(pid_t *pid, pidConfig_t *params){

	pid->a1 = params->a1;
	pid->a2 = params->a2;

	pid->b0 = params->b0;
	pid->b1 = params->b1;
	pid->b2 = params->b2;

	pid->saturate = params->saturate;
	pid->umax = params->umax;
	pid->umin = params->umin;
}
//-----------------------------------------------------------------------------
void pidGetParams(pid_t *pid, pidConfig_t *params){

	params->a1 = pid->a1;
	params->a2 = pid->a2;

	params->b0 = pid->b0;
	params->b1 = pid->b1;
	params->b2 = pid->b2;

	params->saturate = pid->saturate;
	params->umax = pid->umax;
	params->umin = pid->umin;
}
//-----------------------------------------------------------------------------
float pidRun(pid_t *pid, float e){

	float u;

	u = -pid->a1 * pid->u_1 - pid->a2 * pid->u_2 +\
		 pid->b0 * e + pid->b1 * pid->e_1 + pid->b2 * pid->e_2;

	if( pid->saturate ){
		if( u > pid->umax ) u = pid->umax;
		else if ( u < pid->umin ) u = pid->umin;
	}

    pid->u_2 = pid->u_1;
    pid->u_1 = u;

    pid->e_2 = pid->e_1;
    pid->e_1 = e;

    return u;
}
//-----------------------------------------------------------------------------
//=============================================================================
