#pragma once

#include "displayapp/screens/ScreenTree.h"
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
        Widgets::DotIndicator dotIndicator;

      private:      
        ScreenTree* screens;
      };
    }
  }
}
