#pragma once

#include "Screen.h"
#include "displayapp/widgets/PageIndicator.h"
#include <array>
#include <functional>

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class CheckboxList : public Screen {
      public:
        static constexpr size_t MaxItems = 4;

        struct Item {
          const char* name;
          bool enabled;
        };

        CheckboxList(const uint8_t screenID,
                     const uint8_t numScreens,
                     const char* optionsTitle,
                     const char* optionsSymbol,
                     uint32_t originalValue,
                     std::function<void(uint32_t)> OnValueChanged,
                     std::array<Item, MaxItems> options);
        ~CheckboxList() override;
        void Load() override;
        bool UnLoad() override;

      private:
        const uint8_t screenID;
        std::function<void(uint32_t)> OnValueChanged;
        std::array<Item, MaxItems> options;
        std::array<lv_obj_t*, MaxItems> cbOption;
        uint32_t value;

        const uint8_t numScreens;
        const char* optionsTitle;
        const char* optionsSymbol;
        uint32_t originalValue;

        Widgets::PageIndicator pageIndicator;
        void updateSelected(lv_obj_t* object, lv_event_t event);
        static void event_handler(lv_obj_t* obj, lv_event_t event);
      };
    }
  }
}
