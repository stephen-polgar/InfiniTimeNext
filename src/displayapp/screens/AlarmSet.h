#pragma once

#include "Screen.h"
#include "displayapp/widgets/Counter.h"
#include "components/alarm/AlarmController.h"

using Pinetime::Controllers::AlarmController;

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class AlarmSet : public Screen {
      public:
        AlarmSet(bool showAlarm = false);
        AlarmSet(AlarmController* alarmController = NULL);
        ~AlarmSet() override;
        void Load() override;
        bool UnLoad() override;
        bool OnButtonPushed() override;
        bool OnTouchEvent(TouchEvents event) override;

        void SetAlerting();
        void StopAlerting();
        bool OnAlertingState();

      private:
        void onValueChanged();
        static void valueChangedHandler(void* userData);
        bool changed = false;
        static constexpr uint8_t optionsSize = 8;
#ifdef WeekStartsMonday
        const char* options[optionsSize] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun", "Once"};
#else
        const char* options[optionsSize] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Once"};
#endif
        lv_obj_t *cont1, *cont2, *cbOnce;

        AlarmController* alarmController;

        lv_obj_t *btnStop, *btnRecur, *txtRecur;
#ifndef TimeFormat_24H
        lv_obj_t* lblampm;
#endif
        lv_task_t* taskStopAlarm = NULL;

        void showRecurences();
        void invertSeledtedDays();
        void saveRecurences();
        void updateRecurencesButton();
        void UpdateAlarmTime();
        Widgets::Counter hourCounter = Widgets::Counter(0, 23, jetbrains_mono_76);
        Widgets::Counter minuteCounter = Widgets::Counter(0, 59, jetbrains_mono_76);
      };
    }
  }
}
