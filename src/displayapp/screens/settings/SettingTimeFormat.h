#pragma once

#include "displayapp/screens/Screen.h"
#include "displayapp/screens/CheckboxList.h"
#include "components/settings/Settings.h"

namespace Pinetime {

  namespace Applications {
    namespace Screens {

      class SettingTimeFormat : public Screen {
      public:
        SettingTimeFormat();
        ~SettingTimeFormat() override;
        void Load() override;
        bool UnLoad() override;

      private:
         Widgets::PageIndicator pageIndicator;
        CheckboxList checkboxList;

        struct Option {
          Controllers::Settings::ClockType clockType;
          const char* name;
        };

        static constexpr std::array<Option, 2> options = {{
          {Controllers::Settings::ClockType::H12, "12-hour"},
          {Controllers::Settings::ClockType::H24, "24-hour"},
        }};

        std::array<CheckboxList::Item, CheckboxList::MaxItems> CreateOptionArray();
        uint8_t GetDefaultOption(Controllers::Settings::ClockType currentOption);
      };
    }
  }
}
