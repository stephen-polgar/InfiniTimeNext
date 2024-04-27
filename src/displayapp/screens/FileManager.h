#pragma once

#include "displayapp/screens/Screen.h"
#ifdef UseFileManager
  #include "components/fs/FS.h"
  #include "Symbols.h"
  #include "utility/ArrayTouchHandler.h"
  #include "displayapp/widgets/LineIndicator.h"
  #include <string>
  #include <memory>

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class FileManager : public Screen {
      public:
        FileManager(const char* dir = rootDir);
        ~FileManager() override;
        void Load() override;
        bool UnLoad() override;
        bool OnTouchEvent(Applications::TouchEvents event) override;
        bool OnTouchEvent(uint16_t x, uint16_t y) override;
        bool OnButtonPushed() override;
       
      private:
        struct File {
          std::string path;
          bool dir;
          lfs_size_t size;
        };

        static constexpr uint8_t maxItems = 4;
        static constexpr const char* rootDir = "..";
        void updateSize();
        File files[maxItems];
        Utility::ArrayTouchHandler arrayTouchHandler;
        void load(uint8_t indexBegin, uint8_t indexEnd, Screen::FullRefreshDirections direction);
        std::string dirPath;
        lv_obj_t* container = NULL;
        Widgets::LineIndicator pageIndicator;
        Controllers::FS* fs;
        void openFile(lv_obj_t* obj);
        void deleteFile(lv_obj_t* obj);
        void showMenu(const char* title, lv_obj_t* obj, bool image = false);
        void closeMenu();
        static void fileEventHandler(lv_obj_t* obj, lv_event_t event);
        lv_obj_t* image = NULL;
        lv_obj_t* menuContainer = NULL;
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