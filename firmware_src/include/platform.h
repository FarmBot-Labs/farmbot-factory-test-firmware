#ifndef PLATFORM_H
#define PLATFORM_H

  #if defined(MIX_TARGET)
    #if defined(MIX_TARGET_host)
      #include "pins_farmduino_k15.h"
    #endif

    #if defined(MIX_TARGET_rpi)
      #include "pins_express_k10.h"
    #endif

    #if defined(MIX_TARGET_rpi3)
      #include "pins_farmduino_k15.h"

    #endif
  #else // if MIX_TARGET is not defined
      // #include "pins_express_k10.h"
      #include "pins_farmduino_k15.h"
  #endif
#endif