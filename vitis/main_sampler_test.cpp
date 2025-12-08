/*****************************************************************//**
 * @file main_sampler_test.cpp
 *
 * @brief Basic test of nexys4 ddr mmio cores
 *
 * @author p chu
 * @version v1.0: initial release
 *********************************************************************/

// #define _DEBUG
#include "chu_init.h"
#include "gpio_cores.h"
#include "sseg_core.h"
#include "hc_sr04_core.h"


/**
 * blink once per second for 5 times.
 * provide a sanity check for timer (based on SYS_CLK_FREQ)
 * @param led_p pointer to led instance
 */
void timer_check(GpoCore *led_p) {
   int i;

   for (i = 0; i < 5; i++) {
      led_p->write(0xffff);
      sleep_ms(500);
      led_p->write(0x0000);
      sleep_ms(500);
      debug("timer check - (loop #)/now: ", i, now_ms());
   }
}

/**
 * check individual led
 * @param led_p pointer to led instance
 * @param n number of led
 */
void led_check(GpoCore *led_p, int n) {
   int i;

   for (i = 0; i < n; i++) {
      led_p->write(1, i);
      sleep_ms(100);
      led_p->write(0, i);
      sleep_ms(100);
   }
}

/**
 * leds flash according to switch positions.
 * @param led_p pointer to led instance
 * @param sw_p pointer to switch instance
 */
void sw_check(GpoCore *led_p, GpiCore *sw_p) {
   int i, s;

   s = sw_p->read();
   for (i = 0; i < 30; i++) {
      led_p->write(s);
      sleep_ms(50);
      led_p->write(0);
      sleep_ms(50);
   }
}


/**
 * tri-color led dims gradually
 * @param led_p pointer to led instance
 * @param sw_p pointer to switch instance
 */

void pwm_3color_led_check(PwmCore *pwm_p) {
   int i, n;
   double bright, duty;
   const double P20 = 1.2589;  // P20=100^(1/20); i.e., P20^20=100

   pwm_p->set_freq(50);
   for (n = 0; n < 3; n++) {
      bright = 1.0;
      for (i = 0; i < 20; i++) {
         bright = bright * P20;
         duty = bright / 100.0;
         pwm_p->set_duty(duty, n);
         pwm_p->set_duty(duty, n + 3);
         sleep_ms(100);
      }
      sleep_ms(300);
      pwm_p->set_duty(0.0, n);
      pwm_p->set_duty(0.0, n + 3);
   }
}

void servo_test(PwmCore *pwm_p){
   pwm_p->set_freq(50);

   pwm_p->set_duty(3277,6);
   sleep_ms(1000);

   pwm_p->set_duty(6554,6);
   sleep_ms(1000);

}

void distance(SsegCore *sseg_p, int dist){
    uint8_t list[8];

    for (int i = 0; i < 8; i++){
        list[i] = 0xff;
    }
    
    if(dist > 999) dist = 999;

    list[7] = sseg_p->h2s(dist % 10);
    list[6] = sseg_p->h2s((dist/10) % 10);
    list[5] = sseg_p->h2s((dist/100) % 10);

    sseg_p->write_8ptn(list);
    sseg_p->set_dp(0x00);
}

void check_sensor(HcSr04Core *sonar_p, GpoCore *led_p){
    double dist = sonar_p->read_distance();

    if (dist < 50){
        led_p->write(0xffff);        
    }
    else {
        led_p->write(0x0000);
    }
}


void radar_oscillation(PwmCore *pwm_p, HcSr04Core *sonar_p, GpoCore *led_p) {
    static int current_duty = SERVO_MIN;
    static int direction = 1; // 1 = Moving Right, -1 = Moving Left

    // 1. Move Servo to new position
    // Channel 6 maps to JA Top Pin 3
    pwm_p->set_duty(current_duty, 6); 

    // 2. Measure Distance
    double dist_val = sonar_p->read_distance();

    // 4. RADAR VISUALIZATION (Map Angle to LEDs)
    // We map the servo range (3277 to 6554) to the 16 LEDs (0 to 15).
    // Range is ~3277. Each LED represents a chunk of ~205 ticks.
    
    // Only light up if object is DETECTED (< 50cm)
    if (dist_val != -1.0 && dist_val < 50.0) {
        // Calculate which LED corresponds to the current servo angle
        int led_index = (current_duty - SERVO_MIN) / 205;
        
        // Safety bounds
        if (led_index < 0) led_index = 0;
        if (led_index > 15) led_index = 15;

        // Turn on ONLY that LED to show "Blip" direction
        led_p->write(1, led_index); 
    } 
    else {
        // Clear LEDs if no object seen
        led_p->write(0x0000); 
    }

    // 5. Prepare Angle for next loop (Sweep Logic)
    current_duty += (direction * SERVO_STEP);

    // Reverse direction at limits
    if (current_duty >= SERVO_MAX) {
        current_duty = SERVO_MAX;
        direction = -1; // Go Left
    } 
    else if (current_duty <= SERVO_MIN) {
        current_duty = SERVO_MIN;
        direction = 1;  // Go Right
    }
}


GpoCore led(get_slot_addr(BRIDGE_BASE, S2_LED));
GpiCore sw(get_slot_addr(BRIDGE_BASE, S3_SW));
PwmCore pwm(get_slot_addr(BRIDGE_BASE, S6_PWM));
SsegCore sseg(get_slot_addr(BRIDGE_BASE, S8_SSEG));
HcSr04Core sonar(get_slot_addr(BRIDGE_BASE, S4_USER));



int main() {
   // Set PWM Frequency for Servo (Standard 50Hz)
   pwm.set_freq(50);

   while (1) {
       // Run one step of the radar
       radar_oscillation(&pwm, &sonar, &led);
       
       // Small delay to let servo move and prevent sensor jamming
       // 50ms = ~20 scans per second
       sleep_ms(50); 
   } 
}


