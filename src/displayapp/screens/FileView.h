#pragma once

#include "displayapp/screens/Screen.h"
#ifdef UseGallery
#include "displayapp/widgets/PageIndicator.h"
#include "components/fs/FS.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class FileView : public Screen {
      public:
        FileView(uint8_t screenID, uint8_t nScreens, const char* path, Widgets::PageIndicator& pageIndicator);
        ~FileView() override;
        void Load() override;
        bool UnLoad() override;

        void ToggleInfo();

      private:
        char name[LFS_NAME_MAX];
        lv_obj_t* label;

        Widgets::PageIndicator& pageIndicator;
        const uint8_t screenID, nScreens;
      };

      class ImageView : public FileView {
      public:
        ImageView(uint8_t screenID, uint8_t nScreens, const char* path, Widgets::PageIndicator& pageIndicator);
        void Load() override;
        bool UnLoad() override;
      };

      class TextView : public FileView {
      public:
        TextView(uint8_t screenID,
                 uint8_t nScreens,
                 const char* path,                
                 Widgets::PageIndicator& pageIndicator);
        ~TextView() override;
        void Load() override;
        bool UnLoad() override;

      private:
        char* buf;
      };
    }
  }
}
#endif