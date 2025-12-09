FPGA SoC Ultrasonic Radar Project

This project implements a System-on-Chip (SoC) on the Nexys4 DDR FPGA board to create an ultrasonic radar system.

A hardware core (hc_sr04_core) was created to handle the precise microsecond timing required for the ultrasonic sensor trigger and echo pulse measurement, offloading this task from the CPU.

PWM-based control to oscillate a servo motor, sweeping the sensor back and forth to create a "radar" effect.

LEDs: The 16 on-board LEDs light up based on the distance of the detected object (proximity indicator).

Built upon the FPro system architecture (by Dr. Chu)

Hardware Architecture
  The system is built using Xilinx Vivado and consists of the following modules:
  Top Level: mcs_top_sampler.sv
  Processor: MicroBlaze MCS
  Bus: Custom MMIO Bus created by Dr. Chu
  
  Slot 0: System Timer
  
  Slot 1: UART
  
  Slot 2: GPO (LEDs)
  
  Slot 3: GPI
  
  Slot 4: HC-SR04 Ultrasonic Sensor Core
  
  Slot 6: PWM Controller for Servo
  
  Slot 8: 7-Segment Display
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
hc_sr04_core.cpp/h: Driver for the custom ultrasonic core. Handles timeout logic and distance calculations (ticks to centimeters).

gpio_cores.cpp/h, timer_core.cpp/h, uart_core.cpp/h: Drivers for standard peripherals.

Application (main_sampler_test.cpp):
  Implements the radar_oscillation function.
  Continuously sweeps the servo (PWM channel 6) between defined min/max duty cycles.
  Polls the ultrasonic sensor for distance.
  Maps the detected distance to the LED array to provide visual feedback.
