#pragma once

#include <stdint.h>

namespace Pinetime {
  namespace Controllers {
    enum class ButtonActions : uint8_t { None, Click, DoubleClick, LongPress, LongerPress };
  }
}
