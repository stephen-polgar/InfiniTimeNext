#pragma once

#include <cstdint>

namespace Pinetime {
  namespace Applications {
    namespace Widgets {
      class PageIndicator {
      public:
        PageIndicator(uint8_t screens) : screens {screens} {
        }

        PageIndicator() {
        }

        virtual void Load(uint8_t currentScreen) {
        }

        virtual void UnLoad() {
        }

        virtual void SetScreens(uint8_t screens) {
          this->screens = screens;
        }

      protected:
        uint8_t screens;
      };
    }
  }
}
