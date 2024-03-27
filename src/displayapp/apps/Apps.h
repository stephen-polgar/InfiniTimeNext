#pragma once

#include "UsedAppsConfig.h"
#include <cstddef>
#include <cstdint>

namespace Pinetime {
  namespace Applications {
    enum class Apps : uint8_t {
      None,
      Launcher,
      Clock,
      SysInfo,
      FirmwareUpdate,
      FirmwareValidation,
      NotificationsPreview,
      Notifications,
      Timer,
      TimerSet,
      Alarm,
      AlarmSet,
      FlashLight,
      BatteryInfo,
      Music,
      HeartRate,
      Navigation,
      StopWatch,
#ifdef UseMotion
      Motion,
#endif
#ifdef UseCalculator
      Calculator,
#endif
#ifdef UseCalendar
      Calendar,
#endif
#ifdef UseGallery
      Gallery,
      FileView,
#endif
      Steps,
      PassKey,
      QuickSettings,
      Settings,
      SettingWatchFace,
      SettingTimeFormat,
      SettingWeatherFormat,
      SettingDisplay,
      SettingWakeUp,
      SettingSteps,
      SettingSetDateTime,
      SettingShakeThreshold,
      SettingBluetooth,
      Error,
      Weather
    };

    enum class WatchFace : uint8_t {
      Digital,
      Analog,
#ifdef UseWatchFaceTerminal
      Terminal,
#endif
      CasioStyleG7710
    };

    template <Apps>
    struct AppTraits {};

    template <WatchFace>
    struct WatchFaceTraits {};

    template <Apps... As>
    struct TypeList {
      static constexpr uint8_t Count = sizeof...(As);
    };

// Change the order if you want different order in application list
    using UserAppTypes = TypeList<Apps::StopWatch,
                                  Apps::Alarm,
                                  Apps::Timer,
                                  Apps::Steps,
                                  Apps::HeartRate,
                                  Apps::Weather,
                                  Apps::Music,
                                  Apps::Navigation
#ifdef UseCalendar
                                  ,
                                  Apps::Calendar
#endif
#ifdef UseCalculator
                                  ,
                                  Apps::Calculator
#endif
#ifdef UseMotion
                                  ,
                                  Apps::Motion
#endif
#ifdef UseGallery
                                  ,
                                  Apps::Gallery
#endif
                                  >;

    template <WatchFace... Ws>
    struct WatchFaceTypeList {
      static constexpr uint8_t Count = sizeof...(Ws);
    };

    using UserWatchFaceTypes = WatchFaceTypeList<WatchFace::Digital,
                                                 WatchFace::Analog,
#ifdef UseWatchFaceTerminal
                                                 WatchFace::Terminal,
#endif
                                                 WatchFace::CasioStyleG7710>;

    static_assert(UserWatchFaceTypes::Count >= 1);
  }
}
