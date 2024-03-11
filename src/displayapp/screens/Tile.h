#pragma once

#include "Screen.h"
#include "displayapp/widgets/PageIndicator.h"
#include "displayapp/widgets/StatusIcons.h"
#include <array>

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class Tile : public Screen {
      public:
        struct Applications {
          const char* icon;
          Pinetime::Applications::Apps application;
          bool enabled;
        };

        explicit Tile(uint8_t screenID, uint8_t numScreens, std::array<Applications, 6>& applications);

        ~Tile() override;
        void Load() override;
        bool UnLoad() override;

      private:
        void Refresh() override;

        void onValueChangedEvent(lv_obj_t* obj, uint32_t buttonId);

        lv_task_t* taskUpdate;

        lv_obj_t* label_time;
        lv_obj_t* btnm1;
        uint8_t screenID;
        std::array<Applications, 6> applications;

        Widgets::PageIndicator pageIndicator;
        Widgets::StatusIcons statusIcons;

        const char* btnmMap[8];
        Pinetime::Applications::Apps apps[6];
      };
    }
  }
}
