#pragma once

#include "displayapp/screens/Screen.h"
#include "components/settings/Settings.h"
#include <array>

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class SettingWeatherFormat : public Screen {
      public:
        SettingWeatherFormat();
        ~SettingWeatherFormat() override;
        void Load() override;
        bool UnLoad() override;

      private:
        struct Option {
          Controllers::Settings::WeatherFormat weatherFormat;
          const char* name;
        };

        static constexpr std::array<Option, 2> options = {{
          {Controllers::Settings::WeatherFormat::Metric, "Metric"},
          {Controllers::Settings::WeatherFormat::Imperial, "Imperial"},
        }};

        std::array<Option, 2> opts;
                  
        void onButtonEvent(lv_obj_t* obj);
        static void buttonEventHandler(lv_obj_t* obj, lv_event_t event);
      };
    }
  }
}
