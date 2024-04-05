#pragma once

#include "displayapp/apps/Apps.h"
#include "components/fs/FS.h"
#include "components/brightness/BrightnessController.h"
#include <bitset>

namespace Pinetime {
  namespace Controllers {
    class Settings {
    public:
      enum class ClockType : uint8_t { H24, H12 };
      enum class WeatherFormat : uint8_t { Metric, Imperial };
      enum class Notification : uint8_t { On, Off, Sleep };
      enum class WakeUpMode : uint8_t { SingleTap = 0, DoubleTap = 1, RaiseWrist = 2, Shake = 3, LowerWrist = 4 };
      enum class Colors : uint8_t {
        White,
        Silver,
        Gray,
        Black,
        Red,
        Maroon,
        Yellow,
        Olive,
        Lime,
        Green,
        Cyan,
        Teal,
        Blue,
        Navy,
        Magenta,
        Purple,
        Orange,
        Pink
      };

      Settings(Controllers::FS& fs);

#ifdef UseWatchFaceInfineat
      struct WatchFaceInfineat {
        bool showSideCover = true;
        int8_t colorIndex = 0;
      };

      void SetInfineatShowSideCover(bool show) {
        if (show != settings.watchFaceInfineat.showSideCover) {
          settings.watchFaceInfineat.showSideCover = show;
          settingsChanged = true;
        }
      };

      bool GetInfineatShowSideCover() const {
        return settings.watchFaceInfineat.showSideCover;
      };

      void SetInfineatColorIndex(int8_t index) {
        if (index != settings.watchFaceInfineat.colorIndex) {
          settings.watchFaceInfineat.colorIndex = index;
          settingsChanged = true;
        }
      };

      int8_t GetInfineatColorIndex() const {
        return settings.watchFaceInfineat.colorIndex;
      };
#endif
#ifdef UseWatchFacePineTimeStyle
      enum class PTSGaugeStyle : uint8_t { Full, Half, Numeric };
      enum class PTSWeather : uint8_t { On, Off };

      struct PineTimeStyle {
        Colors ColorTime = Colors::Teal;
        Colors ColorBar = Colors::Teal;
        Colors ColorBG = Colors::Black;
        PTSGaugeStyle gaugeStyle = PTSGaugeStyle::Full;
        PTSWeather weatherEnable = PTSWeather::Off;
      };

      void SetPTSColorTime(Colors colorTime) {
        if (colorTime != settings.PTS.ColorTime)
          settingsChanged = true;
        settings.PTS.ColorTime = colorTime;
      };

      Colors GetPTSColorTime() const {
        return settings.PTS.ColorTime;
      };

      void SetPTSColorBar(Colors colorBar) {
        if (colorBar != settings.PTS.ColorBar)
          settingsChanged = true;
        settings.PTS.ColorBar = colorBar;
      };

      Colors GetPTSColorBar() const {
        return settings.PTS.ColorBar;
      };

      void SetPTSColorBG(Colors colorBG) {
        if (colorBG != settings.PTS.ColorBG)
          settingsChanged = true;
        settings.PTS.ColorBG = colorBG;
      };

      Colors GetPTSColorBG() const {
        return settings.PTS.ColorBG;
      };

      void SetPTSGaugeStyle(PTSGaugeStyle gaugeStyle) {
        if (gaugeStyle != settings.PTS.gaugeStyle)
          settingsChanged = true;
        settings.PTS.gaugeStyle = gaugeStyle;
      };

      PTSGaugeStyle GetPTSGaugeStyle() const {
        return settings.PTS.gaugeStyle;
      };

      void SetPTSWeather(PTSWeather weatherEnable) {
        if (weatherEnable != settings.PTS.weatherEnable)
          settingsChanged = true;
        settings.PTS.weatherEnable = weatherEnable;
      };

      PTSWeather GetPTSWeather() const {
        return settings.PTS.weatherEnable;
      };
#endif

      Settings(const Settings&) = delete;
      Settings& operator=(const Settings&) = delete;
      Settings(Settings&&) = delete;
      Settings& operator=(Settings&&) = delete;

      void Init();
      void SaveSettings();

      void SetWatchFace(Applications::WatchFace face) {
        if (face != settings.watchFace) {
          settingsChanged = true;
        }
        settings.watchFace = face;
      };

      Applications::WatchFace GetWatchFace() const {
        return settings.watchFace;
      };

      void SetClockType(ClockType clocktype) {
        if (clocktype != settings.clockType) {
          settingsChanged = true;
        }
        settings.clockType = clocktype;
      };

