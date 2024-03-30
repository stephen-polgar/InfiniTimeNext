#pragma once

// All watch faces must be included here
#include "displayapp/screens/WatchFaceDigital.h"
#include "displayapp/screens/WatchFaceAnalog.h"
#include "displayapp/screens/WatchFaceCasioStyleG7710.h"
#include "displayapp/screens/WatchFaceTerminal.h"

namespace Pinetime {
  namespace Applications {

    struct WatchFaceDescription {
      WatchFace watchFace;
      const char* name;
      Screens::Screen* (*create)();
      bool (*isAvailable)(Controllers::FS& fileSystem);
    };

    template <WatchFace t>
    consteval WatchFaceDescription CreateWatchFaceDescription() {
      return {WatchFaceTraits<t>::watchFace, WatchFaceTraits<t>::name, &WatchFaceTraits<t>::Create, &WatchFaceTraits<t>::IsAvailable};
    }

    template <template <WatchFace...> typename T, WatchFace... ts>
    consteval std::array<WatchFaceDescription, sizeof...(ts)> CreateWatchFaceDescriptions(T<ts...>) {
      return {CreateWatchFaceDescription<ts>()...};
    }

    constexpr auto userWatchFaces = CreateWatchFaceDescriptions(UserWatchFaceTypes {});
  }
}

// All user apps must be included here
#include "displayapp/screens/HeartRate.h"
#include "displayapp/screens/Motion.h"
#include "displayapp/screens/Alarm.h"
#include "displayapp/screens/Timer.h"
#include "displayapp/screens/StopWatch.h"
#include "displayapp/screens/Music.h"
#include "displayapp/screens/Navigation.h"
#include "displayapp/screens/Steps.h"
#include "displayapp/screens/Weather.h"
#include "displayapp/screens/Calendar.h"
#include "displayapp/screens/Calculator.h"
#include "displayapp/screens/Gallery.h"


namespace Pinetime {
  namespace Applications {

    struct AppDescription {
      Apps app;
      const char* icon;
      Screens::Screen* (*create)();
    };

    template <Apps t>
    consteval AppDescription CreateAppDescription() {
      return {AppTraits<t>::app, AppTraits<t>::icon, &AppTraits<t>::Create};
    }

    template <template <Apps...> typename T, Apps... ts>
    consteval std::array<AppDescription, sizeof...(ts)> CreateAppDescriptions(T<ts...>) {
      return {CreateAppDescription<ts>()...};
    }

    constexpr auto userApps = CreateAppDescriptions(UserAppTypes {});
  }
}
