#pragma once

#include "displayapp/screens/Screen.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class ScreenRefresh : public Screen {
      public:
        virtual ~ScreenRefresh();

        virtual bool UnLoad();

      protected:
        ScreenRefresh(Apps Id);
         void createRefreshTask(uint32_t period, lv_task_prio_t prio);

        static void refreshTaskCallback(lv_task_t* task);
        lv_task_t* taskRefresh = NULL;
      };
    }
  }
}
