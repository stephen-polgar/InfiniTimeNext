#pragma once

#include "displayapp/screens/Screen.h"

namespace Pinetime {

  namespace Applications {
    namespace Screens {

      class SettingBluetooth : public Screen {
      public:
        SettingBluetooth();
        void Load() override;
      };
    }
  }
}
