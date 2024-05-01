#pragma once

#include "displayapp/screens/ScreenRefresh.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {

      class SettingShakeThreshold : public ScreenRefresh {
      public:
        SettingShakeThreshold();

        ~SettingShakeThreshold() override;
        void Load() override;
        bool UnLoad() override;

      private:
        void Refresh() override;
        uint32_t screenTimeout;
        void updateSelected(lv_obj_t* object);
        static void event_handler(lv_obj_t* obj, lv_event_t event);
        uint8_t calibrating;
        bool EnableForCal;
        uint32_t vDecay, vCalTime;
        lv_obj_t *positionArc, *animArc, *calButton, *calLabel;       
      };
    }
  }
}
