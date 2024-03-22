#pragma once

#include "Screen.h"
#include "BatteryIcon.h"
#include "utility/DirtyValue.h"
#include "components/fs/FS.h"
#include "components/datetime/DateTimeController.h"
#include <chrono>

namespace Pinetime {
    namespace Applications {
    namespace Screens {
      class WatchFaceAnalog : public Screen {
      public:
        WatchFaceAnalog();
        ~WatchFaceAnalog() override;
       
        void Load() override; 
        bool UnLoad() override; 
        
      private:
        void Refresh() override;
        uint8_t sHour, sMinute, sSecond;

        Utility::DirtyValue<uint8_t> batteryPercentRemaining {0};
        Utility::DirtyValue<bool> isCharging {};
        Utility::DirtyValue<bool> bleState {};
        Utility::DirtyValue<std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>> currentDateTime;
        Utility::DirtyValue<bool> notificationState {false};
        Utility::DirtyValue<std::chrono::time_point<std::chrono::system_clock, std::chrono::days>> currentDate;
     
        lv_obj_t* hour_body;
        lv_obj_t* hour_body_trace;
        lv_obj_t* minute_body;
        lv_obj_t* minute_body_trace;
        lv_obj_t* second_body;

        lv_point_t hour_point[2];
        lv_point_t hour_point_trace[2];
        lv_point_t minute_point[2];
        lv_point_t minute_point_trace[2];
        lv_point_t second_point[2];

        lv_style_t hour_line_style;
        lv_style_t hour_line_style_trace;
        lv_style_t minute_line_style;
        lv_style_t minute_line_style_trace;
        lv_style_t second_line_style;

        lv_obj_t* label_date_day;
        lv_obj_t* plugIcon;
        lv_obj_t* notificationIcon;
        lv_obj_t* bleIcon;

        BatteryIcon batteryIcon;

        void updateDate(Controllers::DateTime *);
        void updateClock();
       
        lv_task_t* taskRefresh;
      };
    }

    template <>
    struct WatchFaceTraits<WatchFace::Analog> {
      static constexpr WatchFace watchFace = WatchFace::Analog;
      static constexpr const char* name = "Analog face";

      static Screens::Screen* Create() {
        return new Screens::WatchFaceAnalog();
      };

      static bool IsAvailable(Controllers::FS& /*filesystem*/) {
        return true;
      }
    };
  }
}
