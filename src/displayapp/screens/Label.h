#pragma once

#include "Screen.h"
#include "displayapp/widgets/PageIndicator.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class Label : public Screen {
      public:
        Label(uint8_t screenID, Widgets::PageIndicator* pageIndicator);
        virtual ~Label() override;
        void Load() override;
        bool UnLoad() override;
      private:
        uint8_t screenID;
        Widgets::PageIndicator* pageIndicator;
      };
    }
  }
}
