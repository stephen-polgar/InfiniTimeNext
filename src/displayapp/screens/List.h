#pragma once

#include "displayapp/screens/ScreenTree.h"
#include "displayapp/widgets/PageIndicator.h"
#include <array>

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class List : public ScreenTree {
      public:
        static constexpr uint8_t MaxElements = 4;

        struct Applications {
          const char* icon;
          const char* name;
          Pinetime::Applications::Apps application;
        };

        explicit List(uint8_t screenID, std::array<Applications, MaxElements>& applications, Widgets::PageIndicator& pageIndicator);
        ~List() override;
        void Load() override;
        bool UnLoad() override;

      private:
        const uint8_t screenID;
        bool loaded = false;
        std::array<Applications, MaxElements> applications;
        Pinetime::Applications::Apps apps[MaxElements];
        void onButtonEvent(lv_obj_t* object, lv_event_t event);
        static void buttonEventHandler(lv_obj_t* obj, lv_event_t event);
        lv_obj_t* itemApps[MaxElements];
        Widgets::PageIndicator& pageIndicator;
      };
    }
  }
}
