#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#include "comms.h"
#include "platform.h"
void pumpTest();
uint32_t process_peripheral(CommsPacket_t* CurrentPacket, size_t peripherals[NUM_PERIPHERALS][2], size_t peripheral_calibration[NUM_PERIPHERALS]);

#endif 