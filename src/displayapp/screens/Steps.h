#pragma once

#include "Screen.h"
#include "displayapp/apps/Apps.h"
#include "Symbols.h"

namespace Pinetime {

  namespace Controllers {
    class Settings;
  }

  namespace Applications {
    namespace Screens {

      class Steps : public Screen {
      public:
        Steps();
        ~Steps() override;
        void Load() override;
        bool UnLoad() override;

      private:
        void Refresh() override;
        void lapBtnClicked();
        static void lap_event_handler(lv_obj_t* obj, lv_event_t event);
        static void set_event_handler(lv_obj_t* obj, lv_event_t event);
        lv_obj_t *lSteps, *stepsArc, *resetBtn, *resetButtonLabel, *tripLabel;
        lv_task_t* taskRefresh;
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
