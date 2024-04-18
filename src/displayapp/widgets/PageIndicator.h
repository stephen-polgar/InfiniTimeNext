#pragma once

#include <cstdint>

namespace Pinetime {
  namespace Applications {
    namespace Widgets {
      class PageIndicator {
      public:
        explicit PageIndicator(uint8_t screens) : screens {screens} {
        }

        virtual void Load(uint8_t currentScreen) {
        }

        virtual void UnLoad() {
        }

      protected:
        const uint8_t screens;
      };
    }
  }
}
