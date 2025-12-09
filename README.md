SoC Ultrasonic Radar Project

This project implements a SoC on the Nexys4 DDR board to create an ultrasonic radar system.

A hardware core (hc_sr04_core) was created to handle the timing required for the ultrasonic sensor trigger and echo pulse measurement.
A testbench is included for confirmation that the core is working. 

PWM-based control to sweep the servo motor back and forth.

The 16 on-board LEDs light up based on the distance of the detected object which is less than 50 cm the object will be detected and the lights will light up. 

Built upon the FPro system architecture (by Dr. Chu)

Hardware Architecture
  The system is built using Xilinx Vivado and consists of the following modules:
  Top Level: mcs_top_sampler.sv
  Processor: MicroBlaze MCS
  Bus: Custom MMIO Bus created by Dr. Chu
  
  Slot 0: System Timer
  
  Slot 1: UART - Didn't use but it is instantiated with correct files so it is there if needed
  
  Slot 2: GPO (LEDs)
  
  Slot 3: GPI
  
  Slot 4: HC-SR04 Ultrasonic Sensor Core
  
  Slot 6: PWM Controller for Servo
  
  Slot 8: 7-Segment Display - Didn't use after the distance testing
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
hc_sr04_core.cpp/h: Driver for the custom ultrasonic core. Handles timeout logic and distance calculations (ticks to centimeters).

Application (main_sampler_test.cpp):
  radar_oscilliation function.
  Continuously sweeps the servo between defined min/max duty cycles.
  Polls the ultrasonic sensor for distance.
  Maps the detected distance to the LED array to provide visual feedback.

For any more questions go look at the report included within the github!
