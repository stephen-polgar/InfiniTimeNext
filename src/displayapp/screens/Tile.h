#pragma once

#include "ScreenTree.h"
#include "displayapp/widgets/PageIndicator.h"
#include "displayapp/widgets/StatusIcons.h"
#include <array>

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class Tile : public ScreenTree {
      public:
        static constexpr uint8_t MaxElements = 6;
        struct Applications {
          const char* icon;
          Pinetime::Applications::Apps application;
          bool enabled;
        };

        explicit Tile(uint8_t screenID, std::array<Applications, MaxElements>& applications, Widgets::PageIndicator* pageIndicator);

        ~Tile() override;
        void Load() override;
        bool UnLoad() override;

      private:
        void Refresh() override;
        void onValueChangedEvent(uint8_t buttonId);
        lv_task_t* taskUpdate = NULL;
        lv_obj_t* label_time;
        lv_obj_t* btnm;
        const uint8_t screenID;
        std::array<Applications, MaxElements> applications;
        Widgets::PageIndicator* pageIndicator;
        Widgets::StatusIcons statusIcons;
        const char* btnmMap[8];
        Pinetime::Applications::Apps apps[MaxElements];
      };
    }
  }
}
