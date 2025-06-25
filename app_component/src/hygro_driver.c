#include "hygro_driver.h"

XTmrCtr_Config HYGRO_TimerConfig = {
   0,
   0,
   0
};

void HYGRO_begin(PmodHYGRO *InstancePtr, u32 IIC_Address, u8 Chip_Address,
      u32 Timer_Address, u32 Timer_SysClockFreqHz) {
   u16 config = HYGRO_CONFIG_DEFAULT & ~(HYGRO_CONFIG_MODE);
   u8 bytes[2] = {(config >> 8) & 0xff, config & 0xff};

   InstancePtr->BaseAddress = IIC_Address;
   InstancePtr->chipAddr = Chip_Address;
   InstancePtr->clockFreqHz = Timer_SysClockFreqHz;

   HYGRO_TimerConfig.BaseAddress = Timer_Address;
   HYGRO_TimerConfig.SysClockFreqHz = Timer_SysClockFreqHz;

   HYGRO_TimerInit(&InstancePtr->HYGROTimer, &HYGRO_TimerConfig);
   HYGRO_IPInit(InstancePtr);
   HYGRO_WriteIIC(InstancePtr, HYGRO_CONFIG, bytes, 2);

   HYGRO_DelayMillis(InstancePtr, 15);
}

void HYGRO_TimerInit(XTmrCtr *TimerInstancePtr, XTmrCtr_Config *TimerConfigPtr) {
   XTmrCtr_CfgInitialize(TimerInstancePtr, TimerConfigPtr, TimerConfigPtr->BaseAddress);
   XTmrCtr_InitHw(TimerInstancePtr);
   //XTmrCtr_Initialize(TimerInstancePtr, TimerConfigPtr->BaseAddress);
   XTmrCtr_SetResetValue(TimerInstancePtr, 0, 0);
}

int HYGRO_IPInit(PmodHYGRO *InstancePtr) {
   
   // not sure why i put this here lol
   //Xil_Out32(InstancePtr->BaseAddress + HYGRO_IP_GCSR, HYGRO_GCSR_AP_START);
   
   // Clear any pending interrupts
   Xil_Out32(InstancePtr->BaseAddress + HYGRO_IP_IPISR, 0x01);
   
   return XST_SUCCESS;
}

void HYGRO_DelayMillis(PmodHYGRO *InstancePtr, u32 millis) {
   XTmrCtr* TimerInstancePtr = &InstancePtr->HYGROTimer;
   u32 done_after_ticks = millis * (InstancePtr->clockFreqHz / 1000);
   XTmrCtr_Reset(TimerInstancePtr, 0);
   XTmrCtr_Start(TimerInstancePtr, 0);
   while (XTmrCtr_GetValue(TimerInstancePtr, 0) < done_after_ticks);
   XTmrCtr_Stop(TimerInstancePtr, 0);
}

void HYGRO_WaitForTxFifoEmpty(PmodHYGRO *InstancePtr) {
   u32 status;
   do {
      status = Xil_In32(InstancePtr->BaseAddress + HYGRO_IP_SR);
   } while (!(status & HYGRO_SR_TX_FIFO_EMPTY));
}

void HYGRO_WaitForRxFifoNotEmpty(PmodHYGRO *InstancePtr) {
   u32 status;
   do {
      status = Xil_In32(InstancePtr->BaseAddress + HYGRO_IP_SR);
   } while (status & HYGRO_SR_RX_FIFO_EMPTY);
}

int HYGRO_WaitForBusIdle(PmodHYGRO *InstancePtr) {
   u32 status;
   
   do {
      status = Xil_In32(InstancePtr->BaseAddress + HYGRO_IP_SR);
   } while (status & HYGRO_SR_BUS_BUSY);
   
   return XST_SUCCESS;
}

