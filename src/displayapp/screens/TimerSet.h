#pragma once

#include "Screen.h"
#include "displayapp/widgets/Counter.h"
#include "components/timer/TimerController.h"

namespace Pinetime::Applications {
  namespace Screens {
    class TimerSet : public Screen {
    public:
      TimerSet(Controllers::TimerController* timer);
      TimerSet();
      ~TimerSet() override;
      void Load() override;
      bool UnLoad() override;
      void TimerDone();

    private:
      void Refresh() override;
      void reset();
      void toggleRunning();
      void buttonPressed();
      void maskReset();
      void setTimerRunning();
      void setTimerStopped();
      void updateMask();
      static void btnEventHandler(lv_obj_t* obj, lv_event_t event);
      void updateTime(std::chrono::milliseconds);
      void setTimer(Controllers::TimerController* timer);
      Controllers::TimerController* timer = NULL;

      lv_obj_t* btnPlayPause;
      lv_obj_t* txtPlayPause;

      lv_obj_t* btnObjectMask;
      lv_obj_t* highlightObjectMask;
      lv_objmask_mask_t* btnMask;
      lv_objmask_mask_t* highlightMask;

      lv_task_t* taskRefresh;
      Widgets::Counter minuteCounter = Widgets::Counter(0, 59, jetbrains_mono_76);
      Widgets::Counter secondCounter = Widgets::Counter(0, 59, jetbrains_mono_76);

      bool buttonPressing = false;
      lv_coord_t maskPosition = 0;
      TickType_t pressTime = 0;
    };
  }
}
