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
        static constexpr const char* directory = "/gallery";

        Gallery();
        ~Gallery() override;
        void Load() override;
        bool UnLoad() override;
        bool OnTouchEvent(Applications::TouchEvents event) override;

      private:
        uint8_t StringEndsWith(const char* str, const char* suffix);
        std::unique_ptr<FileView> current;
        Widgets::PageIndicator pageIndicator;
        void ListDir();
        bool Open(uint8_t n, FullRefreshDirections direction);
        uint8_t nScreens;
        uint8_t index;
      };
    }

    template <>
    struct AppTraits<Apps::Gallery> {
      static constexpr Apps app = Apps::Gallery;
      static constexpr const char* icon = "G";  // TODO: create icon

      static Screens::Screen* Create() {
        return new Screens::Gallery();
      };
    };
  }
}
#endif