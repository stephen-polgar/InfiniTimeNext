#pragma once

#include <FreeRTOS.h>
#include <timers.h>
#include "heartratetask/HeartRateTask.h"
#include "components/settings/Settings.h"
#include "components/motion/MotionController.h"
#include "drivers/Bma421.h"
#include "drivers/PinMap.h"
#include "drivers/Hrs3300.h"
#include "drivers/TwiMaster.h"
#include "drivers/SpiMaster.h"
#include "systemtask/SystemMonitor.h"
#include "components/ble/NimbleController.h"
#include "buttonhandler/ButtonActions.h"
#include "buttonhandler/ButtonHandler.h"

#ifdef PINETIME_IS_RECOVERY
  #include "displayapp/DisplayAppRecovery.h"
#else
  #include "components/settings/Settings.h"
  #include "displayapp/DisplayApp.h"
#endif

extern std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> NoInit_BackUpTime;

namespace Pinetime {

  namespace Applications {
    class DisplayApp;
  }

  namespace System {
    class SystemTask {
    public:
      enum class SystemTaskState : uint8_t { Sleeping, Running, GoingToSleep, WakingUp };
      SystemTask(Drivers::SpiMaster& spi,                 
                 Drivers::TwiMaster& twiMaster,
                 Drivers::Hrs3300& heartRateSensor,
                 Drivers::Bma421& motionSensor,
                 Applications::DisplayApp* displayApp);

      void Start();
      void PushMessage(Messages id);

      void OnTouchEvent();

      bool IsSleepDisabled() {
        return doNotGoToSleep;
      }
      
      bool IsSleeping() const {
        return state == SystemTaskState::Sleeping || state == SystemTaskState::WakingUp;
      }

      static Applications::DisplayApp* displayApp;     
      Applications::HeartRateTask heartRateTask;
      Controllers::NimbleController nimbleController;

    private:
      TaskHandle_t taskHandle;
      Drivers::SpiMaster& spi;
      Drivers::TwiMaster& twiMaster;
      QueueHandle_t systemTasksMsgQueue;
      Drivers::Hrs3300& heartRateSensor;
      Drivers::Bma421& motionSensor;
      Controllers::ButtonHandler buttonHandler;

      static void Process(void* instance);
      void Work();
      bool isBleDiscoveryTimerRunning = false;
      uint8_t bleDiscoveryTimer = 0;
      TimerHandle_t measureBatteryTimer;
      bool doNotGoToSleep = false;
      SystemTaskState state = SystemTaskState::Running;

      void HandleButtonAction(Controllers::ButtonActions action);
      bool fastWakeUpDone = false;

      void GoToRunning();
      void UpdateMotion();
      bool stepCounterMustBeReset = false;
      static void measureBatteryTimerCallback(TimerHandle_t xTimer);
      static constexpr TickType_t batteryMeasurementPeriod = pdMS_TO_TICKS(600000); // 10min

      SystemMonitor monitor;
    };
  }
}
