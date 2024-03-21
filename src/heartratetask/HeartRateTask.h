#pragma once
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <drivers/Hrs3300.h>
#include "components/heartrate/Ppg.h"


namespace Pinetime { 
   namespace Applications {
    class HeartRateTask {
    public:
      enum class Messages : uint8_t { GoToSleep, WakeUp, StartMeasurement, StopMeasurement };
      enum class States  : uint8_t { Idle, Running };

      explicit HeartRateTask(Drivers::Hrs3300& heartRateSensor);
      void Start();
      void Work();
      void PushMessage(Messages msg);

    private:
      static void Process(void* instance);
      void StartMeasurement();
      void StopMeasurement();
      TaskHandle_t taskHandle;
      QueueHandle_t messageQueue;
      States state = States::Running;
      Drivers::Hrs3300& heartRateSensor;    
      Controllers::Ppg ppg;
      bool measurementStarted = false;
    };

  }
}
