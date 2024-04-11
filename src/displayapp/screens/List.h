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
          Pinetime::Applications::Apps app;
        };

        explicit List(uint8_t screenID, Widgets::PageIndicator& pageIndicator);
        ~List() override;
        void Load() override;
        bool UnLoad() override;
        bool Add(Applications item);

      private:
        const uint8_t screenID;
        bool loaded = false;
        uint8_t size = 0;
        std::array<Applications, MaxElements> apps;      
        void onButtonEvent(lv_obj_t* obj);
        static void buttonEventHandler(lv_obj_t* obj, lv_event_t event);       
        Widgets::PageIndicator& pageIndicator;
      };
    }
  }
}
