#pragma once

#include "ScreenRefresh.h"
#include "components/heartrate/HeartRateController.h"
#include "Symbols.h"

namespace Pinetime {
   namespace Applications {
    namespace Screens {

      class HeartRate : public ScreenRefresh {
      public:
        HeartRate();
        ~HeartRate() override;
        void Load() override;
        bool UnLoad() override;

      private:
        void updateStartStopButton(bool isRunning);
        void Refresh() override;
        void onStartStopEvent();
        static void btnStartStopEventHandler(lv_obj_t* obj, lv_event_t event);
        static const char* toString(Controllers::HeartRateController::States s);
        lv_obj_t* label_hr;
        lv_obj_t* label_status;
        lv_obj_t* label_startStop;
      };
    }

    template <>
    struct AppTraits<Apps::HeartRate> {
      static constexpr Apps app = Apps::HeartRate;
      static constexpr const char* icon = Screens::Symbols::heartBeat;

      static Screens::Screen* Create() {
        return new Screens::HeartRate();
      };
    };
  }
}
