#pragma once

#include <lvgl/lvgl.h>

namespace Pinetime {
  namespace Applications {
    namespace Widgets {
      class Container {
      public:
       void Load(const char* title,  const char* symbol, lv_layout_t layout = LV_LAYOUT_COLUMN_LEFT);
       void UpdateHeight(uint8_t pad_top);  
       lv_obj_t* Container;       
      };
    }
  }
}
