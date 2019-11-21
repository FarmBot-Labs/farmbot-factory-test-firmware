#ifndef FARMDUINO_ENCODER_H
#define FARMDUINO_ENCODER_H

#define MDL_CHIP_SELECT 22 
#define MDL_ENABLE_PIN 49

#define MDL_SPI_OFFSET 4
#define MDL_SPI_READ_SIZE 4
#define MDL_SPI_CMD_READ 0x0F
#define MDL_SPI_CMD_WRITE 0x00

/* 
// test encoders
while(1==1) {
  uint32_t val = encoderX.read();
  DEBUG_PRINT("X1=");
  DEBUG_PRINT("%d\r\n", val);
  delay(200);
}
*/

class MDLEncoder {
  public:
    typedef enum {
      MDL_X1 = 0b0001,
      MDL_X2 = 0b0010,
      MDL_Y = 0b0100,
      MDL_Z = 0b1000,
    } MDLEncoderID_t;

    MDLEncoder(MDLEncoderID_t encoder);
    void begin();
    void reset();
    long read();
  private:
    MDLEncoderID_t _encoder;
    uint32_t _encoderValue;
};

#endif