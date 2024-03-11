#pragma once

#include <cstdint>

namespace Pinetime {
  namespace System {
    enum class BootErrors : uint8_t {
      None,
      TouchController
    };
  }
}
