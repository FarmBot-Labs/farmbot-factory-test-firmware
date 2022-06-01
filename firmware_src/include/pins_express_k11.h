#ifndef PINS_EXPRESS_K11
#define PINS_EXPRESS_K11

  #define EXPRESS_K11
  // #define HAS_ENCODERS 1
  // #define HAS_LOAD_DETECTORS 1
  #define NUM_STEPPERS 4
  #define NUM_PERIPHERALS 3

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
  #define WATER_PIN 8
  #define VACUUM_PIN 9
  #define LIGHTING_ADC 255
  #define WATER_ADC 255
  #define VACUUM_ADC 255

  #define SERVO_0_PIN 4
  #define SERVO_1_PIN 5
  #define SERVO_2_PIN 6
  #define SERVO_3_PIN 7
#endif
