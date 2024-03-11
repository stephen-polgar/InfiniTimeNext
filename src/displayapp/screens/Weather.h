#pragma once

#include "Screen.h"
#include "components/ble/SimpleWeatherService.h"
#include "Symbols.h"
#include "utility/DirtyValue.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {

      class Weather : public Screen {
      public:
        Weather();
        ~Weather() override;
        void Load() override;
        bool UnLoad() override;

      private:
        void Refresh() override;
        lv_color_t temperatureColor(int16_t temperature);
        uint8_t temperatureStyle(int16_t temperature);
        int16_t roundTemperature(int16_t temp);

        Utility::DirtyValue<std::optional<Controllers::SimpleWeatherService::CurrentWeather>> currentWeather {};
        Utility::DirtyValue<std::optional<Controllers::SimpleWeatherService::Forecast>> currentForecast {};

        lv_obj_t* icon;
        lv_obj_t* condition;
        lv_obj_t* temperature;
        lv_obj_t* minTemperature;
        lv_obj_t* maxTemperature;
        lv_obj_t* forecast;

        lv_task_t* taskRefresh;
      };
    }

    template <>
    struct AppTraits<Apps::Weather> {
      static constexpr Apps app = Apps::Weather;
      static constexpr const char* icon = Screens::Symbols::cloudSunRain;

      static Screens::Screen* Create() {
        return new Screens::Weather();
      };
    };
  }
}
