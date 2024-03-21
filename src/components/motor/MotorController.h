#pragma once

#include <FreeRTOS.h>
#include <timers.h>

namespace Pinetime {
  namespace Controllers {

    class MotorController {
    public:     
      void Init();
      void RunForDuration(uint8_t motorDuration);
      void StartRinging();
      void StopRinging();

    private:
      static void Ring(TimerHandle_t xTimer);
      static void StopMotor(TimerHandle_t xTimer);
      TimerHandle_t shortVib;
      TimerHandle_t longVib;
    };
  }
}
