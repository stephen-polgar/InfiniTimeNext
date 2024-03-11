#pragma once

#include "displayapp/screens/Screen.h"
#include "SettingSetDateTime.h"
#include "displayapp/widgets/Counter.h"
#include "displayapp/widgets/DotIndicator.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class SettingSetTime : public Screen {
      public:
        SettingSetTime(Pinetime::Applications::Screens::SettingSetDateTime& settingSetDateTime);
        ~SettingSetTime() override;
        void Load() override;
        bool UnLoad() override;

      private:
        Pinetime::Applications::Screens::SettingSetDateTime& settingSetDateTime;
        void setTime();
        void updateScreen();
        static void setTimeEventHandler(lv_obj_t* obj, lv_event_t event);
        static void valueChangedHandler(void* userData);
        lv_obj_t* lblampm;
        lv_obj_t* btnSetTime;
        lv_obj_t* lblSetTime;
        Widgets::Counter hourCounter = Widgets::Counter(0, 23, jetbrains_mono_42);
        Widgets::Counter minuteCounter = Widgets::Counter(0, 59, jetbrains_mono_42);
        Widgets::DotIndicator dotIndicator{1, 2};
      };
    }
  }
}
