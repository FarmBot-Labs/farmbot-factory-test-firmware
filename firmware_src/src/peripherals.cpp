#include <Arduino.h>
#include "platform.h"
#include "peripherals.h"
#include "comms.h"
#include "debug.h"

uint32_t process_pin(CommsPacket_t* CurrentPacket) {
  PIN_ARG_t pin = (PIN_ARG_t)CurrentPacket->payload[0];
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
  return 0;
}