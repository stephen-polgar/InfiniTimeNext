#pragma once

#include "ScreenRefresh.h"
#include "displayapp/apps/Apps.h"
#include "Symbols.h"

namespace Pinetime {
    namespace Applications {
    namespace Screens {

      class Steps : public ScreenRefresh {
      public:
        Steps();       
        void Load() override;
        bool OnTouchEvent(TouchEvents) override;

      private:
        void Refresh() override;
        void lapBtnClicked();
        static void lap_event_handler(lv_obj_t* obj, lv_event_t event);      
        lv_obj_t *lSteps, *stepsArc, *resetBtn, *resetButtonLabel, *tripLabel;       
      };
    }

    template <>
    struct AppTraits<Apps::Steps> {
      static constexpr Apps app = Apps::Steps;
      static constexpr const char* icon = Screens::Symbols::shoe;

      static Screens::Screen* Create() {
        return new Screens::Steps();
      };
    };
  }
}
