#include <Arduino.h>
#include <SPI.h>

#include "encoders.h"

void encoderTest()
#if defined(HAS_ENCODERS)
{
  uint16_t val;
  const int spi_encoder_offset = 4;
  const byte read_cmd  = 0x0F;
  const byte reset_cmd = 0xAA;

  digitalWrite(MDL_ENABLE_PIN, LOW);
  delay(5);
  digitalWrite(MDL_ENABLE_PIN, HIGH);
  delay(20);

  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE3));
  digitalWrite(MDL_CHIP_SELECT,LOW);
  delayMicroseconds(2);
  SPI.transfer(reset_cmd);
  delayMicroseconds(5);
  digitalWrite(MDL_CHIP_SELECT,HIGH);
  SPI.endTransaction();
  delay(20);

  // 1.5
  while(true) {
    // X1
    val = 0;
    SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE3));
    digitalWrite(MDL_CHIP_SELECT,LOW);
    delayMicroseconds(5);
    SPI.transfer(read_cmd | (0b0001 << spi_encoder_offset) );
    delayMicroseconds(5);

    for (size_t i = 0; i < MDL_SPI_READ_SIZE; ++i) {
      val <<= 8;
      val |= SPI.transfer(0x01);
    }
    digitalWrite(MDL_CHIP_SELECT,HIGH);
    SPI.endTransaction();
    delay(10);
    debugPrint("X1=");
    print_uint16(val);
    debugPrint("%d \r\n", val);
    debugPrint("\r\n\r\n");
    delay(500);
  }
}
#else
{}
#endif

MDLEncoder::MDLEncoder(MDLEncoder::MDLEncoderID_t encoder) {
  _encoder = encoder;
}

void MDLEncoder::reset() {
  digitalWrite(MDL_ENABLE_PIN, LOW);
  delay(5);
  digitalWrite(MDL_ENABLE_PIN, HIGH);
  delay(20);

  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE3));
  digitalWrite(MDL_CHIP_SELECT,LOW);
  delayMicroseconds(2);
  SPI.transfer(reset_cmd);
  delayMicroseconds(5);
  digitalWrite(MDL_CHIP_SELECT,HIGH);
  SPI.endTransaction();
  delay(20);
}

long MDLEncoder::read() {
  _encoderValue = 0;
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE3));
  digitalWrite(MDL_CHIP_SELECT,LOW);
  delayMicroseconds(5);
  SPI.transfer(read_cmd | (0b0001 << spi_encoder_offset) );
  delayMicroseconds(5);

  for (size_t i = 0; i < MDL_SPI_READ_SIZE; ++i) {
    _encoderValue <<= 8;
    _encoderValue |= SPI.transfer(0x01);
  }
  digitalWrite(MDL_CHIP_SELECT,HIGH);
  SPI.endTransaction();
  delay(10);
  return _encoderValue;
}
