#include <Arduino.h> 
#include <stdarg.h>
#include "debug.h"
#include "comms.h"

void DEBUG_PRINT(const char *format,...)
{
  char buff[255];
  va_list args;
  va_start (args,format);
  size_t bytes_minus_null_written = vsnprintf(buff,sizeof(buff),format,args);
  va_end (args);
  buff[bytes_minus_null_written+1]='\0';

  char header[2];
  header[0] = PACKET_OP_DEBUG;
  header[1] = bytes_minus_null_written;
  Serial.write(header, 2);
  Serial.write(buff, bytes_minus_null_written);
  Serial.flush();
}

void debugPrint(const char *format,...)
{
  char buff[255];
  va_list args;
  va_start (args,format);
  size_t bytes_minus_null_written = vsnprintf(buff,sizeof(buff),format,args);
  va_end (args);
  buff[bytes_minus_null_written+1]='\0';
  Serial.write(buff, bytes_minus_null_written);
  Serial.flush();
}

void printBits(uint16_t num) {
  debugPrint("| ");
  for(int bit=0;bit<(sizeof(uint16_t) * 8); bit++)
  {
    if(bit == 3 || bit == 7 || bit == 11 || bit == 15) {
      debugPrint("%i | ", num & 0x01);
    } else {
      debugPrint("%i ", num & 0x01);
    }
    num = num >> 1;
  }
}