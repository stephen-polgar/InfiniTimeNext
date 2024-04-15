#pragma once

#include "displayapp/screens/ScreenTree.h"
#include "displayapp/screens/CheckboxList.h"
#include "displayapp/widgets/LineIndicator.h"
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
        static constexpr uint8_t MaxCheckboxItems = 4;
        static constexpr const char* title = "Watch face";
        static constexpr const char* symbol = Symbols::home;
        std::array<Item, UserWatchFaceTypes::Count> watchfaces;  
        uint8_t indexOf(const WatchFace watchface);
        Widgets::LineIndicator pageIndicator;       
        ScreenTree* screens;
      };
    }
  }
}
