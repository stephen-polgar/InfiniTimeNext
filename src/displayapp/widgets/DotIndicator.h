#pragma once

#include "PageIndicator.h"
#include <lvgl/lvgl.h>

namespace Pinetime {
  namespace Applications {
    namespace Widgets {
      class DotIndicator : public PageIndicator {
      public:
        void Create(uint8_t screens, uint8_t currentScreen) override;
      };
    }
  }
}
