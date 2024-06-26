#pragma once

#include "Screen.h"
#include "BatteryIcon.h"
#include "components/fs/FS.h"
#include "utility/DirtyValue.h"
#include <chrono>

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class WatchFaceCasioStyleG7710 : public Screen {
      public:
        WatchFaceCasioStyleG7710();
        ~WatchFaceCasioStyleG7710() override;
        void Load() override;
        bool UnLoad() override;
        void Refresh() override;
        static bool IsAvailable(Controllers::FS& filesystem);

      private:
        Utility::DirtyValue<uint8_t> batteryPercentRemaining;
        Utility::DirtyValue<bool> powerPresent;
        Utility::DirtyValue<bool> bleState;
        Utility::DirtyValue<bool> bleRadioEnabled;
        Utility::DirtyValue<std::chrono::time_point<std::chrono::system_clock, std::chrono::minutes>> currentDateTime;
        Utility::DirtyValue<uint32_t> stepCount;
        Utility::DirtyValue<uint8_t> heartbeat;
        Utility::DirtyValue<bool> heartbeatRunning;
        Utility::DirtyValue<bool> notificationState;
        Utility::DirtyValue<std::chrono::time_point<std::chrono::system_clock, std::chrono::days>> currentDate;

        static constexpr lv_color_t color_text = LV_COLOR_CYAN;
        static constexpr lv_color_t color_line = LV_COLOR_MAGENTA;

        lv_style_t style_line;
        lv_style_t style_border;

        lv_obj_t* label_time;
#ifndef TimeFormat_24H
        lv_obj_t* label_time_ampm;
#endif
        lv_obj_t* label_date;
        lv_obj_t* label_day_of_week;
        lv_obj_t* label_week_number;
        lv_obj_t* line_day_of_week_number;
        lv_obj_t* label_day_of_year;
        lv_obj_t* line_day_of_year;
        lv_obj_t* bleIcon;
        lv_obj_t* batteryPlug;
        lv_obj_t* label_battery_value;
        lv_obj_t* heartbeatIcon;
        lv_obj_t* heartbeatValue;
        lv_obj_t* stepIcon;
        lv_obj_t* stepValue;
        lv_obj_t* notificationIcon;
        BatteryIcon batteryIcon;
        lv_font_t* font_dot40;
        lv_font_t* font_segment40;
        lv_font_t* font_segment115;
      };
    }

    template <>
    struct WatchFaceTraits<WatchFace::CasioStyleG7710> {
      static constexpr WatchFace watchFace = WatchFace::CasioStyleG7710;
      static constexpr const char* name = "Casio G7710";

      static Screens::Screen* Create() {
        return new Screens::WatchFaceCasioStyleG7710();
      };

      static bool IsAvailable(Controllers::FS& filesystem) {
        return Screens::WatchFaceCasioStyleG7710::IsAvailable(filesystem);
      }
    };
  }
}
