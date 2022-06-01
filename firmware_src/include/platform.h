#ifndef PLATFORM_H
#define PLATFORM_H

  #if defined(MIX_TARGET)
    #if defined(MIX_TARGET_host)
      #include "pins_farmduino_k16.h"
    #endif

    #if defined(MIX_TARGET_rpi02w)
      #include "pins_express_k11.h"
    #endif

    #if defined(MIX_TARGET_rpi4)
      #include "pins_farmduino_k16.h"
    #endif
  #else // if MIX_TARGET is not defined
      // #include "pins_express_k11.h"
      #include "pins_farmduino_k16.h"
  #endif
#endif
