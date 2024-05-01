#pragma once

#include "displayapp/screens/Screen.h"

namespace Pinetime {

  namespace Applications {
    namespace Screens {
      class QuickSettings : public Screen {
      public:
        QuickSettings();
        ~QuickSettings() override;
        void Load() override;
        bool UnLoad() override;
        bool OnTouchEvent(TouchEvents event) override;

      private:
        void onButtonEvent(lv_obj_t* object);
        enum class ButtonState : lv_state_t { NotificationsOn = LV_STATE_CHECKED, NotificationsOff = LV_STATE_DEFAULT, Sleep = 0x40 };

        static void buttonEventHandler(lv_obj_t* obj, lv_event_t event);

        lv_style_t btn_style;
        lv_obj_t* btn1;
        lv_obj_t* btn1_lvl;
        lv_obj_t* btn2;
        lv_obj_t* btn3;
        lv_obj_t* btn3_lvl;
        lv_obj_t* btn4;
        bool enableEvent;
      };
    }
  }
}
