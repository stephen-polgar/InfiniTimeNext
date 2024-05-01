#pragma once

#include "ScreenRefresh.h"
#include "displayapp/widgets/ButtonListItem.h"
#include "components/alarm/AlarmController.h"
#include "Symbols.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class Alarm : public ScreenRefresh {
      public:
        explicit Alarm();
        ~Alarm() override;
        void Load() override;
        bool UnLoad() override;

      private:
        void Refresh() override;
        void createNewItem(Controllers::AlarmController* alarmController);
        static bool changed;
        void updateAddButton();
        void on_checked(bool, Controllers::AlarmController*);
        void on_open(Controllers::AlarmController*);
        void on_removed(Controllers::AlarmController*);
        void on_addNew();
        Widgets::ButtonListItem buttonListItem;
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
