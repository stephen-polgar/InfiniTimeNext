#pragma once

#include "SettingSetDateTime.h"
#include "displayapp/widgets/Counter.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class SettingSetTime : public Screen {
      public:
        SettingSetTime(Pinetime::Applications::Screens::SettingSetDateTime& settingSetDateTime);
        void Load() override;

      private:
        Pinetime::Applications::Screens::SettingSetDateTime& settingSetDateTime;
        void setTime();
#ifndef TimeFormat_24H
        void updateScreen();
        static void valueChangedHandler(void* userData);
        lv_obj_t* lblampm;
#endif
        static void setTimeEventHandler(lv_obj_t* obj, lv_event_t event);
        lv_obj_t* btnSetTime;
        lv_obj_t* lblSetTime;
        Widgets::Counter hourCounter = Widgets::Counter(0, 23, jetbrains_mono_42);
        Widgets::Counter minuteCounter = Widgets::Counter(0, 59, jetbrains_mono_42);
      };
    }
  }
}
