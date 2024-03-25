#pragma once

#include "displayapp/screens/ScreenList.h"
#include "displayapp/widgets/DotIndicator.h"

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
        void Advance();
        void Quit();
        Widgets::DotIndicator dotIndicator;

      private:
        bool loaded = false;
        ScreenList<2> screens;
      };
    }
  }
}
