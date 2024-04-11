#pragma once

#include "ScreenTree.h"
#include "Tile.h"
#include <array>

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class ApplicationList : public Screen {
      public:
        explicit ApplicationList(std::array<Tile::Applications, UserAppTypes::Count>& apps);
        ~ApplicationList() override;
        void Load() override;
        bool UnLoad() override;
        bool OnTouchEvent(TouchEvents event) override;

      private:
        Widgets::PageIndicator pageIndicator;        
        ScreenTree* screens;
      };
    }
  }
}
