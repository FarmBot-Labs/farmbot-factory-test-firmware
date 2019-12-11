#ifndef STEPPERS_H
#define STEPPERS_H

#include <AccelStepper.h>
#include "comms.h"
#include "encoders.h"

#define CURRENT_MA 600
#define STEPS_PER_MM 5
#define MAX_SPEED 1000*STEPS_PER_MM
#define ACCELERATION 1000*STEPS_PER_MM

#if defined(HAS_ENCODERS)
uint32_t process_movement(CommsPacket_t* CurrentPacket, AccelStepper* steppers[], MDLEncoder* encoders[]);
#else
uint32_t process_movement(CommsPacket_t* CurrentPacket, AccelStepper* steppers[]);
#endif

#endif