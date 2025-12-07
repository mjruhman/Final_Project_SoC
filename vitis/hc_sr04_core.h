#ifndef _HC_SR04_CORE_H_INCLUDED
#define _HC_SR04_CORE_H_INCLUDED

#include "chu_init.h"

class HcSr04Core {
public:
   /**
    * Register map
    */
   enum {
      DATA_REG = 0,   /**< Read: result ticks / Write: start trigger */
      STATUS_REG = 1  /**< Read status (bit 0 = ready) */
   };

   /**
    * Constructor
    */
   HcSr04Core(uint32_t core_base_addr);
   ~HcSr04Core();

   /**
    * Trigger a measurement and return distance in cm
    * @return distance in cm (floating point)
    */
   double read_distance();

   /**
    * Trigger a measurement and return raw clock ticks
    * @return ticks (1 tick = 10ns)
    */
   uint32_t read_raw();

private:
   uint32_t base_addr;
};

#endif
