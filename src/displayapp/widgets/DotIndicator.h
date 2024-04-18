#pragma once

#include "PageIndicator.h"
#include <lvgl/lvgl.h>

namespace Pinetime {
  namespace Applications {
    namespace Widgets {
      class DotIndicator : public PageIndicator {
      public:
        DotIndicator(uint8_t screens);
        void Load(uint8_t currentScreen) override;
        void UnLoad() override;

      private:
        lv_obj_t* container;
      };
    }
  }
}
