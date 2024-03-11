#pragma once

#include "Symbols.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class BleIcon {
      public:
        static const char* GetIcon(bool isConnected);
      };
    }
  }
}
