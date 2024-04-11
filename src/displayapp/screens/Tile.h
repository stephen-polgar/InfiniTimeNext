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
          Pinetime::Applications::Apps app;
        };

        explicit Tile(uint8_t screenID, Widgets::PageIndicator* pageIndicator);
        ~Tile() override;
        void Load() override;
        bool UnLoad() override;
        bool Add(Applications item);

      private:
        void Refresh() override;
        void onValueChangedEvent(uint8_t index);
        lv_task_t* taskUpdate = NULL;
        lv_obj_t* label_time;
        const uint8_t screenID;
        uint8_t size = 0;
        const char* btnmMap[8];
        std::array<Applications, MaxElements> apps;
        Widgets::PageIndicator* pageIndicator;
        Widgets::StatusIcons statusIcons;
      };
    }
  }
}
