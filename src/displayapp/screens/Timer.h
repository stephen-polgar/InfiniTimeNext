#pragma once

#include "ScreenRefresh.h"
#include "displayapp/widgets/ButtonListItem.h"
#include "components/timer/TimerController.h"
#include "Symbols.h"

namespace Pinetime::Applications {
  namespace Screens {
    class Timer : public ScreenRefresh {
    public:
      Timer();
      ~Timer() override;
      void Load() override;
      bool UnLoad() override;

    private:
      void Refresh() override;
      static bool changed;
      void createNewItem(Controllers::TimerController* timer);
      void updateAddButton();
      void on_checked(bool, Controllers::TimerController*);
      void on_open(Controllers::TimerController*);
      void on_removed(Controllers::TimerController*);
      void on_addNew();
      Widgets::ButtonListItem buttonListItem;
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
