#pragma once

#include "displayapp/screens/ScreenList.h"

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

      private:
        bool loaded = false;
        ScreenList<2> screens;
        std::unique_ptr<Screen> screenSetDate();
        std::unique_ptr<Screen> screenSetTime();       
      };
    }
  }
}
