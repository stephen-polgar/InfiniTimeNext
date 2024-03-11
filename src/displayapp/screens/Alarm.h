#pragma once

#include "Screen.h"
#include "components/alarm/AlarmController.h"
#include "components/settings/Settings.h"
#include "Symbols.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class Alarm : public Screen {
      public:
        explicit Alarm();
        ~Alarm() override;
        void Load() override;
        bool UnLoad() override;

      private:
        void createElem(Controllers::AlarmController* alarmController);
        lv_obj_t *last = NULL, *btnNew;
        void updateAddButton();
        static bool changed;
      };
    }

    template <>
    struct AppTraits<Apps::Alarm> {
      static constexpr Apps app = Apps::Alarm;
      static constexpr const char* icon = Screens::Symbols::clock;

      static Screens::Screen* Create() { 
        return new Screens::Alarm();
      };
    };
  }
}
