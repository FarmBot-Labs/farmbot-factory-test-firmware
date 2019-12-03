#include <Arduino.h>
#include <TMC2130Stepper.h>

//////////////// X1 ////////////////
#define X1_CS      24
#define X1_EN      25
#define X1_DIR     27
#define X1_STEP    26

//////////////// X2 ////////////////
#define X2_CS      29
#define X2_EN      14
#define X2_DIR     30
#define X2_STEP    15

//////////////// Y ////////////////
#define Y_CS     28
#define Y_EN     31
#define Y_DIR    33
#define Y_STEP   32

//////////////// Z ////////////////
#define Z_CS     23
#define Z_EN     34
#define Z_DIR    36
#define Z_STEP   35

//////////////// AUX ////////////////
#define AUX_CS     43
#define AUX_EN     37
#define AUX_DIR    41
#define AUX_STEP   40

#define   FET1  7
#define   FET2  8
#define   FET3  9
#define   FET4  10
#define   FET5  12
#define   LED6  13

#define   ADC1  A0
#define   ADC2  A1
#define   ADC3  A4
#define   ADC4  A3
#define   ADC5  A2

//////////////// STM32 ////////////////
#define NSS_PIN 22 
#define STM_RST_PIN 49

static const int spi_encoder_offset = 4;
static const bool forward_val = false;
static const bool reverse_val = true;
static const long num_pulse = 0x8fff;
static const long adcThreshold = 100;
static const long encoderThreshold = 100;

enum SpiEncoders
{
  _X1 = 0b0001,
  _X2 = 0b0010,
  _Y  = 0b0100,
  _Z  = 0b1000,
};

static const uint8_t enArray[5] = {X1_EN, X2_EN, Y_EN, Z_EN, AUX_EN};
static const uint8_t dirArray[5] = {X1_DIR, X2_DIR, Y_DIR, Z_DIR, AUX_DIR};
static const uint8_t stepArray[5] = {X1_STEP, X2_STEP, Y_STEP, Z_STEP, AUX_STEP};

TMC2130Stepper driverX1 = TMC2130Stepper(X1_EN, X1_DIR, X1_STEP, X1_CS);
TMC2130Stepper driverX2 = TMC2130Stepper(X2_EN, X2_DIR, X2_STEP, X2_CS);
TMC2130Stepper driverY = TMC2130Stepper(Y_EN, Y_DIR, Y_STEP, Y_CS);
TMC2130Stepper driverZ = TMC2130Stepper(Z_EN, Z_DIR, Z_STEP, Z_CS);
TMC2130Stepper driverAUX = TMC2130Stepper(AUX_EN, AUX_DIR, AUX_STEP, AUX_CS);
TMC2130Stepper driver[5] = {driverX1, driverX2, driverY, driverZ, driverAUX};

void SendPulse(long pulse,bool dir);
void RunTest();
long ReadEncoder(SpiEncoders encoder);
void ResetEncoder();

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Start...");
  SPI.begin();

  digitalWrite(NSS_PIN, HIGH);
  pinMode(NSS_PIN, OUTPUT);
  digitalWrite(STM_RST_PIN, HIGH);
  pinMode(STM_RST_PIN, OUTPUT);
  pinMode(MISO, INPUT_PULLUP);

  for(int index = 0; index < 5; index++)
  {
    driver[index].begin();       // Initiate pins and registeries
    driver[index].rms_current(600);  // Set stepper current to 600mA. The command is the same as command TMC2130.setCurrent(600, 0.11, 0.5);
    driver[index].stealthChop(1);  // Enable extremely quiet stepping
  }

  //enable device
  for(int index = 0; index < 5; index++)
  {
    pinMode(enArray[index],OUTPUT);
    digitalWrite(enArray[index],LOW);
  }

  //mosfets
  pinMode(FET1,OUTPUT);
  pinMode(FET2,OUTPUT);
  pinMode(FET3,OUTPUT);
  pinMode(FET4,OUTPUT);
  pinMode(FET5,OUTPUT);
  pinMode(LED6,OUTPUT);

  //reset the STM MCU
  digitalWrite(STM_RST_PIN, LOW);
  delay(5);
  digitalWrite(STM_RST_PIN, HIGH);
  delay(20);
  
  //reset all encoder
  ResetEncoder();
  delay(20);
  ResetEncoder();
  delay(200);

  RunTest();
}
 
void loop() 
{
}


