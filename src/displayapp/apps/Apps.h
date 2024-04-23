#pragma once

#include "UsedAppsConfig.h"
#include <cstddef>
#include <cstdint>

namespace Pinetime {
  namespace Applications {
    enum class Apps : uint8_t {
      None,
      Clock,
      Launcher,
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
#ifdef UseFileManager
      FileManager,
      FileView,
#endif
#ifdef UseTwos
      Twos,
#endif
#ifdef UseDice
      Dice,
#endif
#ifdef UseInfiniPaint
      Paint,
#endif
#ifdef UseMetronome
      Metronome,
#endif
#ifdef UsePaddle
      Paddle,
#endif
      Steps,
      PassKey,
      QuickSettings,
      Settings,
      SettingWatchFace,   
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
      Digital = uint8_t(Apps::Weather) + 1, // first elem = last Apps +1 for start with a different ID
      Analog,
#ifdef UseWatchFaceTerminal
      Terminal,
#endif
#ifdef UseWatchFaceInfineat
      Infineat,
#endif
#ifdef UseWatchFacePineTimeStyle
      PineTimeStyle,
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
#ifdef UsePaddle
                                  ,
                                  Apps::Paddle
#endif
#ifdef UseTwos
                                  ,
                                  Apps::Twos
#endif
#ifdef UseInfiniPaint
                                  ,
                                  Apps::Paint
#endif
#ifdef UseDice
                                  ,
                                  Apps::Dice
#endif
#ifdef UseMetronome
                                  ,
                                  Apps::Metronome
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
#ifdef UseWatchFaceInfineat
                                                 WatchFace::Infineat,
#endif
#ifdef UseWatchFacePineTimeStyle
                                                 WatchFace::PineTimeStyle,
#endif
                                                 WatchFace::CasioStyleG7710>;

    static_assert(UserWatchFaceTypes::Count >= 1);
  }
}
