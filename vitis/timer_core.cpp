/*****************************************************************//**
 * @file timer_core.cpp
 *
 * @brief implementation of TimerCore class
 *
 * @author p chu
 * @version v1.0: initial release
 ********************************************************************/

#include "timer_core.h"

TimerCore::TimerCore(uint32_t core_base_addr) {
   base_addr = core_base_addr;
   ctrl = 0x01;
   clear();
   io_write(base_addr, CTRL_REG, ctrl);  // enable the timer
}

TimerCore::~TimerCore() {
}

void TimerCore::pause() {
   // reset enable bit to 0
   ctrl = ctrl & ~GO_FIELD;
   io_write(base_addr, CTRL_REG, ctrl);
}

void TimerCore::go() {
   // set enable bit to 1
   ctrl = ctrl | GO_FIELD;
   io_write(base_addr, CTRL_REG, ctrl);
}

void TimerCore::clear() {
   uint32_t wdata;

   // write clear_bit to generate a 1-clock pulse
   // clear bit does not affect ctrl
   wdata = ctrl | CLR_FIELD;
   io_write(base_addr, CTRL_REG, wdata);
}

uint64_t TimerCore::read_tick() {
   uint64_t upper, lower, upper_check;

   // 1. Read Upper 32 bits
   upper = (uint64_t) io_read(base_addr, COUNTER_UPPER_REG);
   // 2. Read Lower 32 bits
   lower = (uint64_t) io_read(base_addr, COUNTER_LOWER_REG);
   // 3. Read Upper AGAIN to check if it changed
   upper_check = (uint64_t) io_read(base_addr, COUNTER_UPPER_REG);

   // If Upper changed between step 1 and 3, a rollover happened.
   // We must re-read the Lower part to be safe.
   if (upper != upper_check) {
       lower = (uint64_t) io_read(base_addr, COUNTER_LOWER_REG);
       upper = upper_check; // Use the new upper value
   }

   return ((upper << 32) | lower);
}

uint64_t TimerCore::read_time() {
   // elapsed time in microsecond (SYS_CLK_FREQ in MHz)
   return (read_tick() / SYS_CLK_FREQ);
}

void TimerCore::sleep(uint64_t us) {
   uint64_t start_time, now;

   start_time = read_time();
   // busy waiting
   do {
      now = read_time();
   } while ((now - start_time) < us);
}

