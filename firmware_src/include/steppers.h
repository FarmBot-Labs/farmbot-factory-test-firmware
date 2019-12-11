#ifndef STEPPERS_H
#define STEPPERS_H

#include <AccelStepper.h>
#include "comms.h"
#include "encoders.h"

#if defined(HAS_ENCODERS)
uint32_t process_movement(CommsPacket_t* CurrentPacket, AccelStepper* steppers[], MDLEncoder* encoders[]);
#else
uint32_t process_movement(CommsPacket_t* CurrentPacket, AccelStepper* steppers[]);
#endif

#endif