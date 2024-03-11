#include "SystemTask.h"
#include "buttonhandler/ButtonHandler.h"
#include "components/alarm/AlarmController.h"
#include <hal/nrf_rtc.h>
#include <libraries/gpiote/app_gpiote.h>
#include "BootloaderVersion.h"
#include "components/battery/BatteryController.h"
#include "components/ble/BleController.h"
#include "drivers/Cst816s.h"
#include "drivers/St7789.h"
#include "drivers/InternalFlash.h"
#include "drivers/Watchdog.h"
#include "main.h"
#include "components/timer/TimerController.h"

// #define Log

#ifdef Log
  #include <nrf_log.h>
#endif

using namespace Pinetime::System;

Pinetime::Applications::DisplayApp* SystemTask::displayApp;

namespace {
  inline bool in_isr() {
    return (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0;
  }

  void DisplayAppCallbackTimer(TimerHandle_t xTimer) {
    (static_cast<Pinetime::Controllers::TimerController*>(pvTimerGetTimerID(xTimer)))->done = true;
    SystemTask::displayApp->PushMessage(Pinetime::Applications::Display::Messages::TimerDone);
  }
}

void MeasureBatteryTimerCallback(TimerHandle_t xTimer) {
  auto* sysTask = static_cast<SystemTask*>(pvTimerGetTimerID(xTimer));
  sysTask->PushMessage(Pinetime::System::Messages::MeasureBatteryTimerExpired);
}

SystemTask::SystemTask(Drivers::SpiMaster& spi,
                       Drivers::SpiNorFlash& spiNorFlash,
                       Drivers::TwiMaster& twiMaster,
                       Drivers::Hrs3300& heartRateSensor,
                       Drivers::Bma421& motionSensor,
                       Applications::HeartRateTask& heartRateApp,
                       Controllers::ButtonHandler& buttonHandler,
                       Applications::DisplayApp* displayApp)
  : spi {spi},
    spiNorFlash {spiNorFlash},
    twiMaster {twiMaster},
    heartRateSensor {heartRateSensor},
    motionSensor {motionSensor},
    heartRateApp(heartRateApp),
    buttonHandler {buttonHandler},
    nimbleController(*this,
                     displayApp->bleController,
                     displayApp->dateTimeController,
                     displayApp->notificationManager,
                     displayApp->batteryController,
                     spiNorFlash,
                     displayApp->heartRateController,
                     displayApp->motionController,
                     displayApp->filesystem) {
  displayApp->systemTask = this;
  this->displayApp = displayApp;
}

void SystemTask::Start() {
  systemTasksMsgQueue = xQueueCreate(10, 1);
  if (pdPASS != xTaskCreate(SystemTask::Process, "MAIN", 350, this, 1, &taskHandle)) {
    APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
  }
}

void SystemTask::Process(void* instance) {
  auto* app = static_cast<SystemTask*>(instance);
  // NRF_LOG_INFO("systemtask task started!");
  app->Work();
}

void SystemTask::Work() {
  BootErrors bootError = BootErrors::None;

  displayApp->watchdog.Setup(7, Drivers::Watchdog::SleepBehaviour::Run, Drivers::Watchdog::HaltBehaviour::Pause);
  displayApp->watchdog.Start();
  // NRF_LOG_INFO("Last reset reason : %s", Drivers::ResetReasonToString(watchdog.GetResetReason()));
  APP_GPIOTE_INIT(2);

  spi.Init();
  spiNorFlash.Init();
  spiNorFlash.Wakeup();

  displayApp->filesystem.Init(); // after spiNorFlash

  Controllers::AlarmController::Init();                        // after filesystem.Init()
  Controllers::TimerController::Init(DisplayAppCallbackTimer); // after filesystem.Init()

  nimbleController.Init();

  twiMaster.Init();
  /*
   * TODO We disable this warning message until we ensure it won't be displayed
   * on legitimate PineTime equipped with a compatible touch controller.
   * (some users reported false positive). See https://github.com/InfiniTimeOrg/InfiniTime/issues/763
  if (!touchPanel.Init()) {
    bootError = BootErrors::TouchController;
  }
   */
  displayApp->touchPanel.Init();
  displayApp->dateTimeController.Register(this);
  displayApp->batteryController.Register(this);
  motionSensor.SoftReset();

  // Reset the TWI device because the motion sensor chip most probably crashed it...
  twiMaster.Sleep();
  twiMaster.Init();

  motionSensor.Init();
  displayApp->motionController.Init(motionSensor.DeviceType());
  displayApp->settingsController.Init();

  displayApp->Start(bootError);

  heartRateSensor.Init();
  heartRateSensor.Disable();
  heartRateApp.Start();

  buttonHandler.Init(this);

  // Setup Interrupts
  nrfx_gpiote_in_config_t pinConfig;
  pinConfig.skip_gpio_setup = false;
  pinConfig.hi_accuracy = false;
  pinConfig.is_watcher = false;

  // Button
  nrf_gpio_cfg_output(PinMap::ButtonEnable);
  nrf_gpio_pin_set(PinMap::ButtonEnable);
  pinConfig.sense = NRF_GPIOTE_POLARITY_TOGGLE;
  pinConfig.pull = NRF_GPIO_PIN_PULLDOWN;
  nrfx_gpiote_in_init(PinMap::Button, &pinConfig, nrfx_gpiote_evt_handler);
  nrfx_gpiote_in_event_enable(PinMap::Button, true);

  // Touchscreen
  pinConfig.sense = NRF_GPIOTE_POLARITY_HITOLO;
  pinConfig.pull = NRF_GPIO_PIN_PULLUP;
  nrfx_gpiote_in_init(PinMap::Cst816sIrq, &pinConfig, nrfx_gpiote_evt_handler);
  nrfx_gpiote_in_event_enable(PinMap::Cst816sIrq, true);

  // Power present
  pinConfig.sense = NRF_GPIOTE_POLARITY_TOGGLE;
  pinConfig.pull = NRF_GPIO_PIN_NOPULL;
  nrfx_gpiote_in_init(PinMap::PowerPresent, &pinConfig, nrfx_gpiote_evt_handler);
  nrfx_gpiote_in_event_enable(PinMap::PowerPresent, true);

  displayApp->batteryController.MeasureVoltage();

  measureBatteryTimer = xTimerCreate("measureBattery", batteryMeasurementPeriod, pdTRUE, this, MeasureBatteryTimerCallback);
  xTimerStart(measureBatteryTimer, portMAX_DELAY);

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
  while (true) {
    UpdateMotion();

    Messages msg;
    if (xQueueReceive(systemTasksMsgQueue, &msg, 100) == pdTRUE) {
      switch (msg) {
        case Messages::EnableSleeping:
          // Make sure that exiting an app doesn't enable sleeping,
          // if the exiting was caused by a firmware update
          if (!displayApp->bleController.IsFirmwareUpdating()) {
            doNotGoToSleep = false;
          }
          displayApp->PushMessage(Applications::Display::Messages::RestoreBrightness);
          break;
        case Messages::DisableSleeping:
          doNotGoToSleep = true;
          break;
        case Messages::GoToRunning:
          spi.Wakeup();

          // Double Tap needs the touch screen to be in normal mode
          if (!displayApp->settingsController.isWakeUpModeOn(Controllers::Settings::WakeUpMode::DoubleTap)) {
            displayApp->touchPanel.Wakeup();
          }

          spiNorFlash.Wakeup();

          displayApp->PushMessage(Applications::Display::Messages::GoToRunning);
          heartRateApp.PushMessage(Applications::HeartRateTask::Messages::WakeUp);

          if (displayApp->bleController.IsRadioEnabled() && !displayApp->bleController.IsConnected()) {
            nimbleController.RestartFastAdv();
          }

          state = SystemTaskState::Running;
          break;
        case Messages::TouchWakeUp: {
          if (displayApp->touchHandler.ProcessTouchInfo(displayApp->touchPanel.GetTouchInfo())) {
            auto gesture = displayApp->touchHandler.GestureGet();
            if (displayApp->settingsController.GetNotificationStatus() != Controllers::Settings::Notification::Sleep &&
                gesture != Applications::TouchEvents::None &&
                ((gesture == Applications::TouchEvents::DoubleTap &&
                  displayApp->settingsController.isWakeUpModeOn(Controllers::Settings::WakeUpMode::DoubleTap)) ||
                 (gesture == Applications::TouchEvents::Tap &&
                  displayApp->settingsController.isWakeUpModeOn(Controllers::Settings::WakeUpMode::SingleTap)))) {
              GoToRunning();
            }
          }
          break;
        }
        case Messages::GoToSleep:
          if (doNotGoToSleep) {
            break;
          }
          state = SystemTaskState::GoingToSleep; // Already set in PushMessage()
                                                 // NRF_LOG_INFO("[systemtask] Going to sleep");
          displayApp->PushMessage(Applications::Display::Messages::GoToSleep);
          heartRateApp.PushMessage(Applications::HeartRateTask::Messages::GoToSleep);
          break;
        case Messages::OnNewTime:
          displayApp->PushMessage(Applications::Display::Messages::RestoreBrightness);
          displayApp->PushMessage(Applications::Display::Messages::UpdateDateTime);
          for (auto* alarmController : Controllers::AlarmController::alarmControllers) {
            if (alarmController->State() == Controllers::AlarmController::AlarmState::Set)
              alarmController->ScheduleAlarm();
          }
          break;
        case Messages::OnNewNotification:
          if (displayApp->settingsController.GetNotificationStatus() == Controllers::Settings::Notification::On) {
            if (state == SystemTaskState::Sleeping) {
              GoToRunning();
            } else {
              displayApp->PushMessage(Applications::Display::Messages::RestoreBrightness);
            }
            displayApp->PushMessage(Applications::Display::Messages::NewNotification);
          }
          break;
        case Messages::SetOffAlarm:
          if (state == SystemTaskState::Sleeping) {
            GoToRunning();
          }
          displayApp->PushMessage(Applications::Display::Messages::AlarmTriggered);
          break;
        case Messages::BleConnected:
          displayApp->PushMessage(Applications::Display::Messages::RestoreBrightness);
          isBleDiscoveryTimerRunning = true;
          bleDiscoveryTimer = 5;
          break;
        case Messages::BleFirmwareUpdateStarted:
          doNotGoToSleep = true;
          if (state == SystemTaskState::Sleeping) {
            GoToRunning();
          }
          displayApp->PushMessage(Applications::Display::Messages::BleFirmwareUpdateStarted);
          break;
        case Messages::BleFirmwareUpdateFinished:
          if (displayApp->bleController.State() == Controllers::Ble::FirmwareUpdateStates::Validated) {
            NVIC_SystemReset();
          }
          doNotGoToSleep = false;
          break;
        case Messages::StartFileTransfer:
          //  NRF_LOG_INFO("[systemtask] FS Started");
          doNotGoToSleep = true;
          if (state == SystemTaskState::Sleeping) {
            GoToRunning();
          }
          // TODO add intent of fs access icon or something
          break;
        case Messages::StopFileTransfer:
          //  NRF_LOG_INFO("[systemtask] FS Stopped");
          doNotGoToSleep = false;
          // TODO add intent of fs access icon or something
          break;
        case Messages::OnTouchEvent:
          if (displayApp->touchHandler.ProcessTouchInfo(displayApp->touchPanel.GetTouchInfo())) {
            displayApp->PushMessage(Applications::Display::Messages::TouchEvent);
          }
          break;
        case Messages::HandleButtonEvent: {
          Controllers::ButtonActions action = Controllers::ButtonActions::None;
          if (nrf_gpio_pin_read(PinMap::Button) == 0) {
            action = buttonHandler.HandleEvent(Controllers::ButtonHandler::Events::Release);
          } else {
            action = buttonHandler.HandleEvent(Controllers::ButtonHandler::Events::Press);
            // This is for faster wakeup, sacrificing special longpress and doubleclick handling while sleeping
            if (IsSleeping()) {
              fastWakeUpDone = true;
              GoToRunning();
              break;
            }
          }
          HandleButtonAction(action);
        } break;
        case Messages::HandleButtonTimerEvent: {
          auto action = buttonHandler.HandleEvent(Controllers::ButtonHandler::Events::Timer);
          HandleButtonAction(action);
        } break;
        case Messages::OnDisplayTaskSleeping:
          if (BootloaderVersion::IsValid()) {
            // First versions of the bootloader do not expose their version and cannot initialize the SPI NOR FLASH
            // if it's in sleep mode. Avoid bricked device by disabling sleep mode on these versions.
            spiNorFlash.Sleep();
          }
          spi.Sleep();

          // Double Tap needs the touch screen to be in normal mode
          if (!displayApp->settingsController.isWakeUpModeOn(Controllers::Settings::WakeUpMode::DoubleTap)) {
            displayApp->touchPanel.Sleep();
          }

          state = SystemTaskState::Sleeping;
          break;
        case Messages::OnNewDay:
          // We might be sleeping (with TWI device disabled.
          // Remember we'll have to reset the counter next time we're awake
          stepCounterMustBeReset = true;
          break;
        case Messages::OnChargingEvent:
          displayApp->batteryController.ReadPowerState();
          displayApp->PushMessage(Applications::Display::Messages::OnChargingEvent);
          if (state == SystemTaskState::Sleeping) {
            GoToRunning();
          }
          break;
        case Messages::MeasureBatteryTimerExpired:
          displayApp->batteryController.MeasureVoltage();
          break;
        case Messages::BatteryPercentageUpdated:
          nimbleController.NotifyBatteryLevel(displayApp->batteryController.PercentRemaining());
          break;
        case Messages::OnPairing:
          if (state == SystemTaskState::Sleeping) {
            GoToRunning();
          }
          displayApp->PushMessage(Applications::Display::Messages::ShowPairingKey);
          break;
        case Messages::BleRadioEnableToggle:
          if (displayApp->settingsController.GetBleRadioEnabled()) {
            nimbleController.EnableRadio();
          } else {
            nimbleController.DisableRadio();
          }
          break;
        default:
          break;
      }
    }

    if (isBleDiscoveryTimerRunning) {
      if (bleDiscoveryTimer == 0) {
        isBleDiscoveryTimerRunning = false;
        // Services discovery is deferred from 3 seconds to avoid the conflicts between the host communicating with the
        // target and vice-versa. I'm not sure if this is the right way to handle this...
        nimbleController.StartDiscovery();
      } else {
        bleDiscoveryTimer--;
      }
    }

    monitor.Process();
    uint32_t systick_counter = nrf_rtc_counter_get(portNRF_RTC_REG);
    displayApp->dateTimeController.UpdateTime(systick_counter);
    NoInit_BackUpTime = displayApp->dateTimeController.CurrentDateTime();
    if (nrf_gpio_pin_read(PinMap::Button) == 0) {
      displayApp->watchdog.Reload();
    }
  }
#pragma clang diagnostic pop
}

void SystemTask::UpdateMotion() {
  if (state == SystemTaskState::GoingToSleep || state == SystemTaskState::WakingUp) {
    return;
  }

  if (state == SystemTaskState::Sleeping &&
      !(displayApp->settingsController.isWakeUpModeOn(Controllers::Settings::WakeUpMode::RaiseWrist) ||
        displayApp->settingsController.isWakeUpModeOn(Controllers::Settings::WakeUpMode::Shake) ||
        displayApp->motionController.GetService()->IsMotionNotificationSubscribed())) {
    return;
  }

  if (stepCounterMustBeReset) {
    motionSensor.ResetStepCounter();
    stepCounterMustBeReset = false;
  }

  auto motionValues = motionSensor.Process();

  displayApp->motionController.Update(motionValues.x, motionValues.y, motionValues.z, motionValues.steps);

  if (displayApp->settingsController.GetNotificationStatus() != Controllers::Settings::Notification::Sleep) {
    if ((displayApp->settingsController.isWakeUpModeOn(Controllers::Settings::WakeUpMode::RaiseWrist) &&
         displayApp->motionController.ShouldRaiseWake()) ||
        (displayApp->settingsController.isWakeUpModeOn(Controllers::Settings::WakeUpMode::Shake) &&
         displayApp->motionController.ShouldShakeWake(displayApp->settingsController.GetShakeThreshold()))) {
      GoToRunning();
    }
  }
  if (displayApp->settingsController.isWakeUpModeOn(Controllers::Settings::WakeUpMode::LowerWrist) && state == SystemTaskState::Running &&
      displayApp->motionController.ShouldLowerSleep()) {
    PushMessage(Messages::GoToSleep);
  }
}

void SystemTask::HandleButtonAction(Controllers::ButtonActions action) {
  if (IsSleeping()) {
    return;
  }

  displayApp->PushMessage(Applications::Display::Messages::RestoreBrightness);

  using Actions = Controllers::ButtonActions;

  switch (action) {
    case Actions::Click:
      // If the first action after fast wakeup is a click, it should be ignored.
      if (!fastWakeUpDone && state != SystemTaskState::GoingToSleep) {
        displayApp->PushMessage(Applications::Display::Messages::ButtonPushed);
      }
      break;
    case Actions::DoubleClick:
      displayApp->PushMessage(Applications::Display::Messages::ButtonDoubleClicked);
      break;
    case Actions::LongPress:
      displayApp->PushMessage(Applications::Display::Messages::ButtonLongPressed);
      break;
    case Actions::LongerPress:
      displayApp->PushMessage(Applications::Display::Messages::ButtonLongerPressed);
      break;
    default:
      return;
  }

  fastWakeUpDone = false;
}

void SystemTask::GoToRunning() {
  if (state == SystemTaskState::Sleeping) {
    state = SystemTaskState::WakingUp;
    PushMessage(Messages::GoToRunning);
  }
}

void SystemTask::OnTouchEvent() {
  if (state == SystemTaskState::Running) {
    PushMessage(Messages::OnTouchEvent);
  } else if (state == SystemTaskState::Sleeping) {
    if (displayApp->settingsController.isWakeUpModeOn(Controllers::Settings::WakeUpMode::SingleTap) or
        displayApp->settingsController.isWakeUpModeOn(Controllers::Settings::WakeUpMode::DoubleTap)) {
      PushMessage(Messages::TouchWakeUp);
    }
  }
}

void SystemTask::PushMessage(System::Messages msg) {
  if (msg == Messages::GoToSleep && !doNotGoToSleep) {
    state = SystemTaskState::GoingToSleep;
  }
  if (in_isr()) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(systemTasksMsgQueue, &msg, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken == pdTRUE) {
      /* Actual macro used here is port specific. */
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
  } else {
    xQueueSend(systemTasksMsgQueue, &msg, portMAX_DELAY);
  }
}
