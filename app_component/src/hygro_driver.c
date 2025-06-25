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
   XTmrCtr_Initialize(TimerInstancePtr, TimerConfigPtr->BaseAddress);
   XTmrCtr_SetResetValue(TimerInstancePtr, 0, 0);
}

int HYGRO_IPInit(PmodHYGRO *InstancePtr) {
   // Set slave address (7-bit address shifted left by 1)
   Xil_Out32(InstancePtr->BaseAddress + HYGRO_IP_ADR, (InstancePtr->chipAddr << 1) & 0xFE);
   
   // Enable global interrupts if needed
   Xil_Out32(InstancePtr->BaseAddress + HYGRO_IP_GIER, 0x01);
   Xil_Out32(InstancePtr->BaseAddress + HYGRO_IP_IPIER, 0x01);
   
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

void HYGRO_WaitForDone(PmodHYGRO *InstancePtr) {
   u32 status;
   do {
      status = Xil_In32(InstancePtr->BaseAddress + HYGRO_IP_GCSR);
   } while (!(status & HYGRO_GCSR_AP_DONE));
}

void HYGRO_StartOperation(PmodHYGRO *InstancePtr) {
   Xil_Out32(InstancePtr->BaseAddress + HYGRO_IP_GCSR, HYGRO_GCSR_AP_START);
}

void HYGRO_SendByte(PmodHYGRO *InstancePtr, u8 data, u8 start, u8 stop) {
   u32 tx_data = data;
   if (start) tx_data |= HYGRO_TX_START;
   if (stop) tx_data |= HYGRO_TX_STOP;
   
   Xil_Out32(InstancePtr->BaseAddress + HYGRO_IP_TX_FIFO, tx_data);
}

u8 HYGRO_ReceiveByte(PmodHYGRO *InstancePtr, u8 stop) {
   u32 rx_data;
   u32 status;
   
   // Send read command with number of bytes to receive
   u32 tx_data = 1; // Read 1 byte
   if (stop) tx_data |= HYGRO_TX_STOP;
   Xil_Out32(InstancePtr->BaseAddress + HYGRO_IP_TX_FIFO, tx_data);
   
   // Wait for data to be available
   do {
      status = Xil_In32(InstancePtr->BaseAddress + HYGRO_IP_SR);
   } while (status & HYGRO_SR_RX_FIFO_EMPTY);
   
   rx_data = Xil_In32(InstancePtr->BaseAddress + HYGRO_IP_RX_FIFO);
   return (u8)(rx_data & 0xFF);
}

void HYGRO_ReadIIC(PmodHYGRO *InstancePtr, u8 reg, u8 *Data, int nData,
      u32 conversion_delay_ms) {
   int i;
   
   HYGRO_StartOperation(InstancePtr);
   
   if (InstancePtr->currentRegister != reg) {
      // Send register address
      HYGRO_SendByte(InstancePtr, InstancePtr->chipAddr << 1, 1, 0); // Start, write mode
      HYGRO_SendByte(InstancePtr, reg, 0, 0);
      InstancePtr->currentRegister = reg;
   }
   
   if (conversion_delay_ms > 0)
      HYGRO_DelayMillis(InstancePtr, conversion_delay_ms);
   
   // Send read command
   HYGRO_SendByte(InstancePtr, (InstancePtr->chipAddr << 1) | 0x01, 1, 0); // Start, read mode
   
   // Read data bytes
   for (i = 0; i < nData; i++) {
      Data[i] = HYGRO_ReceiveByte(InstancePtr, (i == nData - 1) ? 1 : 0);
   }
   
   HYGRO_WaitForDone(InstancePtr);
}

void HYGRO_WriteIIC(PmodHYGRO *InstancePtr, u8 reg, u8 *Data, int nData) {
   int i;
   
   HYGRO_StartOperation(InstancePtr);
   
   // Send slave address in write mode
   HYGRO_SendByte(InstancePtr, InstancePtr->chipAddr << 1, 1, 0); // Start, write mode
   
   // Send register address
   HYGRO_SendByte(InstancePtr, reg, 0, 0);
   
   // Send data bytes
   for (i = 0; i < nData; i++) {
      HYGRO_SendByte(InstancePtr, Data[i], 0, (i == nData - 1) ? 1 : 0);
   }
   
   InstancePtr->currentRegister = reg;
   
   HYGRO_WaitForDone(InstancePtr);
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