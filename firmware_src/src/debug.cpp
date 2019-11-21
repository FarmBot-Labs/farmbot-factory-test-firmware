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