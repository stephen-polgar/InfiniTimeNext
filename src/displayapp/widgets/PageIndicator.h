#pragma once
#include <lvgl/lvgl.h>

namespace Pinetime {
  namespace Applications {
    namespace Widgets {
      class PageIndicator {
      public:        
        void Create(uint8_t nCurrentScreen);
        void Clean();
        uint8_t nScreens = 0;

      private:
        lv_obj_t* pageIndicatorBase;
      };
    }
  }
}
