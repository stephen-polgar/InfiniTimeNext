#pragma once

#include "displayapp/apps/Apps.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class Screen;
    }

    struct AppDescription {
      Apps app;
      const char* icon;
      Screens::Screen* (*create)();
    };

    struct WatchFaceDescription {
      WatchFace watchFace;
      const char* name;
      Screens::Screen* (*create)();
      bool (*isAvailable)(Controllers::FS& fileSystem);
    };

    template <Apps t>
    consteval AppDescription CreateAppDescription() {
      return {AppTraits<t>::app, AppTraits<t>::icon, &AppTraits<t>::Create};
    }

    template <WatchFace t>
    consteval WatchFaceDescription CreateWatchFaceDescription() {
      return {WatchFaceTraits<t>::watchFace, WatchFaceTraits<t>::name, &WatchFaceTraits<t>::Create, &WatchFaceTraits<t>::IsAvailable};
    }

    template <template <Apps...> typename T, Apps... ts>
    consteval std::array<AppDescription, sizeof...(ts)> CreateAppDescriptions(T<ts...>) {
      return {CreateAppDescription<ts>()...};
    }

    template <template <WatchFace...> typename T, WatchFace... ts>
    consteval std::array<WatchFaceDescription, sizeof...(ts)> CreateWatchFaceDescriptions(T<ts...>) {
      return {CreateWatchFaceDescription<ts>()...};
    }

    constexpr auto userApps = CreateAppDescriptions(UserAppTypes {});
    constexpr auto userWatchFaces = CreateWatchFaceDescriptions(UserWatchFaceTypes {});
  }
}
