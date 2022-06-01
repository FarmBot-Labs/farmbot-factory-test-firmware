#ifndef FARMDUINO_K16_H
#define FARMDUINO_K16_H
  #include "encoders.h"
  #define FARMDUINO_K16
  #define HAS_ENCODERS 1
  #define HAS_LOAD_DETECTORS 1
  #define NUM_STEPPERS 5
  #define NUM_PERIPHERALS 7

  // X1
  #define X_STEP_PIN 26
  #define X_DIR_PIN 27
  #define X_ENABLE_PIN 25
  #define X_CHIP_SELECT 24

  // X2
  #define E_STEP_PIN 15
  #define E_DIR_PIN 30
  #define E_ENABLE_PIN 14
  #define E_CHIP_SELECT 29

  // Y
  #define Y_STEP_PIN 32
  #define Y_DIR_PIN 33
  #define Y_ENABLE_PIN 31
  #define Y_CHIP_SELECT 28

  // Z
  #define Z_STEP_PIN 35
  #define Z_DIR_PIN 36
  #define Z_ENABLE_PIN 34
  #define Z_CHIP_SELECT 23

  #define LED_PIN 13

  // Peripherals
  #define LIGHTING_PIN 7
  #define LIGHTING_ADC  A0

  #define WATER_PIN 8
  #define WATER_ADC  A1

  #define VACUUM_PIN 9
  #define VACUUM_ADC  A4

  #define PERIPHERAL_4_PIN 10
  #define PERIPHERAL_4_ADC  A3

  #define PERIPHERAL_5_PIN 12
  #define PERIPHERAL_5_ADC  A2

  #define ROTARY_FWD_PIN 2
  #define ROTARY_REV_PIN 3
  #define ROTARY_ADC A6

  // Auxiliary motors
  #define AUX_STEP_PIN 40
  #define AUX_DIR_PIN 41
  #define AUX_ENABLE_PIN 37
  #define AUX_CHIP_SELECT 43

  #define SERVO_0_PIN 4
  #define SERVO_1_PIN 5
  #define SERVO_2_PIN 6
  #define SERVO_3_PIN 11
#endif
