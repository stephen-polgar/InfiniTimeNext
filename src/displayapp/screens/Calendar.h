#pragma once

#include "displayapp/screens/ScreenRefresh.h"
#ifdef UseCalendar
  #include "displayapp/widgets/StatusIcons.h"
//  #include "Symbols.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class Calendar : public ScreenRefresh {
      public:
        Calendar();
        void Load() override;
        bool OnTouchEvent(TouchEvents event) override;

      private:
        void Refresh() override;
        lv_obj_t* label_time;
        lv_obj_t* calendar;
        lv_calendar_date_t today;
        lv_calendar_date_t current;
        Widgets::StatusIcons statusIcons;
      };
    }

    template <>
    struct AppTraits<Apps::Calendar> {
      static constexpr Apps app = Apps::Calendar;
      static constexpr const char* icon = "D"; // TODO: create icon

      static Screens::Screen* Create() {
        return new Screens::Calendar();
      };
    };

  }
}
#endif