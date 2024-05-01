#pragma once

#include "Screen.h"
#include "displayapp/widgets/LineIndicator.h"
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
        bool enableEvent;
        lv_obj_t* btnm = NULL;       
        Utility::ArrayTouchHandler arrayTouchHandler;
        std::array<AppList, UserAppTypes::Count> apps;
        Widgets::LineIndicator pageIndicator;
        void onValueChangedEvent(uint8_t index);        
        const char* btnmMap[8];
      };
    }
  }
}
