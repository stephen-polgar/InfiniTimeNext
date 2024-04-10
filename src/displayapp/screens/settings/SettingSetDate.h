#pragma once

#include "SettingSetDateTime.h"
#include "displayapp/widgets/Counter.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class SettingSetDate : public ScreenTree {
      public:
        SettingSetDate(Pinetime::Applications::Screens::SettingSetDateTime& settingSetDateTime);
        ~SettingSetDate()override;

        void Load() override;
        bool UnLoad() override;        

      private:
        Pinetime::Applications::Screens::SettingSetDateTime& settingSetDateTime;
        void handleButtonPress();
        void checkDay();
        static void event_handler(lv_obj_t* obj, lv_event_t event);
        static void ValueChangedHandler(void* userData) ;

        lv_obj_t* btnSetTime;
        lv_obj_t* lblSetTime;

        Widgets::Counter dayCounter = Widgets::Counter(1, 31, jetbrains_mono_bold_20);
        Widgets::Counter monthCounter = Widgets::Counter(1, 12, jetbrains_mono_bold_20);
        Widgets::Counter yearCounter = Widgets::Counter(1970, 9999, jetbrains_mono_bold_20);         
      };
    }
  }
}
