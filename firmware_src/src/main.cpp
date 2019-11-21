#include <stdarg.h>
#include <Arduino.h>

#include <TMC2130Stepper.h>
#include <AccelStepper.h>

#include "platform.h"
#include "comms.h"
void DEBUG_PRINT(const char *format,...)
{
  char buff[255];
  va_list args;
  va_start (args,format);
  size_t bytes_minus_null_written = vsnprintf(buff,sizeof(buff),format,args);
  va_end (args);
  buff[bytes_minus_null_written+1]='\0';

  char header[2];
  header[0] = PACKET_OP_DEBUG;
  header[1] = bytes_minus_null_written;
  Serial.write(header, 2);
  Serial.write(buff, bytes_minus_null_written);
  Serial.flush();
}

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

/** Packet that is currently being built and processed */
CommsPacket_t CurrentPacket;

/** Does moves a number of steps **/
void process_movement() {
  MOVEMENT_AXIS_t axis = (MOVEMENT_AXIS_t)CurrentPacket.payload[0];
  uint32_t numSteps = 
      (uint32_t)CurrentPacket.payload[1] << 24 |
      (uint32_t)CurrentPacket.payload[2] << 16 |
      (uint32_t)CurrentPacket.payload[3] << 8  |
      (uint32_t)CurrentPacket.payload[4];
  AccelStepper* stepper;
  switch(axis) {
    case MOVEMENT_AXIS_X1:
      DEBUG_PRINT("spinning x1 numSteps=%d\r\n", numSteps);
      stepper = &stepperX;
    break;

    case MOVEMENT_AXIS_X2:
      DEBUG_PRINT("spinning x2 numSteps=%d\r\n", numSteps);
      stepper = &stepperE;
    break;

    case MOVEMENT_AXIS_Y1:
      DEBUG_PRINT("spinning y1 numSteps=%d\r\n", numSteps);
      stepper = &stepperY;
    break;

    case MOVEMENT_AXIS_Z1:
      DEBUG_PRINT("spinning z1 numSteps=%d\r\n", numSteps);
      stepper = &stepperZ;
    break;
#if defined(FARMDUINO_K15)
    case MOVEMENT_AXIS_AUX:
      DEBUG_PRINT("spinning aux numSteps=%d\r\n", numSteps);
      stepper = &stepperAUX;
    break;
#endif
  }

  stepper->move(numSteps);
  stepper->enableOutputs();

  while (stepper->distanceToGo() != 0)
    stepper->run();
    
  stepper->disableOutputs();
  DEBUG_PRINT("MOVEMENT OK\r\n");
}

void process_pin() {
  PIN_ARG_t pin = (PIN_ARG_t)CurrentPacket.payload[0];
  uint8_t pinNum;
  switch(pin) {
    case PIN_ARG_LIGHTING:
      DEBUG_PRINT("testing PIN_ARG_LIGHTING\r\n");
      pinNum = LIGHTING_PIN;
    break;
    case PIN_ARG_WATER:
      DEBUG_PRINT("testing PIN_ARG_WATER\r\n");
      pinNum = WATER_PIN;
    break;
    case PIN_ARG_VACUUM:
      DEBUG_PRINT("testing PIN_ARG_VACUUM\r\n");
      pinNum = VACUUM_PIN;
    break;
#if defined(FARMDUINO_K15)
    case PIN_ARG_P4:
      DEBUG_PRINT("testing PIN_ARG_P4\r\n");
      pinNum = PERIPHERAL_4_PIN;
    break;
    case PIN_ARG_P5:
      DEBUG_PRINT("testing PIN_ARG_P5\r\n");
      pinNum = PERIPHERAL_5_PIN;
    break;
#endif
  }

  digitalWrite(pinNum, LOW);
  DEBUG_PRINT("PIN=%d state=LOW\r\n", pinNum);

  digitalWrite(pinNum, HIGH);
  DEBUG_PRINT("PIN=%d state=HIGH\r\n", pinNum);
  delay(500);
  digitalWrite(pinNum, LOW);
  DEBUG_PRINT("PIN=%d state=LOW\r\n", pinNum);
}

