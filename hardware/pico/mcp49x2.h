/*
 * @file MCP49X2.h
 *
 * @brief MCP49x2 driver. Compatible with MCP4902/4912/4922.
 * 
 */

#ifndef MCP49X2_H_
#define MCP49X2_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"

//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef void(*mcp49x2SpiWrite_t)(uint8_t *data, uint32_t size);

typedef void(*mcp49x2CsSet_t)(void);
typedef void(*mcp49x2CsClear_t)(void);

typedef void(*mcp49x2ShdnSet_t)(void);
typedef void(*mcp49x2ShdnClear_t)(void);

typedef void(*mcp49x2LdacSet_t)(void);
typedef void(*mcp49x2LdacClear_t)(void);

typedef enum mcp49x2Res_t{
    MCP49X2_RES_12_BIT,
    MCP49X2_RES_10_BIT,
    MCP49X2_RES_8_BIT,
}mcp49x2Res_t;


#define MCP49X2_CFG_WRITE_CH_A  (0)
#define MCP49X2_CFG_WRITE_CH_B  (1UL << 15UL)

#define MCP49X2_CFG_EN_BUF      (1UL << 14UL)
#define MCP49X2_CFG_DIS_BUF     (0)

#define MCP49X2_CFG_SET_GA_2    (0)
#define MCP49X2_CFG_SET_GA_1    (1UL << 13UL)

#define MCP49X2_CFG_EN_SHDN     (0)
#define MCP49X2_CFG_DIS_SHDN    (1UL << 12UL)

typedef struct mcp49x2_t{

    mcp49x2SpiWrite_t spiWrite;

    mcp49x2CsSet_t csSet;
    mcp49x2CsClear_t csClear;

    mcp49x2ShdnSet_t shdnSet;
    mcp49x2ShdnClear_t shdnClear;

    mcp49x2LdacSet_t ldacSet;
    mcp49x2LdacClear_t ldacClear;

    mcp49x2Res_t res;

}mcp49x2_t;

#define MCP49X2_CFG_IS_LITTLE_ENDIAN    1
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
/*
 * @brief Writes to the MCP49x2's register.
 *
 * @param mcp mcp structure holding callbacks and configs
 * @param data The DAC data, always starting with the LSB. For 8 and 10 bits,
 *        the data is automatically right-shifted before being sent.
 * @param flags OR'ed configuration flags.  
 */
void mcp49x2Write(mcp49x2_t *mcp, uint16_t data, uint16_t flags);
//-----------------------------------------------------------------------------
/*
 * @brief Writes to the CS pin.
 *
 * @param mcp mcp structure holding callbacks and configs
 * @param cs Value to write to the pin. If 0, the pin is cleared. Any other 
 *        value sets the pin.
 */
void mcp49x2CsWrite(mcp49x2_t *mcp, uint32_t cs);
//-----------------------------------------------------------------------------
/*
 * @brief Writes to the LDAC pin.
 *
 * @param mcp mcp structure holding callbacks and configs
 * @param ldac Value to write to the pin. If 0, the pin is cleared. Any other 
 *        value sets the pin.
 */
void mcp49x2LdacWrite(mcp49x2_t *mcp, uint32_t ldac);
//-----------------------------------------------------------------------------
/*
 * @brief Writes to the SHDN  pin.
 *
 * @param mcp mcp structure holding callbacks and configs
 * @param shdn Value to write to the pin. If 0, the pin is cleared. Any other 
 *        value sets the pin.
 */
void mcp49x2ShdnWrite(mcp49x2_t *mcp, uint32_t shdn);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* MCP49X2_H_ */
