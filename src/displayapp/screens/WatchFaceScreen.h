
#pragma once

#include "Screen.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {

      class WatchFaceScreen : public Screen {
      public:
        WatchFaceScreen();
        ~WatchFaceScreen() override;

        void Load() override;
        bool UnLoad() override;

        bool OnTouchEvent(uint16_t, uint16_t) override;
        bool OnTouchEvent(TouchEvents) override;

      private:
        void Refresh() override;
        lv_task_t* taskRefresh;
        Screen* current = NULL;       
      };
    }
  }
}
