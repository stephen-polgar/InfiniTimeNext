#pragma once

#include "displayapp/screens/BatteryIcon.h"
#include "utility/DirtyValue.h"

namespace Pinetime {
  namespace Applications {
    namespace Widgets {
      class StatusIcons {
      public:
        StatusIcons();
        void Create();

        lv_obj_t* GetObject() {
          return container;
        }

        void Update();

      private:
        bool update = false;
        Screens::BatteryIcon batteryIcon;
        Utility::DirtyValue<uint8_t> batteryPercentRemaining;
        Utility::DirtyValue<bool> powerPresent;
        Utility::DirtyValue<bool> bleState;
        Utility::DirtyValue<bool> bleRadioEnabled;

        lv_obj_t* bleIcon;
        lv_obj_t* batteryPlug;
        lv_obj_t* container;
      };
    }
  }
}
