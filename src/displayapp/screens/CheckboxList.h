#pragma once

#include "ScreenTree.h"
#include <string>
#include <vector>
#include <functional>

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class CheckboxList : public ScreenTree {
      public:
        struct Item {
          std::string name;
          bool enabled;
          uint8_t index;
          lv_obj_t* cb;
        };

        CheckboxList(const uint8_t maxSize,
                     const char* optionsTitle,
                     const char* optionsSymbol,
                     std::function<uint8_t()> currentValue,
                     std::function<void(uint8_t)> OnValueChanged,
                     lv_layout_t layout = LV_LAYOUT_COLUMN_LEFT,
                     Widgets::PageIndicator* pageIndicator = NULL);
        ~CheckboxList() override;
        void Load() override;
        bool UnLoad() override;

        bool Add(Item);

      private:
        const uint8_t maxSize;
        std::function<void(uint8_t)> OnValueChanged;
        std::vector<Item> options;
        std::function<uint8_t()> currentValue;
        lv_layout_t layout;

        const char* optionsTitle;
        const char* optionsSymbol;
        void updateSelected(lv_obj_t* object);
        static void event_handler(lv_obj_t* obj, lv_event_t event);
      };
    }
  }
}