//runs the full test
void RunTest()
{
  bool encoder1Error = false;
  bool encoder2Error = false;
  bool encoder3Error = false;
  bool encoder4Error = false;
  
  bool adc1Error = false;
  bool adc2Error = false;
  bool adc3Error = false;
  bool adc4Error = false;
  bool adc5Error = false;

  long encoder1 = 0;
  long encoder2 = 0;
  long encoder3 = 0;
  long encoder4 = 0;

  //turn on run time led
  digitalWrite(LED6,HIGH);

  //move motors forward
  SendPulse(num_pulse, forward_val);

  //read the encoders
  encoder1 = ReadEncoder(_X1);
  encoder2 = ReadEncoder(_X2);
  encoder3 = ReadEncoder(_Y);
  encoder4 = ReadEncoder(_Z);
  
  Serial.print("X1 reading: ");
  Serial.println(encoder1);
  Serial.print("X2 reading: ");
  Serial.println(encoder2);
  Serial.print("Y reading: ");
  Serial.println(encoder3);
  Serial.print("Z reading: ");
  Serial.println(encoder4);
  
  encoder1Error = encoder1 < encoderThreshold;
  encoder2Error = encoder2 < encoderThreshold;
  encoder3Error = encoder3 < encoderThreshold;
  encoder4Error = encoder4 < encoderThreshold;

  delay(200);

  //turn on mosfets
  digitalWrite(FET1,1);
  digitalWrite(FET2,1);
  digitalWrite(FET3,1);
  digitalWrite(FET4,1);
  digitalWrite(FET5,1);
  
  //move motors backwards
  SendPulse(num_pulse, reverse_val);

  //read adc value
  adc1Error = analogRead(ADC1) > adcThreshold;
  adc2Error = analogRead(ADC2) > adcThreshold;
  adc3Error = analogRead(ADC3) > adcThreshold;
  adc4Error = analogRead(ADC4) > adcThreshold;
  adc5Error = analogRead(ADC5) > adcThreshold;
  
  //turn off mosfets with OK readings
  digitalWrite(FET1, !adc1Error);
  digitalWrite(FET2, !adc2Error);
  digitalWrite(FET3, !adc3Error);
  digitalWrite(FET4, !adc4Error);
  digitalWrite(FET5, !adc5Error);

  //read encoder again
  encoder1Error |= ReadEncoder(_X1) > encoderThreshold;
  encoder2Error |= ReadEncoder(_X2) > encoderThreshold;
  encoder3Error |= ReadEncoder(_Y) > encoderThreshold;
  encoder4Error |= ReadEncoder(_Z) > encoderThreshold;

  //turn off run time led
  digitalWrite(LED6, LOW);

  //move motors with bad encoder readings
  while(1)
  {
    if(encoder1Error) digitalWrite(stepArray[0], HIGH);
    if(encoder2Error) digitalWrite(stepArray[1], HIGH);
    if(encoder3Error) digitalWrite(stepArray[2], HIGH);
    if(encoder4Error) digitalWrite(stepArray[3], HIGH);
    delayMicroseconds(1);
    if(encoder1Error) digitalWrite(stepArray[0], LOW);
    if(encoder2Error) digitalWrite(stepArray[1], LOW);
    if(encoder3Error) digitalWrite(stepArray[2], LOW);
    if(encoder4Error) digitalWrite(stepArray[3], LOW);
    delayMicroseconds(1);
  }
}


void ResetEncoder()
{
  const byte reset_cmd = 0xF0;

  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  digitalWrite(NSS_PIN,LOW);
  delayMicroseconds(2);
  SPI.transfer(reset_cmd);
  delayMicroseconds(5);
  digitalWrite(NSS_PIN,HIGH);
  SPI.endTransaction();
  delay(5);
}


long ReadEncoder(SpiEncoders encoder)
{
  const byte read_cmd  = 0x0F;
  const int readSize = 4;
  long encoderVal = 0;

  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  digitalWrite(NSS_PIN,LOW);
  delayMicroseconds(2);
  SPI.transfer(read_cmd | (encoder << spi_encoder_offset) );
  delayMicroseconds(5);

  for (int i = 0; i < readSize; ++i)
  {
    encoderVal <<= 8;
    encoderVal |= SPI.transfer(0x01);
  }
  
  digitalWrite(NSS_PIN,HIGH);
  SPI.endTransaction();
  delay(10);
  return abs(encoderVal);
}


//sends a series of pulses to move the stepper motors
void SendPulse(long pulse,bool dir)
{
  for(int index = 0; index < 5; index++)
    digitalWrite(dirArray[index], dir);
  while(pulse-- > 0)
  {
    for(int index = 0; index < 5; index++)
    {
      digitalWrite(stepArray[index], HIGH);
      delayMicroseconds(1);
      digitalWrite(stepArray[index], LOW);
      delayMicroseconds(1);
    }
  }
}
