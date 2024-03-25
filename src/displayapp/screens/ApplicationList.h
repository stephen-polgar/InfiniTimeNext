#pragma once

#include "ScreenList.h"
#include "Tile.h"
#include <array>

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class ApplicationList : public Screen {
      public:
        explicit ApplicationList(std::array<Tile::Applications, UserAppTypes::Count>&& apps);
        ~ApplicationList() override;
        void Load() override;
        bool UnLoad() override;
        bool OnTouchEvent(TouchEvents event) override;

      private:
        Screen* CreateScreen(uint8_t screenNum);
        std::array<Tile::Applications, UserAppTypes::Count> apps;
        static constexpr uint8_t appsPerScreen = 6;
        static constexpr uint8_t nScreens = UserAppTypes::Count > 0 ? (UserAppTypes::Count - 1) / appsPerScreen + 1 : 1;
        Widgets::PageIndicator pageIndicator;
        ScreenList<nScreens> screens;
      };
    }
  }
}
