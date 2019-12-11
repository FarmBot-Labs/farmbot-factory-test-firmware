#include <Arduino.h>
#include <AccelStepper.h>

#include "platform.h"
#include "steppers.h"
#include "debug.h"
#include "comms.h"

/** Does moves a number of steps **/
#if defined(HAS_ENCODERS)
uint32_t process_movement(CommsPacket_t* CurrentPacket, AccelStepper* steppers[], MDLEncoder* encoders[])
#else
uint32_t process_movement(CommsPacket_t* CurrentPacket, AccelStepper* steppers[])
#endif
{
  MOVEMENT_AXIS_t axis = (MOVEMENT_AXIS_t)CurrentPacket->payload[0];
  DEBUG_PRINT("selecting axis %d\r\n", axis);
  AccelStepper* stepper = steppers[axis];
  uint32_t numSteps = 
      (uint32_t)CurrentPacket->payload[1] << 24 |
      (uint32_t)CurrentPacket->payload[2] << 16 |
      (uint32_t)CurrentPacket->payload[3] << 8  |
      (uint32_t)CurrentPacket->payload[4];
  uint32_t ret = 0;
#if defined(HAS_ENCODERS)
  MDLEncoder* encoder = encoders[axis];
#endif

  DEBUG_PRINT("moving %u steps\r\n", numSteps);

#if defined(HAS_ENCODERS)
  encoder->reset();
  DEBUG_PRINT("reset encoder\r\n");
#endif
  stepper->move(numSteps * STEPS_PER_MM);
  stepper->enableOutputs();
  DEBUG_PRINT("motor enabled\r\n");
  while (stepper->distanceToGo() != 0) {
    stepper->run();
// don't read every loop. it will case the motors to move very slow
// #if defined(HAS_ENCODERS)
//     ret = encoder->read();
//     // DEBUG_PRINT("encoders read %d\r\n", ret);
// #endif
  }
  DEBUG_PRINT("moved for %u steps\r\n", numSteps);
    
  stepper->disableOutputs();
  DEBUG_PRINT("motor disabled\r\n");
#if defined(HAS_ENCODERS)
  ret = encoder->read();
  DEBUG_PRINT("MOVEMENT OK ret=%d\r\n", ret);
#else
  DEBUG_PRINT("MOVEMENT OK\r\n");
  ret = (uint32_t)numSteps;
#endif
  return ret;
}