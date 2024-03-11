#pragma once

#include "Screen.h"
#include "components/timer/TimerController.h"
#include "Symbols.h"


namespace Pinetime::Applications {
  namespace Screens {
    class Timer : public Screen {
    public:
      Timer();
      ~Timer() override;
      void Load() override;
      bool UnLoad() override;

    private:
      static bool changed;
      void createElem(Controllers::TimerController* timer);
      lv_obj_t *last = NULL, *btnNew;
      void updateAddButton();   
    };
  }

  template <>
  struct AppTraits<Apps::Timer> {
    static constexpr Apps app = Apps::Timer;
    static constexpr const char* icon = Screens::Symbols::hourGlass;

    static Screens::Screen* Create() { 
      return new Screens::Timer();
    };
  };
}
