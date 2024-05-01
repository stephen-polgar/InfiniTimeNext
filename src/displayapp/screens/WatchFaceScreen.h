
#pragma once

#include "ScreenRefresh.h"
#include <string>

namespace Pinetime {
  namespace Applications {
    namespace Screens {

      class WatchFaceScreen : public ScreenRefresh {
      public:
        WatchFaceScreen();
        ~WatchFaceScreen() override;
        void Load() override;
        bool UnLoad() override;
        bool OnButtonPushed() override;
        bool OnTouchEvent(uint16_t, uint16_t) override;
        bool OnTouchEvent(TouchEvents) override;
#ifdef UseFileManager
        static std::string bgImage;
        static lv_obj_t* image;
#endif
      private:
        void Refresh() override;      
        Screen* current = NULL;
      };
    }
  }
}
