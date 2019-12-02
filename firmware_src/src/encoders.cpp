#include <Arduino.h>
#include <SPI.h>

#include "encoders.h"
static const byte MDL_SPI_CMD_READ = 0x0F;
static const byte MDL_SPI_CMD_RESET = 0x00;
static const int MDL_SPI_OFFSET    = 4;

MDLEncoder::MDLEncoder(MDLEncoder::MDLEncoderID_t encoder) {
  _encoder = encoder;
}

void MDLEncoder::begin() {
  ;;
}

void MDLEncoder::reset() {
  digitalWrite(MDL_CHIP_SELECT,LOW);
  SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));

  SPI.transfer(MDL_SPI_CMD_RESET | (_encoder << MDL_SPI_OFFSET) );

  SPI.endTransaction();

  digitalWrite(MDL_CHIP_SELECT, HIGH);
}

long MDLEncoder::read() {
  _encoderValue = 0;
  
  digitalWrite(MDL_CHIP_SELECT,LOW);
  SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));

  SPI.transfer(MDL_SPI_CMD_READ | (_encoder << MDL_SPI_OFFSET) );
  delayMicroseconds(5);

  for (size_t i = 0; i < MDL_SPI_READ_SIZE; ++i) {
    _encoderValue <<= 8;
    _encoderValue |= SPI.transfer(0x01);
  }
  
  digitalWrite(MDL_CHIP_SELECT, HIGH);
  SPI.endTransaction();
  delay(20);
  return _encoderValue;
}
