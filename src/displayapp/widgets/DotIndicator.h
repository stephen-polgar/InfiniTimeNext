#pragma once
#include <lvgl/lvgl.h>

namespace Pinetime {
  namespace Applications {
    namespace Widgets {
      class DotIndicator {
      public:       
        void Create(uint8_t nCurrentScreen, uint8_t nScreens);   
      };
    }
  }
}
