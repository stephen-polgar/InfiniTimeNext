#pragma once

#include <cstdint>

namespace Pinetime {
  namespace Applications {
    enum class TouchEvents : uint8_t { None, Tap, SwipeLeft, SwipeRight, SwipeUp, SwipeDown, LongTap, DoubleTap };
  }
}