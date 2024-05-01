#pragma once

#include "displayapp/screens/Screen.h"

namespace Pinetime {

  namespace Applications {
    namespace Screens {

      class SettingSteps : public Screen {
      public:
        SettingSteps();
        ~SettingSteps() override;
        void Load() override;
      
      private:
        void updateGoal(lv_obj_t* object, lv_event_t event);
        void updateLength(lv_obj_t* object, lv_event_t event);
        static void event_handlerGoal(lv_obj_t* obj, lv_event_t event);
        static void event_handlerLength(lv_obj_t* obj, lv_event_t event);
        lv_obj_t *stepGoal, *stepLengt, *btnPlus, *btnMinus, *btnM, *btnP;
      };
    }
  }
}
