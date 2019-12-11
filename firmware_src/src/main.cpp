#include <Arduino.h>
#include <TMC2130Stepper.h>
#include <AccelStepper.h>

#include "debug.h"
#include "platform.h"
#include "comms.h"
#include "steppers.h"
#include "encoders.h"
#include "peripherals.h"

#define CURRENT_MA 600
#define STEPS_PER_MM 5
#define MAX_SPEED 1000*STEPS_PER_MM
#define ACCELERATION 1500*STEPS_PER_MM

/** X1 stepper */
static TMC2130Stepper TMC2130X = TMC2130Stepper(X_ENABLE_PIN, X_DIR_PIN, X_STEP_PIN, X_CHIP_SELECT);
static AccelStepper stepperX = AccelStepper(AccelStepper::DRIVER, X_STEP_PIN, X_DIR_PIN);

/** X2 driver */
static TMC2130Stepper TMC2130E = TMC2130Stepper(E_ENABLE_PIN, E_DIR_PIN, E_STEP_PIN, E_CHIP_SELECT);
static AccelStepper stepperE = AccelStepper(AccelStepper::DRIVER, E_STEP_PIN, E_DIR_PIN);

/** Y1 driver */
static TMC2130Stepper TMC2130Y = TMC2130Stepper(Y_ENABLE_PIN, Y_DIR_PIN, Y_STEP_PIN, Y_CHIP_SELECT);
static AccelStepper stepperY = AccelStepper(AccelStepper::DRIVER, Y_STEP_PIN, Y_DIR_PIN);

/** Z1 driver */
static TMC2130Stepper TMC2130Z = TMC2130Stepper(Z_ENABLE_PIN, Z_DIR_PIN, Z_STEP_PIN, Z_CHIP_SELECT);
static AccelStepper stepperZ = AccelStepper(AccelStepper::DRIVER, Z_STEP_PIN, Z_DIR_PIN);

#if defined(FARMDUINO_K15)
/** AUX driver */
static TMC2130Stepper TMC2130AUX = TMC2130Stepper(AUX_ENABLE_PIN, AUX_DIR_PIN, AUX_STEP_PIN, AUX_CHIP_SELECT);
static AccelStepper stepperAUX = AccelStepper(AccelStepper::DRIVER, AUX_STEP_PIN, AUX_DIR_PIN);
#endif

#if defined(HAS_ENCODERS)
/** Encoders */
static MDLEncoder encoderX = MDLEncoder(MDLEncoder::MDL_X1);
static MDLEncoder encoderE = MDLEncoder(MDLEncoder::MDL_X2);
static MDLEncoder encoderY = MDLEncoder(MDLEncoder::MDL_Y);
static MDLEncoder encoderZ = MDLEncoder(MDLEncoder::MDL_Z);

static MDLEncoder* Encoders[4] = {
  &encoderX,
  &encoderE,
  &encoderY,
  &encoderZ,
#if defined(FARMDUINO_K15) // hack since there's no encoder for the aux driver
  &encoderZ
#endif
};
#endif

static AccelStepper* Steppers[NUM_STEPPERS] = {
  &stepperX,
  &stepperE,
  &stepperY,
  &stepperZ,
#if defined(FARMDUINO_K15)
  &stepperAUX
#endif
};

static TMC2130Stepper* _Steppers[NUM_STEPPERS] = {
  &TMC2130X,
  &TMC2130E,
  &TMC2130Y,
  &TMC2130Z,
#if defined(FARMDUINO_K15)
  &TMC2130AUX
#endif
};

/** Packet that is currently being built and processed */
CommsPacket_t CurrentPacket;

void setup() 
{
#if defined(HAS_ENCODERS)
  // setup encoders
  pinMode(MDL_ENABLE_PIN, OUTPUT);
  pinMode(MDL_CHIP_SELECT, OUTPUT);
  digitalWrite(MDL_ENABLE_PIN, HIGH);
  digitalWrite(MDL_CHIP_SELECT, HIGH);
#endif

  Serial.begin(9600);
  SPI.begin();

  // Initialize drivers
  for(size_t i = 0; i<NUM_STEPPERS; i++) {
    _Steppers[i]->begin(); 			
	  _Steppers[i]->rms_current(CURRENT_MA); 	
	  _Steppers[i]->stealthChop(1); 	
    _Steppers[i]->stealth_autoscale(1);
    _Steppers[i]->microsteps(16);
    Steppers[i]->setMaxSpeed(MAX_SPEED); 
    Steppers[i]->setAcceleration(ACCELERATION); 
    Steppers[i]->setEnablePin(X_ENABLE_PIN);
    Steppers[i]->setPinsInverted(false, false, true);
    Steppers[i]->enableOutputs();
  }

  // setup pins
  pinMode(LIGHTING_PIN, OUTPUT);
  digitalWrite(LIGHTING_PIN, LOW);

  pinMode(WATER_PIN, OUTPUT);
  digitalWrite(WATER_PIN, LOW);

  pinMode(VACUUM_PIN, OUTPUT);
  digitalWrite(VACUUM_PIN, LOW);

#if defined(FARMDUINO_K15)
  pinMode(PERIPHERAL_4_PIN, OUTPUT);
  digitalWrite(PERIPHERAL_4_PIN, LOW);

  pinMode(PERIPHERAL_5_PIN, OUTPUT);
  digitalWrite(PERIPHERAL_5_PIN, LOW);
#endif

  DEBUG_PRINT("SETUP.......");
  CurrentPacket._state = COMMS_STATE_OP;
  CurrentPacket.op = PACKET_OP_NOOP;
  CurrentPacket._index = 0;
  CurrentPacket.payloadLength = 0;
  for(uint8_t i = 0; i < COMMS_BUFFER_MAX; i++)
    CurrentPacket.payload[i] = 0x69;
  DEBUG_PRINT("ok\r\n");
}

