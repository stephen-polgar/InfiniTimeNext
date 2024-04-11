#pragma once

#include "displayapp/screens/Screen.h"
#include "BootErrors.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class Error : public Screen {
      public:
        Error(System::BootErrors error);
        ~Error() override;
        void Load() override;
        bool UnLoad() override;

      private:
        System::BootErrors error;
        static void buttonEventCallback(lv_obj_t*, lv_event_t);
      };
    }
  }
}