void HYGRO_ReadIIC(PmodHYGRO *InstancePtr, u8 reg, u8 *Data, int nData,
      u32 conversion_delay_ms) {
   int i;
   
   if (HYGRO_WaitForBusIdle(InstancePtr) != XST_SUCCESS) {
      return;
   }
   
   // Step 1: Send device address + write bit + register address
   u32 tx_data = ((InstancePtr->chipAddr << 1) & 0xFE) | HYGRO_TX_START;
   Xil_Out32(InstancePtr->BaseAddress + HYGRO_IP_TX_FIFO, tx_data);
   
   // Send register address
   tx_data = reg;
   Xil_Out32(InstancePtr->BaseAddress + HYGRO_IP_TX_FIFO, tx_data);
   
   // Wait for transmission
   HYGRO_WaitForTxFifoEmpty(InstancePtr);
   
   // Conversion delay if needed
   if (conversion_delay_ms > 0) {
      HYGRO_DelayMillis(InstancePtr, conversion_delay_ms);
   }
   
   // Step 2: Repeated start with device address + read bit
   tx_data = ((InstancePtr->chipAddr << 1) | 0x01) | HYGRO_TX_START;
   Xil_Out32(InstancePtr->BaseAddress + HYGRO_IP_TX_FIFO, tx_data);
   
   // Send number of bytes to read with stop bit
   tx_data = nData | HYGRO_TX_STOP;
   Xil_Out32(InstancePtr->BaseAddress + HYGRO_IP_TX_FIFO, tx_data);
   
   // Step 3: Read data from RX_FIFO
   for (i = 0; i < nData; i++) {
      HYGRO_WaitForRxFifoNotEmpty(InstancePtr);
      u32 rx_data = Xil_In32(InstancePtr->BaseAddress + HYGRO_IP_RX_FIFO);
      Data[i] = (u8)(rx_data & 0xFF);
   }
   
   InstancePtr->currentRegister = reg;
}

void HYGRO_WriteIIC(PmodHYGRO *InstancePtr, u8 reg, u8 *Data, int nData) {
   int i;
   
   if (HYGRO_WaitForBusIdle(InstancePtr) != XST_SUCCESS) {
      return;
   }
   
   // Send device address + write bit
   u32 tx_data = ((InstancePtr->chipAddr << 1) & 0xFE) | HYGRO_TX_START;
   Xil_Out32(InstancePtr->BaseAddress + HYGRO_IP_TX_FIFO, tx_data);
   
   // Send register address
   tx_data = reg;
   Xil_Out32(InstancePtr->BaseAddress + HYGRO_IP_TX_FIFO, tx_data);
   
   // Send data bytes
   for (i = 0; i < nData; i++) {
      tx_data = Data[i];
      if (i == nData - 1) {
         tx_data |= HYGRO_TX_STOP;  // Stop bit on last byte
      }
      Xil_Out32(InstancePtr->BaseAddress + HYGRO_IP_TX_FIFO, tx_data);
   }
   
   HYGRO_WaitForTxFifoEmpty(InstancePtr);
   InstancePtr->currentRegister = reg;
}

float HYGRO_getTemperature(PmodHYGRO *InstancePtr) {
   u8 bytes[2];
   float deg_c;

   HYGRO_ReadIIC(InstancePtr, HYGRO_TEMP, bytes, 2, 7);
   deg_c = 256.0 * bytes[0] + 1.0 * bytes[1];
   deg_c /= 65536.0;
   deg_c *= 160.0;
   deg_c -= 40.0;
   return deg_c;
}

float HYGRO_getHumidity(PmodHYGRO *InstancePtr) {
   u8 bytes[2];
   float per_rh;

   HYGRO_ReadIIC(InstancePtr, HYGRO_HUM, bytes, 2, 7);
   per_rh = 256.0 * bytes[0] + 1.0 * bytes[1];
   per_rh /= 65536.0;
   per_rh *= 100.0;
   return per_rh;
}

float HYGRO_tempF2C(float deg_f) {
   return (deg_f - 32) / 1.8;
}

float HYGRO_tempC2F(float deg_c) {
   return deg_c * 1.8 + 32;
}