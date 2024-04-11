#pragma once

#include "displayapp/screens/ScreenTree.h"
#include "displayapp/screens/Symbols.h"
#include "displayapp/screens/List.h"

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
        static constexpr std::array<List::Applications, 11> apps {{{Symbols::sun, "Display", Apps::SettingDisplay},
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
        ScreenTree* screens;
        Widgets::PageIndicator pageIndicator;
      };
    }
  }
}
