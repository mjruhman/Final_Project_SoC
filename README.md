This project implements an SoC on the Nexys4 DDR board to create an ultrasonic sensor radar system.

A dedicated hardware core (hc sr04 core.sv) was created within the Vivado folder to handle the precise timing of the trigger and echo required for the measurement of the ultrasonic sensor.

A PWM based control was created to oscillate a servo motor sweeping the sensor back and forth. 

The LED's on board light up based on the distance that the object is detected, if the object is detected within 50cm then the lights will light up where that object is on the LEDs. For example, lets say the object is detected on the far right when the sensor and servo are going then LED 0 will light up and if it was on the far left then LED 15 would light up.

This was all built and created on the architecture provided by Chu.
