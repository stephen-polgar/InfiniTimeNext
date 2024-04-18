#pragma once

#include "displayapp/screens/Screen.h"
#ifdef UseFileManager
  #include "Symbols.h"
  #include "displayapp/screens/FileView.h"
  #include <memory>

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class FileManager : public Screen {
      public:
               
        FileManager();
        ~FileManager() override;
        void Load() override;
        bool UnLoad() override;
        bool OnTouchEvent(Applications::TouchEvents event) override;

      private:
        File root{"/"};
        //const uint8_t nScreens;
       // Widgets::PageIndicator* pageIndicator;
       // ScreenList<nScreens> screens;
         Controllers::FS* fs;
      };
    }

    template <>
    struct AppTraits<Apps::FileManager> {
      static constexpr Apps app = Apps::FileManager;
      static constexpr const char* icon = "F"; // TODO: create icon

      static Screens::Screen* Create() {
        return new Screens::FileManager();
      };
    };
  }
}
#endif