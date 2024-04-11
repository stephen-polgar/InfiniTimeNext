#pragma once

#include "Screen.h"
#ifdef UseMotion

namespace Pinetime {
  namespace Applications {
    namespace Screens {

      class Motion : public Screen {
      public:
        Motion();
        ~Motion() override;
        void Load() override;
        bool UnLoad() override;
        void Refresh() override;

      private:
        lv_obj_t* chart;
        lv_chart_series_t* ser1;
        lv_chart_series_t* ser2;
        lv_chart_series_t* ser3;
        lv_obj_t* label;

        lv_obj_t* labelStep;
        lv_task_t* taskRefresh;
      };
    }

    template <>
    struct AppTraits<Apps::Motion> {
      static constexpr Apps app = Apps::Motion;
      static constexpr const char* icon = "M";

      static Screens::Screen* Create() {
        return new Screens::Motion();
      };
    };
  }
}
#endif