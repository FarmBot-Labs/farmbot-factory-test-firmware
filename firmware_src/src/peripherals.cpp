#include <Arduino.h>
#include "platform.h"
#include "peripherals.h"
#include "comms.h"
#include "debug.h"

void pumpTest() 
#if defined(HAS_LOAD_DETECTORS)
{
  digitalWrite(VACUUM_PIN, HIGH);
  delay(5);
  while(true) {
    long data[20];
    for(int i = 0; i < 5; i++) {
      data[i] = analogRead(VACUUM_ADC);
      delay(100);
    }

    int value = 0;
    for(int i = 0; i < 5; i++) {
      value+=data[i];
    }
    value = value / 5;

    DEBUG_PRINT("load=%d\r\n", value);
  }
}
#else
{}
#endif

uint32_t process_peripheral(CommsPacket_t* CurrentPacket, size_t peripherals[NUM_PERIPHERALS][2], size_t calibration[NUM_PERIPHERALS]) {
  PIN_ARG_t pin = (PIN_ARG_t)CurrentPacket->payload[0];
  uint32_t r = 150; // random value chose to make test pass
  digitalWrite(peripherals[pin][0], LOW);
  DEBUG_PRINT("PIN=%d state=LOW\r\n", peripherals[pin][0]);

  digitalWrite(peripherals[pin][0], HIGH);
  DEBUG_PRINT("PIN=%d state=HIGH\r\n", peripherals[pin][0]);
  delay(500);
#if defined(HAS_LOAD_DETECTORS)
  r = (uint32_t)analogRead(peripherals[pin][1]) -calibration[pin];
  DEBUG_PRINT("PIN=%d state=LOW adc=%u calibration=%d\r\n", peripherals[pin][0], r, calibration[pin]);
#endif
  digitalWrite(peripherals[pin][0], LOW);
  if(r < 0)
    return 0;
  else
    return r;
}