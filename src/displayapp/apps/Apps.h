#pragma once
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
      Motion,
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
      Terminal,     
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

    using UserAppTypes = TypeList<Apps::StopWatch, Apps::Alarm, Apps::Timer, Apps::Steps, Apps::HeartRate, Apps::Weather, Apps::Music, Apps::Motion, Apps::Navigation>;

    template <WatchFace... Ws>
    struct WatchFaceTypeList {
      static constexpr uint8_t Count = sizeof...(Ws);
    };

    using UserWatchFaceTypes = WatchFaceTypeList<WatchFace::Digital, WatchFace::Analog, WatchFace::Terminal, WatchFace::CasioStyleG7710>;

    static_assert(UserWatchFaceTypes::Count >= 1);
  }
}
