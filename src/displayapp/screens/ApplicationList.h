#pragma once

#include "ScreenList.h"
#include "Tile.h"
#include <array>
#include <memory>

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
        auto CreateScreenList() const;
        std::unique_ptr<Screen> CreateScreen(unsigned int screenNum) const;

        std::array<Tile::Applications, UserAppTypes::Count> apps;

        static constexpr int appsPerScreen = 6;

        static constexpr int nScreens = UserAppTypes::Count > 0 ? (UserAppTypes::Count - 1) / appsPerScreen + 1 : 1;

        ScreenList<nScreens> screens;
      };
    }
  }
}
