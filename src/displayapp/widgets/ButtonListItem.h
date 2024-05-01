#pragma once

#include "displayapp/widgets/StatusIcons.h"
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
        void Load();               
        void CreateNewItem(lv_obj_t* label, bool checked, void* data);
        void CreateButtonNew();
        void EnableAddButton(bool enable);
        void Unload();
        void Refresh();        

      private:
       lv_obj_t* label_time;
        Widgets::StatusIcons statusIcons;
        std::function<void(bool, void*)> on_checked;
        std::function<void(void*)> on_open;
        std::function<void(void*)> on_removed;
        std::function<void()> on_addNew;
        lv_obj_t *last = NULL, *btnNew;
      };
    }
  }
}
