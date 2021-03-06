#ifndef __R51_DEBUG_H__
#define __R51_DEBUG_H__

#include <Arduino.h>

#include "bus.h"
#include "config.h"


#ifdef DEBUG_ENABLE

#ifndef DEBUG_SERIAL
#error DEBUG_SERIAL must be defined with DEBUG_ENABLE is set
#endif

#ifndef DEBUG_BAUDRATE
#error DEBUG_BAUDRATE must be defined with DEBUG_ENABLE is set
#endif

// Update these to change debug output settings.

size_t printDebugFrame(const Frame& frame);

#define D(x) x
#define DEBUG_BEGIN() DEBUG_SERIAL.begin(DEBUG_BAUDRATE)

#define INFO_MSG(MSG) ({\
    DEBUG_SERIAL.print("[INFO]  ");\
    DEBUG_SERIAL.println(MSG);\
    DEBUG_SERIAL.flush();\
})
#define INFO_MSG_VAL(MSG, VAL) ({\
    DEBUG_SERIAL.print("[INFO]  ");\
    DEBUG_SERIAL.print(MSG);\
    DEBUG_SERIAL.println(VAL);\
    DEBUG_SERIAL.flush();\
})
#define INFO_MSG_VAL_FMT(MSG, VAL, FMT) ({\
    DEBUG_SERIAL.print("[INFO]  ");\
    DEBUG_SERIAL.print(MSG);\
    DEBUG_SERIAL.println(VAL, FMT);\
    DEBUG_SERIAL.flush();\
})
#define INFO_MSG_FRAME(MSG, FRAME) ({\
    DEBUG_SERIAL.print("[INFO]  ");\
    DEBUG_SERIAL.print(MSG);\
    printDebugFrame(FRAME);\
    DEBUG_SERIAL.println("");\
    DEBUG_SERIAL.flush();\
})

#define ERROR_MSG(MSG) ({\
    DEBUG_SERIAL.print("[ERROR] ");\
    DEBUG_SERIAL.println(MSG);\
    DEBUG_SERIAL.flush();\
})
#define ERROR_MSG_VAL(MSG, VAL) ({\
    DEBUG_SERIAL.print("[ERROR] ");\
    DEBUG_SERIAL.print(MSG);\
    DEBUG_SERIAL.println(VAL);\
    DEBUG_SERIAL.flush();\
})
#define ERROR_MSG_VAL_FMT(MSG, VAL, FMT) ({\
    DEBUG_SERIAL.print("[ERROR] ");\
    DEBUG_SERIAL.print(MSG);\
    DEBUG_SERIAL.println(VAL, FMT);\
    DEBUG_SERIAL.flush();\
})
#define ERROR_MSG_FRAME(MSG, FRAME) ({\
    DEBUG_SERIAL.print("[ERROR] ");\
    DEBUG_SERIAL.print(MSG);\
    printDebugFrame(FRAME);\
    DEBUG_SERIAL.println("");\
    DEBUG_SERIAL.flush();\
})

#else

#define D(x)
#define DEBUG_BEGIN()

#define INFO_MSG(MSG)
#define INFO_MSG_VAL(MSG, VAL)
#define INFO_MSG_VAL_FMT(MSG, VAL, FMT)
#define INFO_MSG_FRAME(MSG, FRAME)

#define ERROR_MSG(MSG)
#define ERROR_MSG_VAL(MSG, VAL)
#define ERROR_MSG_VAL_FMT(MSG, VAL, FMT)
#define ERROR_MSG_FRAME(MSG, FRAME)

#endif  // DEBUG_ENABLE

#endif  // __R51_DEBUG_H__
