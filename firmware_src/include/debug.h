#ifndef FARMDUINO_DEBUG
#define FARMDUINO_DEBUG

#include "comms.h"

/** printf */
void DEBUG_PRINT(const char *format,...);
void debugPrint(const char *format,...);
void print_uint16(uint16_t num);
uint32_t process_test(CommsPacket_t* CurrentPacket);

#endif