void loop() {
  if(CurrentPacket._state == COMMS_STATE_COMPLETE) {
    DEBUG_PRINT("packet read complete op=%d payloadLength=%d \r\n", CurrentPacket.op, CurrentPacket.payloadLength);
    for (uint8_t i = 0; i < CurrentPacket.payloadLength; i++) {
        DEBUG_PRINT("0x%02X ", CurrentPacket.payload[i]);
    }
    DEBUG_PRINT("\r\n");
    uint32_t returnValue;
    switch(CurrentPacket.op) {
      case PACKET_OP_NOOP:
        DEBUG_PRINT("processing noop\r\n");
        returnValue = 0;
      break;
      case PACKET_OP_MOVEMENT:
        DEBUG_PRINT("processing movement command\r\n");
#if defined(HAS_ENCODERS)
        returnValue = process_movement(&CurrentPacket, Steppers, Encoders);
#else
        returnValue = process_movement(&CurrentPacket, Steppers);
#endif
      break;
      case PACKET_OP_PIN:
        DEBUG_PRINT("processing pin control\r\n");
        returnValue = process_pin(&CurrentPacket);
      break;
      case PACKET_OP_READY:
        DEBUG_PRINT("processing ready message\r\n");
        returnValue = 0;
      break;
      case PACKET_OP_TEST: 
        DEBUG_PRINT("processing TEST op\r\n");
        returnValue = process_test(&CurrentPacket);
      break;
      default:
        DEBUG_PRINT("unknown opcode %d\r\n", CurrentPacket.op);
        returnValue = 0;
    }

    // Reset the buffer
    CurrentPacket._state = COMMS_STATE_OP;
    CurrentPacket.op = PACKET_OP_NOOP;
    CurrentPacket.payloadLength = 0;
    CurrentPacket._index = 0;
    for(uint8_t i = 0; i < COMMS_BUFFER_MAX; i++)
      CurrentPacket.payload[i] = 0;

    uint8_t commandCompleteBuffer[6];
    commandCompleteBuffer[0] = PACKET_OP_NOOP;
    commandCompleteBuffer[1] = 4;
    commandCompleteBuffer[2] = returnValue >> 24;
    commandCompleteBuffer[3] = returnValue >> 16;
    commandCompleteBuffer[4] = returnValue >>  8;
    commandCompleteBuffer[5] = returnValue;
    Serial.write(commandCompleteBuffer, 6);
  }
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    uint8_t in = (uint8_t)Serial.read();
    // DEBUG_PRINT("received serial byte\r\n");
    switch(CurrentPacket._state) {
      // Read OP code first
      case COMMS_STATE_OP: {
        CurrentPacket.op = (CommsPacketOp_t)in;
        CurrentPacket._state = COMMS_STATE_PAYLOAD_LENGTH;
        DEBUG_PRINT("advancing to read payload length state\r\n");
        break;
      }

      // Read payload length second
      case COMMS_STATE_PAYLOAD_LENGTH: {
        CurrentPacket.payloadLength = in;
        CurrentPacket._index = 0;
        if(CurrentPacket.payloadLength == 0) {
          DEBUG_PRINT("advancing to complete state (because leng=0) \r\n");
          CurrentPacket._state = COMMS_STATE_COMPLETE;
        } else {
          DEBUG_PRINT("advancing to read payload state (reading %d bytes)\r\n", CurrentPacket.payloadLength);
          CurrentPacket._state = COMMS_STATE_PAYLOAD;
        }
        break;
      }

      // Read bytes for payload until 
      case COMMS_STATE_PAYLOAD: {
        CurrentPacket.payload[CurrentPacket._index] = in;
        DEBUG_PRINT("paylod byte read\r\n");
        if(CurrentPacket._index+1 == CurrentPacket.payloadLength) {
          DEBUG_PRINT("advancing to complete state\r\n");
          CurrentPacket._state = COMMS_STATE_COMPLETE;
          CurrentPacket._index = 0;
        } else {
          CurrentPacket._index++;
        }
        break;
      }
      default:
        DEBUG_PRINT("unknown state %d \r\n", CurrentPacket._state);
    }
  }
}