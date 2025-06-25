#ifndef PMODCLP_H
#define PMODCLP_H

#include "xil_types.h"
#include "xstatus.h"
#include "xil_io.h"
#include "sleep.h"

// Custom LCD IP register offsets
#define CLP_IP_GCSR      0x00
#define CLP_IP_GIER      0x04
#define CLP_IP_IPIER     0x08
#define CLP_IP_IPISR     0x0C
#define CLP_IP_IDR       0x10
#define CLP_IP_VERR      0x14
#define CLP_IP_SCSR0     0x18
#define CLP_IP_LCD_CMD   0x1C
#define CLP_IP_LCD_DATA  0x20
#define CLP_IP_LCD_STATUS 0x24

// GCSR register bits
#define CLP_GCSR_AP_START  0x01
#define CLP_GCSR_AP_DONE   0x02
#define CLP_GCSR_AP_IDLE   0x04

// LCD_CMD register bits
#define CLP_LCD_CLEAR_DISPLAY     0x001
#define CLP_LCD_HOME              0x002
#define CLP_LCD_ENTRY_MODE_DIR    0x004
#define CLP_LCD_ENTRY_MODE_SHIFT  0x008
#define CLP_LCD_DISPLAY           0x010
#define CLP_LCD_CURSOR            0x020
#define CLP_LCD_BLINK             0x040
#define CLP_LCD_SHIFT_MODE        0x080
#define CLP_LCD_SHIFT_DIRECTION   0x100
#define CLP_LCD_FUNCTION_SET      0x200
#define CLP_LCD_SET_ADDR          0x400
#define CLP_LCD_RW_DATA           0x1000

// LCD_STATUS register bits
#define CLP_LCD_BUSY              0x001

// Error definitions
#define CLP_LCDS_ERR_SUCCESS           0
#define CLP_LCDS_ERR_ARG_ROW_RANGE     1
#define CLP_LCDS_ERR_ARG_COL_RANGE     2

typedef struct PmodCLP {
   u32 BaseAddress;
} PmodCLP;

void CLP_begin(PmodCLP *InstancePtr, u32 BaseAddress);
void CLP_end(PmodCLP *InstancePtr);
int CLP_IPInit(PmodCLP *InstancePtr);

u8 CLP_WriteStringAtPos(PmodCLP *InstancePtr, uint8_t idxRow, uint8_t idxCol, char *strLn);
void CLP_SetWriteDdramPosition(PmodCLP *InstancePtr, u8 bAdr);
void CLP_DisplayClear(PmodCLP *InstancePtr);
void CLP_FunctionSet(PmodCLP *InstancePtr);

void CLP_WriteCommand(PmodCLP *InstancePtr, u32 cmd_bits);
void CLP_WriteDataByte(PmodCLP *InstancePtr, u8 byte);
void CLP_WaitUntilNotBusy(PmodCLP *InstancePtr);
u8 CLP_ReadStatus(PmodCLP *InstancePtr);

// Internal functions
void CLP_StartOperation(PmodCLP *InstancePtr);
void CLP_WaitForDone(PmodCLP *InstancePtr);

#endif // PMODCLP_H