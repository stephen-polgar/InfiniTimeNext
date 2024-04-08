#pragma once

#include "displayapp/screens/Screen.h"
#ifdef UseFileManager
  #include "displayapp/widgets/PageIndicator.h"
  #include <string>
  #include "components/fs/FS.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class File {
      public:
        File(lfs_info& info);
        File(const char* path, bool dir = true);
        ~File();
        bool IsDir();
        bool Open();
        File* parrent = NULL;

      private:
        bool dir;
        std::string path;
        lfs_size_t size;
      };

      class FileView : public Screen {
      public:
        FileView();

        ~FileView() override;
        void Load() override;
        bool UnLoad() override;

      private:
      };
    }
  }
}
#endif
