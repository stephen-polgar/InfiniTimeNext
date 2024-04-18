#pragma once

#include "displayapp/screens/Screen.h"
#include "components/settings/Settings.h"
#include <array>

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
        struct Option {
          Controllers::Settings::ClockType clockType;
          const char* name;
        };

        static constexpr std::array<Option, 2> options = {{
          {Controllers::Settings::ClockType::H12, "12-hour"},
          {Controllers::Settings::ClockType::H24, "24-hour"},
        }};

        void onButtonEvent(lv_obj_t* obj);
        static void buttonEventHandler(lv_obj_t* obj, lv_event_t event);
      };
    }
  }
}
