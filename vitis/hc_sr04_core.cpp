#include "hc_sr04_core.h"

HcSr04Core::HcSr04Core(uint32_t core_base_addr) {
   base_addr = core_base_addr;
}

HcSr04Core::~HcSr04Core() {
}

uint32_t HcSr04Core::read_raw() {
   // 1. Write any value to DATA_REG to trigger the sensor
   io_write(base_addr, DATA_REG, 1);


    int timeout = 1000000;
   // 2. Poll the ready bit in STATUS_REG
   while ((io_read(base_addr, STATUS_REG) & 0x01) == 0) {
      // Busy wait
      timeout--;
      if(timeout == 0){
          return 0xFFFFFFFF;
      }
   }

   // 3. Read the result from DATA_REG
   return io_read(base_addr, DATA_REG);
}

double HcSr04Core::read_distance() {
   uint32_t ticks = read_raw();
   
   // If timeout/error
   if (ticks == 0xFFFFFFFF) return -1.0;

   // Calculation:
   // System Clock = 100 MHz (1 tick = 10 ns = 0.01 us)
   // Sound Speed ~ 343 m/s = 0.0343 cm/us
   // Distance = (Time * Speed) / 2
   
   double time_us = ticks * 0.01; 
   double distance_cm = (time_us * 0.0343) / 2.0;
   
   return distance_cm;
}

