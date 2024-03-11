#pragma once

#include <FreeRTOS.h>
#include <timers.h>
#include <chrono>
#include <vector>


namespace Pinetime {
  namespace Controllers {
    class TimerController {
    public:
      TimerController(std::chrono::milliseconds duration = (std::chrono::milliseconds) 0);
      ~TimerController();

      void StartTimer(std::chrono::milliseconds duration);

      void StopTimer();

      std::chrono::milliseconds GetTimeRemaining();
      std::chrono::milliseconds duration;
      bool IsRunning();
      bool done = false;

      static constexpr uint8_t MaxElements = 4;
      static std::vector<TimerController*> timers;
      static void Init(TimerCallbackFunction_t pxCallbackFunction);
      static void Save();

    private:
      TimerHandle_t timer;
      static TimerCallbackFunction_t pxCallbackFunction;     
      static const char* fileName;
    };
  }
}
