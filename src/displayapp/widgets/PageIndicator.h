#pragma once

#include <cstdint>

namespace Pinetime {
  namespace Applications {
    namespace Widgets {
      class PageIndicator {
      public:       
        virtual void Create(uint8_t screens, uint8_t currentScreen) {
        }
      };
    }
  }
}
