#pragma once

#include "displayapp/screens/Screen.h"
#ifdef UseWatchFacePineTimeStyle
  #include "components/fs/FS.h"
  #include <chrono>
  #include "displayapp/screens/BatteryIcon.h"
  #include "displayapp/Colors.h"
  #include "components/datetime/DateTimeController.h"
  #include "components/ble/SimpleWeatherService.h"
  #include "utility/DirtyValue.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class WatchFacePineTimeStyle : public Screen {
      public:
        WatchFacePineTimeStyle();
        ~WatchFacePineTimeStyle() override;
        void Load() override;
        bool UnLoad() override;
        bool OnTouchEvent(TouchEvents event) override;
        bool OnButtonPushed() override;
        void Refresh() override;

      private:
        uint8_t displayedHour, displayedMinute, displayedSecond;
        uint16_t currentYear;
        Controllers::DateTime::Months currentMonth;
        Controllers::DateTime::Days currentDayOfWeek;
        uint8_t currentDay;
        uint32_t savedTick;

        Utility::DirtyValue<uint8_t> batteryPercentRemaining;
        Utility::DirtyValue<bool> isCharging;
        Utility::DirtyValue<bool> bleState;
        Utility::DirtyValue<bool> bleRadioEnabled;
        Utility::DirtyValue<std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>> currentDateTime;
        Utility::DirtyValue<uint32_t> stepCount;
        Utility::DirtyValue<bool> notificationState;
        Utility::DirtyValue<std::optional<Controllers::SimpleWeatherService::CurrentWeather>> currentWeather;

        static Controllers::Settings::Colors GetNext(Controllers::Settings::Colors color);
        static Controllers::Settings::Colors GetPrevious(Controllers::Settings::Colors color);

        lv_obj_t* btnNextTime;
        lv_obj_t* btnPrevTime;
        lv_obj_t* btnNextBar;
        lv_obj_t* btnPrevBar;
        lv_obj_t* btnNextBG;
        lv_obj_t* btnPrevBG;
        lv_obj_t* btnReset;
        lv_obj_t* btnRandom;
        lv_obj_t* btnClose;
        lv_obj_t* btnSteps;
        lv_obj_t* btnWeather;
        lv_obj_t* timebar;
        lv_obj_t* sidebar;
        lv_obj_t* timeDD1;
        lv_obj_t* timeDD2;
        lv_obj_t* timeDD3;
        lv_obj_t* timeAMPM;
        lv_obj_t* dateDayOfWeek;
        lv_obj_t* dateDay;
        lv_obj_t* dateMonth;
        lv_obj_t* weatherIcon;
        lv_obj_t* temperature;
        lv_obj_t* plugIcon;
        lv_obj_t* bleIcon;
        lv_obj_t* calendarOuter;
        lv_obj_t* calendarInner;
        lv_obj_t* calendarBar1;
        lv_obj_t* calendarBar2;
        lv_obj_t* calendarCrossBar1;
        lv_obj_t* calendarCrossBar2;
        lv_obj_t* notificationIcon;
        lv_obj_t* stepGauge;
        lv_obj_t* btnSetColor;
        lv_obj_t* btnSetOpts;
        lv_obj_t* stepIcon;
        lv_obj_t* stepValue;
        lv_color_t needle_colors[1];

        BatteryIcon batteryIcon {false};

        Controllers::Settings* settingsController;

        void updateSelected(lv_obj_t* object);
        static void event_handler(lv_obj_t* obj, lv_event_t event);

        void SetBatteryIcon();
        void CloseMenu();
      };
    }

    template <>
    struct WatchFaceTraits<WatchFace::PineTimeStyle> {
      static constexpr WatchFace watchFace = WatchFace::PineTimeStyle;
      static constexpr const char* name = "PineTimeStyle";

      static Screens::Screen* Create() {
        return new Screens::WatchFacePineTimeStyle();
      };

      static bool IsAvailable(Controllers::FS& /*filesystem*/) {
        return true;
      }
    };
  }
}
#endif