      ClockType GetClockType() const {
        return settings.clockType;
      };

      void SetWeatherFormat(WeatherFormat weatherFormat) {
        if (weatherFormat != settings.weatherFormat) {
          settingsChanged = true;
        }
        settings.weatherFormat = weatherFormat;
      };

      WeatherFormat GetWeatherFormat() const {
        return settings.weatherFormat;
      };

      void SetNotificationStatus(Notification status) {
        if (status != settings.notificationStatus) {
          settingsChanged = true;
        }
        settings.notificationStatus = status;
      };

      Notification GetNotificationStatus() const {
        return settings.notificationStatus;
      };

      void SetScreenTimeOut(uint32_t timeout) {
        if (timeout != settings.screenTimeOut) {
          settingsChanged = true;
        }
        settings.screenTimeOut = timeout;
      };

      uint32_t GetScreenTimeOut() const {
        return settings.screenTimeOut;
      };

      void SetShakeThreshold(uint16_t thresh) {
        if (settings.shakeWakeThreshold != thresh) {
          settings.shakeWakeThreshold = thresh;
          settingsChanged = true;
        }
      }

      int16_t GetShakeThreshold() const {
        return settings.shakeWakeThreshold;
      }

      void setWakeUpMode(WakeUpMode wakeUp, bool enabled) {
        if (enabled != isWakeUpModeOn(wakeUp)) {
          settingsChanged = true;
        }
        settings.wakeUpMode.set(static_cast<size_t>(wakeUp), enabled);
        // Handle special behavior
        if (enabled) {
          switch (wakeUp) {
            case WakeUpMode::SingleTap:
              settings.wakeUpMode.set(static_cast<size_t>(WakeUpMode::DoubleTap), false);
              break;
            case WakeUpMode::DoubleTap:
              settings.wakeUpMode.set(static_cast<size_t>(WakeUpMode::SingleTap), false);
              break;
            default:
              break;
          }
        }
      };

      std::bitset<5> getWakeUpModes() const {
        return settings.wakeUpMode;
      }

      bool isWakeUpModeOn(const WakeUpMode mode) const {
        return getWakeUpModes()[static_cast<size_t>(mode)];
      }

      void SetBrightness(BrightnessController::Levels level) {
        if (level != settings.brightLevel) {
          settingsChanged = true;
        }
        settings.brightLevel = level;
      };

      BrightnessController::Levels GetBrightness() const {
        return settings.brightLevel;
      };

      void SetStepsGoal(uint32_t goal) {
        if (goal != settings.stepsGoal) {
          settingsChanged = true;
        }
        settings.stepsGoal = goal;
      };

      uint32_t GetStepLength() const {
        return settings.stepLength;
      };

      void SetStepLength(uint32_t stepLength) {
        if (stepLength != settings.stepLength) {
          settingsChanged = true;
        }
        settings.stepLength = stepLength;
      };

      uint32_t GetStepsGoal() const {
        return settings.stepsGoal;
      };

      void SetBleRadioEnabled(bool enabled) {
        bleRadioEnabled = enabled;
      };

      bool GetBleRadioEnabled() const {
        return bleRadioEnabled;
      };

    private:
      Controllers::FS& fs;
      static constexpr uint32_t settingsVersion = 0x0001;

      struct SettingsData {
        uint32_t version = settingsVersion;
        uint32_t stepsGoal = 10000, stepLength = 40;
        uint32_t screenTimeOut = 15000;

        ClockType clockType = ClockType::H24;
        WeatherFormat weatherFormat = WeatherFormat::Metric;
        Notification notificationStatus = Notification::On;

        Applications::WatchFace watchFace = Applications::WatchFace::Digital;
        std::bitset<5> wakeUpMode {0};
        uint16_t shakeWakeThreshold = 150;

        Controllers::BrightnessController::Levels brightLevel = Controllers::BrightnessController::Levels::Medium;
#ifdef UseWatchFaceInfineat
        WatchFaceInfineat watchFaceInfineat;
#endif
#ifdef UseWatchFacePineTimeStyle
        PineTimeStyle PTS;
#endif
      };

      SettingsData settings;
      bool settingsChanged = false;

      /* ble state is intentionally not saved with the other watch settings and initialized
       * to off (false) on every boot because we always want ble to be enabled on startup
       */
      bool bleRadioEnabled = true;

      static constexpr const char* file = "/settings.dat";
    };
  }
}
