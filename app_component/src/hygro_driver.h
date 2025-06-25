#ifndef PMODHYGRO_H
#define PMODHYGRO_H

#include "xil_types.h"
#include "xstatus.h"
#include "xtmrctr.h"
#include "xil_io.h"

// HDC1080 sensor register addresses
#define HYGRO_CONFIG 0x2
#define HYGRO_HUM    0x1
#define HYGRO_TEMP   0x0

// HDC1080 config bit masks
#define HYGRO_CONFIG_RST     0x8000
#define HYGRO_CONFIG_HEAT    0x2000
#define HYGRO_CONFIG_MODE    0x1000
#define HYGRO_CONFIG_BTST    0x0800
#define HYGRO_CONFIG_TRES    0x0400
#define HYGRO_CONFIG_HRES8   0x0200
#define HYGRO_CONFIG_HRES11  0x0100
#define HYGRO_CONFIG_DEFAULT 0x1000

// Custom IP register offsets
#define HYGRO_IP_GCSR     0x00
#define HYGRO_IP_GIER     0x04
#define HYGRO_IP_IPIER    0x08
#define HYGRO_IP_IPISR    0x0C
#define HYGRO_IP_IDR      0x10
#define HYGRO_IP_VERR     0x14
#define HYGRO_IP_SR       0x18
#define HYGRO_IP_TX_FIFO  0x1C
#define HYGRO_IP_RX_FIFO  0x20
#define HYGRO_IP_ADR      0x24

// GCSR register bits
#define HYGRO_GCSR_AP_START  0x01
#define HYGRO_GCSR_AP_DONE   0x02
#define HYGRO_GCSR_AP_IDLE   0x04

// TX_FIFO register bits
#define HYGRO_TX_START  0x100
#define HYGRO_TX_STOP   0x200

// SR register bits (corrected based on documentation)
#define HYGRO_SR_ABGC           0x01  // Addressed By General Call
#define HYGRO_SR_AAS            0x02  // Addressed as Slave
#define HYGRO_SR_BUS_BUSY       0x04  // Bus Busy
#define HYGRO_SR_SRW            0x08  // Slave Read/Write
#define HYGRO_SR_TX_FIFO_FULL   0x10  // TX FIFO Full
#define HYGRO_SR_RX_FIFO_FULL   0x20  // RX FIFO Full
#define HYGRO_SR_RX_FIFO_EMPTY  0x40  // RX FIFO Empty
#define HYGRO_SR_TX_FIFO_EMPTY  0x80  // TX FIFO Empty

typedef struct PmodHYGRO {
   u32 BaseAddress;
   XTmrCtr HYGROTimer;
   u32 clockFreqHz;
   u8 chipAddr;
   u8 currentRegister;
} PmodHYGRO;

// Public API functions
void HYGRO_begin(PmodHYGRO *InstancePtr, u32 IIC_Address, u8 Chip_Address,
      u32 TMR_Address, u32 TMR_SysClockFreqHz);
void HYGRO_TimerInit(XTmrCtr *TMRInstancePtr, XTmrCtr_Config *TMRConfigPtr);
int HYGRO_IPInit(PmodHYGRO *InstancePtr);
void HYGRO_ReadIIC(PmodHYGRO *InstancePtr, u8 reg, u8 *Data, int nData,
      u32 conversion_delay_ms);
void HYGRO_WriteIIC(PmodHYGRO *InstancePtr, u8 reg, u8 *Data, int nData);
void HYGRO_DelayMillis(PmodHYGRO *InstancePtr, u32 millis);

float HYGRO_getTemperature(PmodHYGRO *InstancePtr);
float HYGRO_getHumidity(PmodHYGRO *InstancePtr);
float HYGRO_tempF2C(float tempF);
float HYGRO_tempC2F(float tempC);

// Internal helper functions
void HYGRO_WaitForTxFifoEmpty(PmodHYGRO *InstancePtr);
void HYGRO_WaitForRxFifoNotEmpty(PmodHYGRO *InstancePtr);
int HYGRO_WaitForBusIdle(PmodHYGRO *InstancePtr);

#endif // PMODHYGRO_H