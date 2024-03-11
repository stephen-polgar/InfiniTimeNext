#pragma once

#include "displayapp/screens/Screen.h"
#include "displayapp/widgets/StatusIcons.h"

namespace Pinetime {

  namespace Applications {
    namespace Screens {

      class QuickSettings : public Screen {
      public:
        QuickSettings();

        ~QuickSettings() override;
        void Load() override;
        bool UnLoad() override;
        void OnButtonEvent(lv_obj_t* object);

      private:
        enum class ButtonState : lv_state_t {
          NotificationsOn = LV_STATE_CHECKED,
          NotificationsOff = LV_STATE_DEFAULT,
          Sleep = 0x40,
        };

        void Refresh() override;

        static void ButtonEventHandler(lv_obj_t* obj, lv_event_t event);

        lv_task_t* taskUpdate;
        lv_obj_t* label_time;

        lv_style_t btn_style;

        lv_obj_t* btn1;
        lv_obj_t* btn1_lvl;
        lv_obj_t* btn2;
        lv_obj_t* btn3;
        lv_obj_t* btn3_lvl;
        lv_obj_t* btn4;

        Widgets::StatusIcons statusIcons;
      };
    }
  }
}
