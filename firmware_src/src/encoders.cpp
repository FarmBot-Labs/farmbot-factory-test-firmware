#include <Arduino.h>
#include <SPI.h>

#include "encoders.h"

MDLEncoder::MDLEncoder(MDLEncoder::MDLEncoderID_t encoder) {
  _encoder = encoder;
}

void MDLEncoder::begin() {
  ;;
}

void MDLEncoder::reset() {
  digitalWrite(MDL_CHIP_SELECT,LOW);
  SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));

  SPI.transfer(MDL_SPI_CMD_WRITE | (_encoder << MDL_SPI_OFFSET) );

  SPI.endTransaction();

  digitalWrite(MDL_CHIP_SELECT, HIGH);
}

long MDLEncoder::read() {
  _encoderValue = 0;
  
  digitalWrite(MDL_CHIP_SELECT,LOW);
  SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));

  SPI.transfer(MDL_SPI_CMD_READ | (_encoder << MDL_SPI_OFFSET) );
  delayMicroseconds(10);

  for (size_t i = 0; i < MDL_SPI_READ_SIZE; ++i) {
    _encoderValue <<= 8;
    _encoderValue |= SPI.transfer(0x01);
  }
  
  digitalWrite(MDL_CHIP_SELECT, HIGH);
  SPI.endTransaction();
  return _encoderValue;
}

// void MDLEncoder::encoderTest() {
//   pinMode(MDL_ENABLE_PIN, INPUT_PULLUP);
//   pinMode(MDL_CHIP_SELECT, OUTPUT);
//   digitalWrite(MDL_CHIP_SELECT, HIGH);

//   Serial.begin(9600);
  
//   SPI.setBitOrder(MSBFIRST);
//   SPI.setDataMode(SPI_MODE0);
//   SPI.setClockDivider(SPI_CLOCK_DIV4);
//   SPI.begin();
//   //SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
  
//   ResetEncoder(MDL_X1);
//   ResetEncoder(MDL_X2);
//   ResetEncoder(MDL_Y);
//   ResetEncoder(MDL_Z);
//   delay(100);

//   while(1==1) {
//     long X1EncoderData = ReadEncoder(MDL_X1);
//     long X2EncoderData = ReadEncoder(MDL_X2);
//     long YEncoderData = ReadEncoder(MDL_Y);
//     long ZEncoderData = ReadEncoder(MDL_Z);
//     Serial.print("EncoderData");
//     Serial.print(" X1: ");
//     Serial.print(X1EncoderData);
//     Serial.print(" X2: ");
//     Serial.print(X2EncoderData);
//     Serial.print(" Y: ");
//     Serial.print(YEncoderData);
//     Serial.print(" Z: ");
//     Serial.println(ZEncoderData);
//     delay(200);
//   }
// }