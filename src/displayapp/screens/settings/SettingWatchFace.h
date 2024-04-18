#pragma once

#include "displayapp/screens/Screen.h"
#include "displayapp/widgets/LineIndicator.h"
#include "displayapp/widgets/Container.h"
#include "components/settings/Settings.h"
#include "utility/ArrayTouchHandler.h"
#include <array>

namespace Pinetime {
  namespace Applications {
    namespace Screens {

      class SettingWatchFace : public Screen {
      public:
        struct Item {
          const char* name;
          WatchFace watchface; 
          bool enabled;
        };

        SettingWatchFace(std::array<Item, UserWatchFaceTypes::Count>&& watchfaceItems);
        ~SettingWatchFace() override;
        void Load() override;
        bool UnLoad() override;
        bool OnTouchEvent(TouchEvents event) override;

      private:
        static constexpr uint8_t maxItems = 4;
        void load(uint8_t indexBegin, uint8_t indexEnd, Screen::FullRefreshDirections direction);
        Utility::ArrayTouchHandler arrayTouchHandler;
        void unload();
        std::array<Item, UserWatchFaceTypes::Count> watchfaces;
        Widgets::LineIndicator pageIndicator;
        Widgets::Container container;
        WatchFace watchface;
        void onButtonEvent(lv_obj_t* obj);
        static void buttonEventHandler(lv_obj_t* obj, lv_event_t event);
      };
    }
  }
}
