#pragma once

#include "displayapp/screens/Screen.h"
#include "displayapp/widgets/LineIndicator.h"
#include "displayapp/screens/Symbols.h"
#include "utility/ArrayTouchHandler.h"
#include <array>

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class Settings : public Screen {
      public:
        Settings();
        ~Settings() override;
        void Load() override;
        bool UnLoad() override;
        bool OnTouchEvent(Applications::TouchEvents event) override;

      private:
        struct AppList {
          const char* icon;
          const char* name;
          Pinetime::Applications::Apps app;
        };

        static constexpr std::array<AppList, 11> apps {{{Symbols::sun, "Display", Apps::SettingDisplay},
                                                        {Symbols::bluetooth, "Bluetooth", Apps::SettingBluetooth},
                                                        {Symbols::batteryHalf, "Battery", Apps::BatteryInfo},
                                                        {Symbols::eye, "Wake Up", Apps::SettingWakeUp},
                                                        {Symbols::home, "Watch face", Apps::SettingWatchFace},
                                                        {Symbols::tachometer, "Shake Calib.", Apps::SettingShakeThreshold},
                                                        {Symbols::list, "System Info", Apps::SysInfo},
                                                        {Symbols::check, "Firmware", Apps::FirmwareValidation},
                                                        {Symbols::clock, "Time format", Apps::SettingTimeFormat},
                                                        {Symbols::cloudSunRain, "Weather", Apps::SettingWeatherFormat},
                                                        {Symbols::clock, "Date&Time", Apps::SettingSetDateTime}}};

                                                        
        static constexpr uint8_t maxItems = 4;
        void load(uint8_t indexBegin, uint8_t indexEnd, Screen::FullRefreshDirections direction);
        void unload();
        Utility::ArrayTouchHandler arrayTouchHandler;
        lv_obj_t* container;
        bool loaded = false;
        static constexpr uint8_t innerPad = 4;
        Widgets::LineIndicator pageIndicator;
        void onButtonEvent(lv_obj_t* obj);
        static void buttonEventHandler(lv_obj_t* obj, lv_event_t event);
      };
    }
  }
}
