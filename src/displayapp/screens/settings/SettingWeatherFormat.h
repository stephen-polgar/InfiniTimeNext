#pragma once

#include "displayapp/screens/Screen.h"
#include "displayapp/screens/CheckboxList.h"
#include "components/settings/Settings.h"

namespace Pinetime {

  namespace Applications {
    namespace Screens {

      class SettingWeatherFormat : public Screen {
      public:
        explicit SettingWeatherFormat();
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
       
        uint8_t getCurrentOption(Controllers::Settings::WeatherFormat currentOption);       
        CheckboxList checkboxList;
      };
    }
  }
}
