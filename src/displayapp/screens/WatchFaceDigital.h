#pragma once

#include "Screen.h"
#include "components/ble/SimpleWeatherService.h"
#include "displayapp/widgets/StatusIcons.h"
#include "utility/DirtyValue.h"
#include "components/fs/FS.h"
#include <chrono>

namespace Pinetime {
  namespace Applications {
    namespace Screens {

      class WatchFaceDigital : public Screen {
      public:
        WatchFaceDigital();
        ~WatchFaceDigital() override;

        void Load() override;
        bool UnLoad() override;
        void Refresh() override;
        bool OnTouchEvent(uint16_t /*x*/, uint16_t /*y*/) override;

      private:
        uint8_t displayedHour = -1, displayedMinute = -1;
        Utility::DirtyValue<std::chrono::time_point<std::chrono::system_clock, std::chrono::minutes>> currentDateTime;
        Utility::DirtyValue<uint32_t> stepCount;
        Utility::DirtyValue<uint8_t> heartbeat;
        Utility::DirtyValue<bool> heartbeatRunning;
        Utility::DirtyValue<bool> notificationState;
        Utility::DirtyValue<std::optional<Controllers::SimpleWeatherService::CurrentWeather>> currentWeather;
        Utility::DirtyValue<std::chrono::time_point<std::chrono::system_clock, std::chrono::days>> currentDate;

        lv_obj_t* label_time;
#ifndef TimeFormat_24H
        lv_obj_t* label_time_ampm;
#endif
        lv_obj_t* label_date;
        lv_obj_t* heartbeatIcon;
        lv_obj_t* heartbeatValue;
        lv_obj_t* stepIcon;
        lv_obj_t* stepValue;
        lv_obj_t* notificationIcon;
        lv_obj_t* weatherIcon;
        lv_obj_t* temperature;
        Widgets::StatusIcons statusIcons;
      };
    }

    template <>
    struct WatchFaceTraits<WatchFace::Digital> {
      static constexpr WatchFace watchFace = WatchFace::Digital;
      static constexpr const char* name = "Digital face";

      static Screens::Screen* Create() {
        return new Screens::WatchFaceDigital();
      };

      static bool IsAvailable(Pinetime::Controllers::FS& /*filesystem*/) {
        return true;
      }
    };
  }
}
