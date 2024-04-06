#pragma once

#include "displayapp/screens/ScreenList.h"
#include "displayapp/screens/CheckboxList.h"
#include "displayapp/widgets/PageIndicator.h"
#include "components/settings/Settings.h"
#include "displayapp/screens/Symbols.h"

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
        static uint8_t indexOf(const std::array<SettingWatchFace::Item, UserWatchFaceTypes::Count>& watchfaces, WatchFace watchface);
        static WatchFace indexToWatchFace(const std::array<SettingWatchFace::Item, UserWatchFaceTypes::Count>& watchfaces, uint8_t index);
        Screen* createScreen(uint8_t screenNum);
      
        std::array<Item, UserWatchFaceTypes::Count> watchfaceItems;
        static constexpr uint8_t nScreens = UserWatchFaceTypes::Count > 0 ? (UserWatchFaceTypes ::Count - 1) / CheckboxList::MaxItems + 1 : 1;

        static constexpr const char* title = "Watch face";
        static constexpr const char* symbol = Symbols::home;
        Widgets::PageIndicator pageIndicator;
        ScreenList<nScreens> screens;
      };
    }
  }
}
