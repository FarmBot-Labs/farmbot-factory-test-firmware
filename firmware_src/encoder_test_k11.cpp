void encoderTest() {
  uint16_t val;
  const int spi_encoder_offset = 4;
  const byte read_cmd  = 0x0F;
  const byte reset_cmd = 0xAA;

  digitalWrite(MDL_CHIP_SELECT,LOW);
  delay(20);
  SPI.transfer(reset_cmd);
  digitalWrite(MDL_CHIP_SELECT,HIGH);
  delay(20);

  while(true) {
    // X1
    val = 0;
    digitalWrite(MDL_CHIP_SELECT,LOW);
    SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE3));
    SPI.transfer(read_cmd | (0b0001 << spi_encoder_offset) );
    delayMicroseconds(5);
    for (size_t i = 0; i < MDL_SPI_READ_SIZE+1; ++i) {
      val <<= 8;
      val |= SPI.transfer(0x01);
    }
    digitalWrite(MDL_CHIP_SELECT, HIGH);
    SPI.endTransaction();
    debugPrint("X1=");
    printBits(val);
    debugPrint("%d \r\n", val);

    // X2
    val = 0;
    digitalWrite(MDL_CHIP_SELECT,LOW);
    SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE3));
    SPI.transfer(read_cmd | (0b0010 << spi_encoder_offset) );
    delayMicroseconds(5);
    for (size_t i = 0; i < MDL_SPI_READ_SIZE+1; ++i) {
      val <<= 8;
      val |= SPI.transfer(0x01);
    }
    digitalWrite(MDL_CHIP_SELECT, HIGH);
    SPI.endTransaction();
    debugPrint("X2=");
    printBits(val);
    debugPrint("%d \r\n", val);

    // Y1
    val = 0;
    digitalWrite(MDL_CHIP_SELECT,LOW);
    SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE3));
    SPI.transfer(read_cmd | (0b0100 << spi_encoder_offset) );
    delayMicroseconds(5);
    for (size_t i = 0; i < MDL_SPI_READ_SIZE+1; ++i) {
      val <<= 8;
      val |= SPI.transfer(0x01);
    }
    digitalWrite(MDL_CHIP_SELECT, HIGH);
    SPI.endTransaction();
    debugPrint("Y1=");
    printBits(val);
    debugPrint("%d \r\n", val);

    // Z1
    val = 0;
    digitalWrite(MDL_CHIP_SELECT,LOW);
    SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE3));
    SPI.transfer(read_cmd | (0b1000 << spi_encoder_offset) );
    delayMicroseconds(5);
    for (size_t i = 0; i < MDL_SPI_READ_SIZE+1; ++i) {
      val <<= 8;
      val |= SPI.transfer(0x01);
    }
    digitalWrite(MDL_CHIP_SELECT, HIGH);
    SPI.endTransaction();
    debugPrint("Z1=");
    printBits(val);
    debugPrint("%d \r\n", val);

    debugPrint("\r\n\r\n");
    delay(500);
  }
}
