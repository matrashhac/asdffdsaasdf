// Hygro includes
#include "hygro_driver.h"
// CLP includes
#include <stdio.h>
#include "lcd_driver.h"
#include "string.h"
// same for Hygro and CLP
#include "sleep.h"
#include "xil_cache.h"
#include "xil_printf.h"
#include "xparameters.h"

//Hygro defines
#ifdef __MICROBLAZE__
#define TIMER_FREQ_HZ  XPAR_XTMRCTR_0_CLOCK_FREQUENCY
#else
#define TIMER_FREQ_HZ 100000000
#endif

//Hygro global variable
PmodHYGRO myDeviceHygro;
//CLP global variable
PmodCLP myDeviceCLP;

//functions for Hygro
void DemoInitializeHygro();
void DemoRunHygro();
void DemoCleanupHygro();
//functions for CLP
void DemoInitializeCLP();
void DemoRunCLP();
void DemoCleanupCLP();
//same for Hygro and CLP
void EnableCaches();
void DisableCaches();

int main() {
   //Hygro function calls
   DemoInitializeHygro();
   //CLP function calls
   DemoInitializeCLP();
   
   // Combined demo run
   DemoRunCombined();
   
   //Cleanup
   DemoCleanupHygro();
   DemoCleanupCLP();
   
   return 0;
}

void DemoInitializeHygro() {
   EnableCaches();
   xil_printf("HYGRO Init Started\n\r");
   HYGRO_begin(
      &myDeviceHygro,
      XPAR_HYGRO_I2C_0_BASEADDR,        // Your custom HDC1080 IP base address
      0x40,                            // Chip address of HDC1080 sensor
      XPAR_AXI_TIMER_0_BASEADDR,       // Timer base address
      TIMER_FREQ_HZ                    // Clock frequency
   );
   xil_printf("HYGRO Init Done\n\r");
}

void DemoInitializeCLP() {
   xil_printf("CLP Init Started\n\r");
   CLP_begin(&myDeviceCLP, XPAR_LCD_WRAPPER_0_BASEADDR);  // Your custom LCD IP base address
   xil_printf("CLP Init Done\n\r");
}

void DemoRunCombined() {
   float temp_degc, hum_perrh;
   char szTemp[32];
   char szHum[32];
   int counter = 0;

   // Initial display
   CLP_DisplayClear(&myDeviceCLP);
   CLP_WriteStringAtPos(&myDeviceCLP, 0, 0, "Temp/Humidity Demo");
   CLP_WriteStringAtPos(&myDeviceCLP, 1, 0, "Initializing...");
   
   usleep(2000000); // 2 second delay

   while (1) {
      // Read sensor data
      temp_degc = HYGRO_getTemperature(&myDeviceHygro);
      hum_perrh = HYGRO_getHumidity(&myDeviceHygro);
      
      // Format strings for display
      sprintf(szTemp, "T:%d.%02dC", 
              (int)temp_degc, 
              ((int)(temp_degc * 100)) % 100);
      sprintf(szHum, "H:%d.%02d%%RH", 
              (int)hum_perrh, 
              ((int)(hum_perrh * 100)) % 100);

      // Update LCD display
      CLP_DisplayClear(&myDeviceCLP);
      CLP_WriteStringAtPos(&myDeviceCLP, 0, 0, szTemp);
      CLP_WriteStringAtPos(&myDeviceCLP, 1, 0, szHum);

      // Also print to console
      xil_printf("Temperature: %d.%02d deg C  Humidity: %d.%02d RH\n\r",
         (int)temp_degc,
         ((int)(temp_degc * 100)) % 100,
         (int)hum_perrh,
         ((int)(hum_perrh * 100)) % 100
      );

      // Update every 2 seconds (HDC1080 max sample rate)
      usleep(2000000);
      counter++;
   }
}

void DemoCleanupHygro() {
   DisableCaches();
}

void DemoCleanupCLP() {
   CLP_end(&myDeviceCLP);
}

void EnableCaches() {
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_ICACHE
   Xil_ICacheEnable();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE
   Xil_DCacheEnable();
#endif
#endif
}

void DisableCaches() {
#ifdef __MICROBLAZE__
#ifdef XPAR_MICROBLAZE_USE_ICACHE
   Xil_ICacheDisable();
#endif
#ifdef XPAR_MICROBLAZE_USE_DCACHE
   Xil_DCacheDisable();
#endif
#endif
}