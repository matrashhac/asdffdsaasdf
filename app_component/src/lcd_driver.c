#include "lcd_driver.h"
#include <string.h>

void CLP_begin(PmodCLP *InstancePtr, u32 BaseAddress) {
   InstancePtr->BaseAddress = BaseAddress;
   CLP_IPInit(InstancePtr);
   usleep(30000);
   
   CLP_FunctionSet(InstancePtr);
   usleep(40);
   
   CLP_WriteCommand(InstancePtr, CLP_LCD_DISPLAY); // Display on
   usleep(40);
   
   CLP_DisplayClear(InstancePtr);
   usleep(1640);
   
   CLP_WriteCommand(InstancePtr, CLP_LCD_ENTRY_MODE_DIR); // Increment mode
   usleep(40);
}

void CLP_end(PmodCLP *InstancePtr) {
   // Nothing specific needed for cleanup
}

int CLP_IPInit(PmodCLP *InstancePtr) {
   // Enable global interrupts if needed
   //commented out, bc we are currently just using polling
   //Xil_Out32(InstancePtr->BaseAddress + CLP_IP_GIER, 0x01);
   //Xil_Out32(InstancePtr->BaseAddress + CLP_IP_IPIER, 0x01);
   
   return XST_SUCCESS;
}

void CLP_StartOperation(PmodCLP *InstancePtr) {
    //clear any previous ap_done
    Xil_In32(InstancePtr->BaseAddress + CLP_IP_GCSR);
    
   Xil_Out32(InstancePtr->BaseAddress + CLP_IP_GCSR, CLP_GCSR_AP_START);
}

void CLP_WaitForDone(PmodCLP *InstancePtr) {
   u32 status;
   do {
      status = Xil_In32(InstancePtr->BaseAddress + CLP_IP_GCSR);
   } while (!(status & CLP_GCSR_AP_DONE));
}

u8 CLP_WriteStringAtPos(PmodCLP *InstancePtr, uint8_t idxRow, uint8_t idxCol, char *strLn) {
   uint8_t bResult = CLP_LCDS_ERR_SUCCESS;
   
   if (idxRow > 1) {
      bResult |= CLP_LCDS_ERR_ARG_ROW_RANGE;
   }
   if (idxCol > 39) {
      bResult |= CLP_LCDS_ERR_ARG_COL_RANGE;
   }
   
   if (bResult == CLP_LCDS_ERR_SUCCESS) {
      uint8_t len = strlen(strLn);
      if (len > 40) {
         len = 40;
      }
      
      uint8_t lenToPrint = len + idxCol;
      if (lenToPrint > 40) {
         len = 40 - idxCol;
      }
      
      // Set write position
      u8 bAddrOffset = (idxRow == 0 ? 0: 0x40) + idxCol;
      CLP_SetWriteDdramPosition(InstancePtr, bAddrOffset);
      
      // Write data
      for (u8 bIdx = 0; bIdx < len; bIdx++) {
         CLP_WriteDataByte(InstancePtr, strLn[bIdx]);
      }
   }
   
   return bResult;
}

void CLP_SetWriteDdramPosition(PmodCLP *InstancePtr, u8 bAdr) {
   CLP_WaitUntilNotBusy(InstancePtr);
   
   // Set the address in LCD_DATA register
   Xil_Out32(InstancePtr->BaseAddress + CLP_IP_LCD_DATA, bAdr | 0x80);  //is 0x80 needed here?
   
   // Trigger set address command
   CLP_WriteCommand(InstancePtr, CLP_LCD_SET_ADDR);
}

void CLP_DisplayClear(PmodCLP *InstancePtr) {
   CLP_WaitUntilNotBusy(InstancePtr);
   CLP_WriteCommand(InstancePtr, CLP_LCD_CLEAR_DISPLAY);
}

void CLP_FunctionSet(PmodCLP *InstancePtr) {
   CLP_WaitUntilNotBusy(InstancePtr);
   
   // Set function parameters in LCD_DATA (4-bit interface, 2 lines, 5x8 dots)
   Xil_Out32(InstancePtr->BaseAddress + CLP_IP_LCD_DATA, 0x28);
   
   // Trigger function set command
   CLP_WriteCommand(InstancePtr, CLP_LCD_FUNCTION_SET);
}

void CLP_WriteCommand(PmodCLP *InstancePtr, u32 cmd_bits) {
   CLP_StartOperation(InstancePtr);
   Xil_Out32(InstancePtr->BaseAddress + CLP_IP_LCD_CMD, cmd_bits);
   CLP_WaitForDone(InstancePtr);
}

void CLP_WriteDataByte(PmodCLP *InstancePtr, u8 byte) {
   CLP_WaitUntilNotBusy(InstancePtr);
   
   // Set data in LCD_DATA register
   Xil_Out32(InstancePtr->BaseAddress + CLP_IP_LCD_DATA, byte);
   
   // Trigger write data command (bit 12 = 1 for write, 0 for read)
   CLP_WriteCommand(InstancePtr, CLP_LCD_RW_DATA);
}

void CLP_WaitUntilNotBusy(PmodCLP *InstancePtr) {
   u8 bStatus;
   do {
      bStatus = CLP_ReadStatus(InstancePtr);
      usleep(10);
   } while (bStatus & CLP_LCD_BUSY);
}

u8 CLP_ReadStatus(PmodCLP *InstancePtr) {
   u32 status = Xil_In32(InstancePtr->BaseAddress + CLP_IP_LCD_STATUS);
   return (u8)(status & 0xFF);
}