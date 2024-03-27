#pragma once

#include "displayapp/screens/Screen.h"
#ifdef UseCalendar
  #include "displayapp/widgets/StatusIcons.h"
//  #include "Symbols.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class Calendar : public Screen {
      public:
        Calendar();
        ~Calendar() override;
        void Load() override;
        bool UnLoad() override;
        bool OnTouchEvent(TouchEvents event) override;

      private:
        void Refresh() override;
        lv_task_t* taskUpdate;
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