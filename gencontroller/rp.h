/*
 * rp.h
 *
 *  Created on: 07.10.2022
 *      Author: mguerreiro
 */

#ifndef RP_H_
#define RP_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef unsigned int rpuint_t;
typedef signed int rpint_t;
typedef rpuint_t rpid_t;

typedef rpint_t (*rphandle_t)(void *in, rpuint_t insize, void **out, rpuint_t maxoutsize);

typedef struct rpctx_t{
	uint32_t maxid;
	rphandle_t *handle;
}rpctx_t;

#define RP_ERR_INVALID_ID	-1
#define RP_ERR_NO_HANDLE	-2
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void rpInitialize(rpctx_t *rp, rpuint_t maxid, rphandle_t *handlebuffer);
//-----------------------------------------------------------------------------
rpint_t rpRegisterHandle(rpctx_t *rp, rpid_t id, rphandle_t handle);
//-----------------------------------------------------------------------------
rpint_t rpRequest(rpctx_t *rp, void *in, rpuint_t insize, void **out, rpuint_t maxoutsize);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* RP_H_ */
