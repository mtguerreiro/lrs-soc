/*
 * ocpConfig.h
 *
 *  Created on: 01.04.2023
 *      Author: mguerreiro
 */

#ifndef OCP_CONFIG_H_
#define OCP_CONFIG_H_

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

/* Trace configurations */
#define OCP_TRACE_CONFIG_TRACE_NAME_MAX_LEN		20

#ifndef OCP_CONFIG_MASTER_CORE
typedef enum{
	OCP_TRACE_1 = 0,
	OCP_TRACE_END
}ocpTraceIDs_t;
#else
typedef enum{
	OCP_TRACE_END
}ocpTraceIDs_t;
#endif

/* Controller configurations */
#define OCP_CS_CONFIG_CS_NAME_MAX_LEN		20

#ifndef OCP_CONFIG_MASTER_CORE
typedef enum{
	OCP_CS_1 = 0,
	OCP_CS_END
}ocpCSIDs_t;
#else
typedef enum{
	OCP_CS_END
}ocpCSIDs_t;
#endif
//=============================================================================

#endif /* OCP_CONFIG_H_ */
