#pragma once

#include "ButtonActions.h"
#include "systemtask/SystemTask.h"
#include <FreeRTOS.h>
#include <timers.h>

namespace Pinetime {
  namespace Controllers {
    class ButtonHandler {
    public:
      enum class Events : uint8_t { Press, Release, Timer };
      void Init(Pinetime::System::SystemTask* systemTask);
      ButtonActions HandleEvent(Events event);

    private:
      static void buttonTimerCallback(TimerHandle_t xTimer);
      enum class States : uint8_t { Idle, Pressed, Holding, LongHeld };
      TickType_t releaseTime = 0;
      TimerHandle_t buttonTimer;
      bool buttonPressed = false;
      States state = States::Idle;
    };
  }
}
