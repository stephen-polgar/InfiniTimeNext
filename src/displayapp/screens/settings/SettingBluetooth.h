#pragma once

#include "displayapp/screens/Screen.h"

namespace Pinetime {

  namespace Applications {
    namespace Screens {

      class SettingBluetooth : public Screen {
      public:
        SettingBluetooth();
        ~SettingBluetooth() override;

        void Load() override;
        bool UnLoad() override;
      };
    }
  }
}
