#pragma once

#include "Screen.h"
#include "displayapp/widgets/LineIndicator.h"
#include "displayapp/widgets/StatusIcons.h"
#include "utility/ArrayTouchHandler.h"
#include <array>

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class ApplicationList : public Screen {
      public:
        struct AppList {
          const char* icon;
          Pinetime::Applications::Apps app;
        };

        explicit ApplicationList(std::array<AppList, UserAppTypes::Count>& apps);
        ~ApplicationList() override;
        void Load() override;
        bool UnLoad() override;
        bool OnTouchEvent(TouchEvents event) override;

      private:
        static constexpr uint8_t maxItems = 6;
        void load(uint8_t indexBegin, uint8_t indexEnd, Screen::FullRefreshDirections direction);
        uint8_t indexBegin;
        void unLoad();
        lv_obj_t* btnm;
        void Refresh() override;
        Utility::ArrayTouchHandler arrayTouchHandler;
        std::array<AppList, UserAppTypes::Count> apps;
        Widgets::LineIndicator pageIndicator;
        Widgets::StatusIcons statusIcons;
        void onValueChangedEvent(uint8_t index);
        lv_task_t* taskUpdate = NULL;
        lv_obj_t* label_time;
        const char* btnmMap[8];
      };
    }
  }
}
