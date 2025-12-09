This project implements a SoC on the Nexys4 DDR board to create an ultrasonic sensor radar system.

A dedicated hardware core was created (hc_sr04_core.sv) within the vivado folder to handle
precise timing required for the ultrasonic sensor measurment. 

A PWM based control was created to oscillate a servo motor sweeping the sensor back and forth.

The LED's on board light up based on the distance that the object is detected, if the object is 
detected within 50cm then the lights will light up where that object is on the LEDs. For example lets say the object is detected on the far right when the sensor and servo are going then LED 0 will light up and if it was on the far left then LED 15 would light up. 

This was all built and created upon the architecture provided by Chu. 
