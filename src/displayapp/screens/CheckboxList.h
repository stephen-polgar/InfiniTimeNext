#pragma once

#include "Screen.h"
#include "displayapp/widgets/PageIndicator.h"
#include <string>
#include <vector>
#include <functional>

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class CheckboxList : public Screen {
      public:
        struct Item {
          std::string name;
          bool enabled;
          lv_obj_t* cb;
        };

        CheckboxList(const uint8_t screenID,
                     Widgets::PageIndicator* pageIndicator,
                     const char* optionsTitle,
                     const char* optionsSymbol,
                     std::function<uint8_t(uint8_t screenId)> currentValue,
                     std::function<void(uint8_t screenId, uint8_t index)> OnValueChanged,
                     lv_layout_t layout = LV_LAYOUT_COLUMN_LEFT);

        CheckboxList(const char* optionsTitle,
                     const char* optionsSymbol,
                     std::function<uint8_t(uint8_t screenId)> currentValue,
                     std::function<void(uint8_t screenId, uint8_t index)> OnValueChanged,
                     lv_layout_t layout = LV_LAYOUT_COLUMN_LEFT);
        ~CheckboxList() override;
        void Load() override;
        bool UnLoad() override;

        void Add(Item);

      private:
        uint8_t screenID;
        Widgets::PageIndicator* pageIndicator;
        std::function<void(uint8_t screenId, uint8_t index)> OnValueChanged;
        std::vector<Item> options;
        std::function<uint8_t(uint8_t screenId)> currentValue;
        lv_layout_t layout;

        const char* optionsTitle;
        const char* optionsSymbol;
        void updateSelected(lv_obj_t* object);
        static void event_handler(lv_obj_t* obj, lv_event_t event);
      };
    }
  }
}
