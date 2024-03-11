#pragma once

#include "displayapp/screens/ScreenList.h"
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
        auto createScreenList() const;
        static uint32_t indexOf(const std::array<SettingWatchFace::Item, UserWatchFaceTypes::Count>& watchfaces, WatchFace watchface);
        static WatchFace indexToWatchFace(const std::array<SettingWatchFace::Item, UserWatchFaceTypes::Count>& watchfaces, size_t index);
        std::unique_ptr<Screen> createScreen(unsigned int screenNum) const;

        static constexpr int settingsPerScreen = 4;
        std::array<Item, UserWatchFaceTypes::Count> watchfaceItems;
        static constexpr int nScreens = UserWatchFaceTypes::Count > 0 ? (UserWatchFaceTypes ::Count - 1) / settingsPerScreen + 1 : 1;

        static constexpr const char* title = "Watch face";
        static constexpr const char* symbol = Symbols::home;

        ScreenList<nScreens> screens;
      };
    }
  }
}
