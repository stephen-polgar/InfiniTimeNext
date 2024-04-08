#pragma once
#include <lvgl/lvgl.h>

namespace Pinetime {
  namespace Applications {
    namespace Widgets {
      class PageIndicator {
      public:
        PageIndicator(const uint8_t nScreens);
        void Create(uint8_t nCurrentScreen);
        void Clean();
        const uint8_t nScreens;

      private:
        lv_obj_t* pageIndicatorBase;
      };
    }
  }
}
