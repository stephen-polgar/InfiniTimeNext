#pragma once

#include "displayapp/screens/Screen.h"
#include "displayapp/widgets/DotIndicator.h"
#include "utility/ArrayTouchHandler.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class SettingSetDateTime : public Screen {
      public:
        SettingSetDateTime();
        ~SettingSetDateTime() override;

        void Load() override;
        bool UnLoad() override;

        bool OnTouchEvent(TouchEvents event) override;
        void SwipeUp();
        Widgets::DotIndicator dotIndicator;

      private:
        static constexpr uint8_t items = 2;
        Screen* list[items];
        void load(uint8_t indexBegin, uint8_t indexEnd, Screen::FullRefreshDirections direction);
        Utility::ArrayTouchHandler arrayTouchHandler;
      };
    }
  }
}
