#pragma once

#include "PageIndicator.h"
#include <lvgl/lvgl.h>

namespace Pinetime {
  namespace Applications {
    namespace Widgets {
      class LineIndicator : public PageIndicator {
      public:
        LineIndicator(uint8_t screens);

        LineIndicator() {
        }

        void Load(uint8_t currentScreen) override;
        void UnLoad() override;

        void SetScreens(uint8_t screens) override;
      
      private:
        lv_obj_t *lineIndicator = NULL, *lineIndicatorBase = NULL;
      };
    }
  }
}
