#pragma once

#include "displayapp/screens/ScreenList.h"
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

        bool OnTouchEvent(Pinetime::Applications::TouchEvents event) override;

      private:
        auto CreateScreenList() const;
        std::unique_ptr<Screen> CreateScreen(unsigned int screenNum) const;

        static constexpr int entriesPerScreen = 4;

        // Increment this when more space is needed
        static constexpr int nScreens = 4;

        static constexpr std::array<List::Applications, entriesPerScreen * nScreens> entries {
          {{Symbols::sun, "Display", Apps::SettingDisplay},
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
        ScreenList<nScreens> screens;
      };
    }
  }
}
