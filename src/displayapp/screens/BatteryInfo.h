#pragma once

#include "Screen.h"

namespace Pinetime {
   namespace Applications {
    namespace Screens {

      class BatteryInfo : public Screen {
      public:
        BatteryInfo();
        ~BatteryInfo()override;
        void Load() override;
        bool UnLoad() override;      

      private:
       void Refresh() override;   

        lv_obj_t* voltage;
        lv_obj_t* percent;
        lv_obj_t* charging_bar;
        lv_obj_t* status;

        lv_task_t* taskRefresh;

        uint8_t batteryPercent = 0;
        uint16_t batteryVoltage = 0;
      };
    }
  }
}
