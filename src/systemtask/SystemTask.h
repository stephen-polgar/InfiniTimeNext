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
#include "drivers/SpiNorFlash.h"
#include "systemtask/SystemMonitor.h"
#include "components/ble/NimbleController.h"
#include "buttonhandler/ButtonActions.h"

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
 
  namespace Controllers {
     class ButtonHandler;
  }

  namespace System {
    class SystemTask {
    public:
      enum class SystemTaskState : uint8_t { Sleeping, Running, GoingToSleep, WakingUp };
      SystemTask(Drivers::SpiMaster& spi,
                 Drivers::SpiNorFlash& spiNorFlash,
                 Drivers::TwiMaster& twiMaster,                            
                 Drivers::Hrs3300& heartRateSensor,
                 Drivers::Bma421& motionSensor,
                 Applications::HeartRateTask& heartRateApp,
                 Controllers::ButtonHandler& buttonHandler,
                 Applications::DisplayApp* displayApp);

      void Start();
      void PushMessage(Messages id);
     
      void OnTouchEvent();

      bool IsSleepDisabled() {
        return doNotGoToSleep;
      }

      Controllers::NimbleController& nimble() {
        return nimbleController;
      };

      bool IsSleeping() const {
        return state == SystemTaskState::Sleeping || state == SystemTaskState::WakingUp;
      }

      static Applications::DisplayApp* displayApp;

    private:
      TaskHandle_t taskHandle;

      Drivers::SpiMaster& spi;
      Drivers::SpiNorFlash& spiNorFlash;
      Drivers::TwiMaster& twiMaster;    
      QueueHandle_t systemTasksMsgQueue;     
      Drivers::Hrs3300& heartRateSensor;
      Drivers::Bma421& motionSensor;
      Applications::HeartRateTask& heartRateApp;
      Controllers::ButtonHandler& buttonHandler;
      Controllers::NimbleController nimbleController;

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
      static constexpr TickType_t batteryMeasurementPeriod = pdMS_TO_TICKS(600000); // 10min

      SystemMonitor monitor;
    };
  }
}
