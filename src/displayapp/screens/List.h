#pragma once

#include "displayapp/screens/Screen.h"
#include "displayapp/widgets/PageIndicator.h"
#include <array>

#define MAXLISTITEMS 4

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class List : public Screen {
      public:
        struct Applications {
          const char* icon;
          const char* name;
          Pinetime::Applications::Apps application;
        };

        explicit List(uint8_t screenID, uint8_t numScreens, std::array<Applications, MAXLISTITEMS>& applications, Widgets::PageIndicator& pageIndicator);
        ~List() override;
        void Load() override;
        bool UnLoad() override;

      private:
        const uint8_t screenID, numScreens;
        bool loaded = false;
        std::array<Applications, MAXLISTITEMS> applications;
        Pinetime::Applications::Apps apps[MAXLISTITEMS];
        void onButtonEvent(lv_obj_t* object, lv_event_t event);
        static void buttonEventHandler(lv_obj_t* obj, lv_event_t event);
        lv_obj_t* itemApps[MAXLISTITEMS];
        Widgets::PageIndicator& pageIndicator;
      };
    }
  }
}
