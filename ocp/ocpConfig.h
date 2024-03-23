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
#define OCP_TRACE_CONFIG_TRACE_NAME_MAX_LEN			20
#define OCP_TRACE_MASTER_CONFIG_TRACE_NAME_MAX_LEN	OCP_TRACE_CONFIG_TRACE_NAME_MAX_LEN

typedef enum{
	OCP_TRACE_1 = 0,
	OCP_TRACE_END
}ocpTraceIDs_t;
typedef enum{
	OCP_TRACE_MASTER_END
}ocpTraceMasterIDs_t;

/* Controller configurations */
#define OCP_CS_CONFIG_CS_NAME_MAX_LEN			20
#define OCP_CS_MASTER_CONFIG_CS_NAME_MAX_LEN	OCP_CS_CONFIG_CS_NAME_MAX_LEN

typedef enum{
	OCP_CS_1 = 0,
	OCP_CS_END
}ocpCSIDs_t;
typedef enum{
	OCP_CS_MASTER_END
}ocpCSMasterIDs_t;
//=============================================================================

#endif /* OCP_CONFIG_H_ */
