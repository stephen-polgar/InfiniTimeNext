
#pragma once

#include "Screen.h"
#include <string>

namespace Pinetime {
  namespace Applications {
    namespace Screens {

      class WatchFaceScreen : public Screen {
      public:
        WatchFaceScreen();
        ~WatchFaceScreen() override;
        void Load() override;
        bool UnLoad() override;
        bool OnButtonPushed() override;
        bool OnTouchEvent(uint16_t, uint16_t) override;
        bool OnTouchEvent(TouchEvents) override;
        static std::string bgImage;
        static lv_obj_t* image;

      private:
        void Refresh() override;
        lv_task_t* taskRefresh;
        Screen* current = NULL;
      };
    }
  }
}
