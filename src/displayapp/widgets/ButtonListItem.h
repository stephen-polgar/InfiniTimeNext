#pragma once

#include <lvgl/lvgl.h>
#include <functional>

namespace Pinetime {
  namespace Applications {
    namespace Widgets {
      class ButtonListItem {
      public:
        ButtonListItem(std::function<void(bool, void*)> on_checked,
                       std::function<void(void*)> on_open,
                       std::function<void(void*)> on_removed,
                       std::function<void()> on_addNew);
        void CreateNewItem(lv_obj_t* label, bool checked, void* data);
        void CreateButtonNew();
        void EnableAddButton(bool enable);
        void Unload();

      private:
        std::function<void(bool, void*)> on_checked;
        std::function<void(void*)> on_open;
        std::function<void(void*)> on_removed;
        std::function<void()> on_addNew;
        lv_obj_t *last = NULL, *btnNew;
      };
    }
  }
}
