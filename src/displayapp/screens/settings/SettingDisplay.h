#pragma once

#include "displayapp/screens/Screen.h"
#include "displayapp/screens/CheckboxList.h"
#include <array>

namespace Pinetime {

  namespace Applications {
    namespace Screens {

      class SettingDisplay : public Screen {
      public:
        SettingDisplay();
        ~SettingDisplay() override;
        void Load() override;
        bool UnLoad() override;

      private:
        static constexpr std::array<uint16_t, 6> options = {5000, 7000, 10000, 15000, 20000, 30000};
        CheckboxList checkboxList;
        uint8_t getCurrentOption(uint32_t currentOption);
      };
    }
  }
}
