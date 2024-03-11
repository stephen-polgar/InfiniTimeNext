#pragma once

#include "Screen.h"
#include "displayapp/widgets/PageIndicator.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {

      class Label : public Screen {
      public:
        Label(uint8_t screenID, uint8_t numScreens);
        virtual ~Label() override;
        void Load() override;
        bool UnLoad() override;

      private:
        Widgets::PageIndicator pageIndicator;
      };
    }
  }
}
