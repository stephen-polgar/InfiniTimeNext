#pragma once

#include "ScreenRefresh.h"

namespace Pinetime {
   namespace Applications {
    namespace Screens {

      class BatteryInfo : public ScreenRefresh {
      public:
        BatteryInfo();
        void Load() override;        

      private:
       void Refresh() override;   
        lv_obj_t* voltage;
        lv_obj_t* percent;
        lv_obj_t* charging_bar;
        lv_obj_t* status;
      };
    }
  }
}
