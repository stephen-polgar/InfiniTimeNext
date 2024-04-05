#pragma once

#include "displayapp/screens/Screen.h"
#ifdef UseGallery
  #include "Symbols.h"
  #include "displayapp/screens/FileView.h"
  #include <memory>

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class Gallery : public Screen {
      public:
        // static constexpr const char* directory = "/gallery";
        static constexpr const char* directory = "/images";

        Gallery();
        ~Gallery() override;
        void Load() override;
        bool UnLoad() override;
        bool OnTouchEvent(Applications::TouchEvents event) override;

      private:
        static uint8_t stringEndsWith(const char* str, const char* suffix);
        std::unique_ptr<FileView> current;
        Widgets::PageIndicator pageIndicator;
        void ListDir();
        bool Open(uint8_t n, FullRefreshDirections direction);
        uint8_t nScreens;
        uint8_t index;
        Controllers::FS* fs;
      };
    }

    template <>
    struct AppTraits<Apps::Gallery> {
      static constexpr Apps app = Apps::Gallery;
      static constexpr const char* icon = "G"; // TODO: create icon

      static Screens::Screen* Create() {
        return new Screens::Gallery();
      };
    };
  }
}
#endif