void setup() {
  Serial.begin(9600);
  SPI.begin();

  // Initialize drivers

  // X1
  TMC2130X.begin(); 			
	TMC2130X.rms_current(CURRENT_MA); 	
	TMC2130X.stealthChop(1); 	
  TMC2130X.stealth_autoscale(1);
  TMC2130X.microsteps(16);
  stepperX.setMaxSpeed(MAX_SPEED); 
  stepperX.setAcceleration(ACCELERATION); 
  stepperX.setEnablePin(X_ENABLE_PIN);
  stepperX.setPinsInverted(false, false, true);
  stepperX.enableOutputs();

  // X2
  TMC2130E.begin(); 			
	TMC2130E.rms_current(CURRENT_MA); 	
	TMC2130E.stealthChop(1); 	
  TMC2130E.stealth_autoscale(1);
  TMC2130E.microsteps(16);
  stepperE.setMaxSpeed(MAX_SPEED); 
  stepperE.setAcceleration(ACCELERATION); 
  stepperE.setEnablePin(E_ENABLE_PIN);
  stepperE.setPinsInverted(false, false, true);
  stepperE.enableOutputs();

  // Y1
  TMC2130Y.begin(); 			
	TMC2130Y.rms_current(CURRENT_MA); 	
	TMC2130Y.stealthChop(1); 	
  TMC2130Y.stealth_autoscale(1);
  TMC2130Y.microsteps(16);
  stepperY.setMaxSpeed(MAX_SPEED); 
  stepperY.setAcceleration(ACCELERATION); 
  stepperY.setEnablePin(Y_ENABLE_PIN);
  stepperY.setPinsInverted(false, false, true);
  stepperY.enableOutputs();

  // Z1
  TMC2130Z.begin(); 			
	TMC2130Z.rms_current(CURRENT_MA); 	
	TMC2130Z.stealthChop(1); 	
  TMC2130Z.stealth_autoscale(1);
  TMC2130Z.microsteps(16);
  stepperZ.setMaxSpeed(MAX_SPEED); 
  stepperZ.setAcceleration(ACCELERATION); 
  stepperZ.setEnablePin(Z_ENABLE_PIN);
  stepperZ.setPinsInverted(false, false, true);
  stepperZ.enableOutputs();

#if defined(FARMDUINO_K15)
  // AUX
  TMC2130AUX.begin(); 			
	TMC2130AUX.rms_current(CURRENT_MA); 	
	TMC2130AUX.stealthChop(1); 	
  TMC2130AUX.stealth_autoscale(1);
  TMC2130AUX.microsteps(16);
  stepperAUX.setMaxSpeed(MAX_SPEED); 
  stepperAUX.setAcceleration(ACCELERATION); 
  stepperAUX.setEnablePin(AUX_ENABLE_PIN);
  stepperAUX.setPinsInverted(false, false, true);
  stepperAUX.enableOutputs();
#endif

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
    switch(CurrentPacket.op) {
      case PACKET_OP_NOOP:
        DEBUG_PRINT("processing noop\r\n");
        break;
      case PACKET_OP_MOVEMENT:
        DEBUG_PRINT("processing movement command\r\n");
        process_movement();
        break;
      case PACKET_OP_PIN:
        DEBUG_PRINT("processing pin control\r\n");
        process_pin();
        break;
      case PACKET_OP_READY:
        DEBUG_PRINT("processing ready message\r\n");\
        break;
      default:
        DEBUG_PRINT("unknown opcode %d\r\n", CurrentPacket.op);
    }

    // Reset the buffer
    CurrentPacket._state = COMMS_STATE_OP;
    CurrentPacket.op = PACKET_OP_NOOP;
    CurrentPacket.payloadLength = 0;
    CurrentPacket._index = 0;
    for(uint8_t i = 0; i < COMMS_BUFFER_MAX; i++)
      CurrentPacket.payload[i] = 0;
    uint8_t commandCompleteBuffer[2] = {PACKET_OP_NOOP, 0};
    Serial.write(commandCompleteBuffer, 2);
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