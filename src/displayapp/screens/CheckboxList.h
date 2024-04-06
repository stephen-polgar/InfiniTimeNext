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
        static constexpr uint8_t MaxItems = 4;

        struct Item {
          const char* name;
          bool enabled;
        };

        CheckboxList(const uint8_t screenID,
                     const uint8_t numScreens,
                     const char* optionsTitle,
                     const char* optionsSymbol,
                     std::function<uint8_t()> currentValue,
                     std::function<void(uint8_t)> OnValueChanged,
                     std::array<Item, MaxItems> options,
                     Widgets::PageIndicator& pageIndicator);
        ~CheckboxList() override;
        void Load() override;
        bool UnLoad() override;

      private:
        const uint8_t screenID, numScreens;
        std::function<void(uint8_t)> OnValueChanged;
        std::array<Item, MaxItems> options;
        std::array<lv_obj_t*, MaxItems> cbOption;
        std::function<uint8_t()> currentValue;
       
        const char* optionsTitle;
        const char* optionsSymbol;

        Widgets::PageIndicator& pageIndicator;
        void updateSelected(lv_obj_t* object);
        static void event_handler(lv_obj_t* obj, lv_event_t event);
      };
    }
  }
}
