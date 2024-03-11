#pragma once

#include "displayapp/screens/Screen.h"
#include "components/settings/Settings.h"
#include <array>

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class SettingWakeUp : public Screen {
      public:
        SettingWakeUp();
        ~SettingWakeUp() override;
        void Load() override;
        bool UnLoad() override;

      private:
        void updateSelected(lv_obj_t* object);
        static void event_handler(lv_obj_t* obj, lv_event_t event);       

        struct Option {
          Controllers::Settings::WakeUpMode wakeUpMode;
          const char* name;
        };

        static constexpr std::array<Option, 5> options = {{
          {Controllers::Settings::WakeUpMode::SingleTap, "Single Tap"},
          {Controllers::Settings::WakeUpMode::DoubleTap, "Double Tap"},
          {Controllers::Settings::WakeUpMode::RaiseWrist, "Raise Wrist"},
          {Controllers::Settings::WakeUpMode::Shake, "Shake Wake"},
          {Controllers::Settings::WakeUpMode::LowerWrist, "Lower Wrist"},
        }};

        lv_obj_t* cbOption[options.size()];
      };
    }
  }
}
