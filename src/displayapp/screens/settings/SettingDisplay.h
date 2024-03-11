#pragma once

#include "displayapp/screens/Screen.h"
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
        void updateSelected(lv_obj_t* object);
        static void event_handlerGoal(lv_obj_t* obj, lv_event_t event);

        lv_obj_t* cbOption[options.size()];
      };
    }
  }
}
