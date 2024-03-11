#include "displayapp/DisplayApp.h"
#include "displayapp/screens/HeartRate.h"
#include "displayapp/screens/Motion.h"
#include "displayapp/screens/Alarm.h"
#include "displayapp/screens/Timer.h"
#include "displayapp/screens/AlarmSet.h"
#include "displayapp/screens/TimerSet.h"
#include "displayapp/screens/ApplicationList.h"
#include "displayapp/screens/FirmwareUpdate.h"
#include "displayapp/screens/FirmwareValidation.h"
#include "displayapp/screens/StopWatch.h"
#include "displayapp/screens/Music.h"
#include "displayapp/screens/Navigation.h"
#include "displayapp/screens/Notifications.h"
#include "displayapp/screens/SystemInfo.h"
#include "displayapp/screens/Tile.h"
#include "displayapp/screens/FlashLight.h"
#include "displayapp/screens/BatteryInfo.h"
#include "displayapp/screens/Steps.h"
#include "displayapp/screens/PassKey.h"
#include "displayapp/screens/Error.h"

#include "displayapp/screens/settings/QuickSettings.h"
#include "displayapp/screens/settings/Settings.h"
#include "displayapp/screens/settings/SettingWatchFace.h"
#include "displayapp/screens/settings/SettingTimeFormat.h"
#include "displayapp/screens/settings/SettingWeatherFormat.h"
#include "displayapp/screens/settings/SettingWakeUp.h"
#include "displayapp/screens/settings/SettingDisplay.h"
#include "displayapp/screens/settings/SettingSteps.h"
#include "displayapp/screens/settings/SettingSetDateTime.h"
#include "displayapp/screens/settings/SettingShakeThreshold.h"
#include "displayapp/screens/settings/SettingBluetooth.h"

#include "displayapp/screens/WatchFaceDigital.h"
#include "displayapp/screens/WatchFaceAnalog.h"
#include "displayapp/screens/WatchFaceCasioStyleG7710.h"
#include "displayapp/screens/WatchFaceTerminal.h"

#include "displayapp/screens/Weather.h"

#include "UserApps.h"

// #define Log

#ifdef Log
  #include <nrf_log.h>
#endif

using namespace Pinetime::Applications;
using namespace Pinetime::Applications::Display;

namespace {
  inline bool in_isr() {
    return (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0;
  }
}

DisplayApp::DisplayApp(Drivers::St7789& lcd,
                       Drivers::Cst816S& touchPanel,
                       Controllers::Battery& batteryController,
                       Controllers::Ble& bleController,
                       Controllers::DateTime& dateTimeController,
                       Drivers::Watchdog& watchdog,
                       Controllers::NotificationManager& notificationManager,
                       Controllers::HeartRateController& heartRateController,
                       Controllers::Settings& settingsController,
                       Controllers::MotorController& motorController,
                       Controllers::MotionController& motionController,
                       Controllers::BrightnessController& brightnessController,
                       Controllers::TouchHandler& touchHandler,
                       Controllers::FS& filesystem)
  : lcd {lcd},
    touchPanel {touchPanel},
    batteryController {batteryController},
    bleController {bleController},
    dateTimeController {dateTimeController},
    watchdog {watchdog},
    notificationManager {notificationManager},
    heartRateController {heartRateController},
    settingsController {settingsController},
    motorController {motorController},
    motionController {motionController},
    brightnessController {brightnessController},
    touchHandler {touchHandler},
    filesystem {filesystem},
    lvgl {lcd, filesystem} {
}

void DisplayApp::Start(System::BootErrors error) {
  msgQueue = xQueueCreate(queueSize, itemSize);
  bootError = error;
  lvgl.Init();
  if (error == System::BootErrors::TouchController) {
    loadNewScreen(Apps::Error, Screen::FullRefreshDirections::None);
  } else {
    loadNewScreen(Apps::Clock, Screen::FullRefreshDirections::None);
  }

  if (pdPASS != xTaskCreate(DisplayApp::Process, "displayapp", 800, this, 0, &taskHandle)) {
    APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
  }
}

void DisplayApp::InitHw() {
  brightnessController.Init();
  ApplyBrightness();
  motorController.Init();
  lcd.Init();
}

void DisplayApp::Process(void* instance) {
  auto* app = static_cast<DisplayApp*>(instance);
#ifdef Log
  NRF_LOG_INFO("DisplayApp::Process task started!");
#endif
  app->InitHw();
  // Send a dummy notification to unlock the lvgl display driver for the first iteration
  xTaskNotifyGive(xTaskGetCurrentTaskHandle());
  while (true) {
    app->refresh();
  }
}

void DisplayApp::refresh() {
  auto loadPreviousScreen = [this]() {
    Screen::FullRefreshDirections returnDirection;
    if (screenStack.Empty()) returnDirection = Screen::FullRefreshDirections::None;
   else switch (screenStack.Top()->direction) {
      case Screen::FullRefreshDirections::Up:
        returnDirection = Screen::FullRefreshDirections::Down;
        break;
      case Screen::FullRefreshDirections::Down:
        returnDirection = Screen::FullRefreshDirections::Up;
        break;
      case Screen::FullRefreshDirections::LeftAnim:
        returnDirection = Screen::FullRefreshDirections::RightAnim;
        break;
      case Screen::FullRefreshDirections::RightAnim:
        returnDirection = Screen::FullRefreshDirections::LeftAnim;
        break;
      default:
        returnDirection = Screen::FullRefreshDirections::None;
        break;
    }
    loadScreen(screenStack.Pop(), returnDirection);
  };

  auto DimScreen = [this]() {
    if (brightnessController.Level() != Controllers::BrightnessController::Levels::Off) {
      isDimmed = true;
      brightnessController.Set(Controllers::BrightnessController::Levels::Low);
    }
  };

  auto RestoreBrightness = [this]() {
    if (brightnessController.Level() != Controllers::BrightnessController::Levels::Off) {
      isDimmed = false;
      lv_disp_trig_activity(NULL);
      ApplyBrightness();
    }
  };

  auto IsPastDimTime = [this]() -> bool {
    return lv_disp_get_inactive_time(NULL) >= pdMS_TO_TICKS(settingsController.GetScreenTimeOut() - 2000);
  };

  auto IsPastSleepTime = [this]() -> bool {
    return lv_disp_get_inactive_time(NULL) >= pdMS_TO_TICKS(settingsController.GetScreenTimeOut());
  };

  TickType_t queueTimeout;
  switch (state) {
    case States::Idle:
      queueTimeout = portMAX_DELAY;
      break;
    case States::Running:
      if (!currentScreen || !currentScreen->IsRunning()) {
        loadPreviousScreen();
      }
      queueTimeout = lv_task_handler();

      if (!systemTask->IsSleepDisabled() && IsPastDimTime()) {
        if (!isDimmed) {
          DimScreen();
        }
        if (IsPastSleepTime()) {
          systemTask->PushMessage(System::Messages::GoToSleep);
          state = States::Idle;
        }
      } else if (isDimmed) {
        RestoreBrightness();
      }
      break;
    default:
      queueTimeout = portMAX_DELAY;
      break;
  }

  Messages msg;
  if (xQueueReceive(msgQueue, &msg, queueTimeout) == pdTRUE) {
#ifdef Log
    // NRF_LOG_INFO("DisplayApp::xQueueReceive Id=%d, data=%d", msg.Id, msg.data);
#endif
    switch (msg) {
      case Messages::DimScreen:
        DimScreen();
        break;
      case Messages::RestoreBrightness:
        RestoreBrightness();
        break;
      case Messages::GoToSleep:
        while (brightnessController.Level() != Controllers::BrightnessController::Levels::Off) {
          brightnessController.Lower();
          vTaskDelay(100);
        }
        lcd.Sleep();
        PushMessageToSystemTask(System::Messages::OnDisplayTaskSleeping);
        state = States::Idle;
        break;
      case Messages::GoToRunning:
        lcd.Wakeup();
        lv_disp_trig_activity(NULL);
        ApplyBrightness();
        state = States::Running;
        break;
      case Messages::UpdateBleConnection:
        //        clockScreen.SetBleConnectionState(bleController.IsConnected() ? Screens::Clock::BleConnectionStates::Connected :
        //        Screens::Clock::BleConnectionStates::NotConnected);
        break;
      case Messages::NewNotification:
        loadNewScreen(Apps::NotificationsPreview, Screen::FullRefreshDirections::Down);
        break;
      case Messages::TimerDone: {
        if (state != States::Running)
          PushMessageToSystemTask(System::Messages::GoToRunning);
        // motorController.StartRinging();
        motorController.RunForDuration(255);
        if (currentScreen->Id == Apps::TimerSet) {
          lv_disp_trig_activity(NULL);
          (static_cast<Screens::TimerSet*>(currentScreen))->TimerDone();
        } else
          loadNewScreen(new TimerSet(), Screen::FullRefreshDirections::Up);
      } break;
      case Messages::AlarmTriggered:
        systemTask->PushMessage(System::Messages::DisableSleeping);
        motorController.StartRinging();
        if (currentScreen->Id == Apps::AlarmSet) {
          auto* alarm = static_cast<AlarmSet*>(currentScreen);
          if (alarm->OnAlertingState()) {
            alarm->SetAlerting();
            break;
          }
        }
        loadNewScreen(new AlarmSet(true), Screen::FullRefreshDirections::None);
        break;
      case Messages::ShowPairingKey:
        loadNewScreen(Apps::PassKey, Screen::FullRefreshDirections::Up);
        motorController.RunForDuration(35);
        break;
      case Messages::TouchEvent: {
        if (state != States::Running) {
          break;
        }
        lvgl.SetNewTouchPoint(touchHandler.GetX(), touchHandler.GetY(), touchHandler.IsTouching());
        auto gesture = touchHandler.GestureGet();
        if (gesture == TouchEvents::None) {
          break;
        }
        auto LoadDirToReturnSwipe = [](Screen::FullRefreshDirections refreshDirection) {
          switch (refreshDirection) {
            default:
            case Screen::FullRefreshDirections::Up:
              return TouchEvents::SwipeDown;
            case Screen::FullRefreshDirections::Down:
              return TouchEvents::SwipeUp;
            case Screen::FullRefreshDirections::LeftAnim:
              return TouchEvents::SwipeRight;
            case Screen::FullRefreshDirections::RightAnim:
              return TouchEvents::SwipeLeft;
          }
        };
        if (!currentScreen->OnTouchEvent(gesture)) {
          if (currentScreen->Id == Apps::Clock) {
            switch (gesture) {
              case TouchEvents::SwipeUp:
                loadNewScreen(Apps::Launcher, Screen::FullRefreshDirections::Up);
                break;
              case TouchEvents::SwipeDown:
                loadNewScreen(Apps::Notifications, Screen::FullRefreshDirections::Down);
                break;
              case TouchEvents::SwipeRight:
                loadNewScreen(Apps::QuickSettings, Screen::FullRefreshDirections::RightAnim);
                break;
              case TouchEvents::DoubleTap:
                PushMessageToSystemTask(System::Messages::GoToSleep);
                break;
              default:
                break;
            }
          } else if (!screenStack.Empty() && gesture == LoadDirToReturnSwipe(screenStack.Top()->direction)) {
            loadPreviousScreen();
          }
        } else {
          lvgl.CancelTap();
        }
      } break;
      case Messages::ButtonPushed:
        motorController.StopRinging();
        if (!currentScreen->OnButtonPushed()) {
          if (currentScreen->Id == Apps::Clock) {
            PushMessageToSystemTask(System::Messages::GoToSleep);
          } else {
            loadPreviousScreen();
          }
        }
        break;
      case Messages::ButtonLongPressed:
        screenStack.Reset();
        if (currentScreen->Id != Apps::Clock) {
          if (currentScreen->Id == Apps::Notifications) {
            loadNewScreen(Apps::Clock, Screen::FullRefreshDirections::Up);
          } else if (currentScreen->Id == Apps::QuickSettings) {
            loadNewScreen(Apps::Clock, Screen::FullRefreshDirections::LeftAnim);
          } else {
            loadNewScreen(Apps::Clock, Screen::FullRefreshDirections::Down);
          }          
        }
        break;
      case Messages::ButtonLongerPressed:
        // Create reboot app and open it instead
        // LoadScreen(Apps::SysInfo, Screen::FullRefreshDirections::Up);
        break;
      case Messages::ButtonDoubleClicked:
        // if (currentApp != Apps::Weather && controllers.weatherController->has_value()) {
        if (currentScreen->Id != Apps::Weather) {
          loadNewScreen(new Screens::Weather(), Screen::FullRefreshDirections::Down);
        } else if (currentScreen->Id != Apps::Notifications && currentScreen->Id != Apps::NotificationsPreview) {
          loadNewScreen(Apps::Notifications, Screen::FullRefreshDirections::Down);
        }
        break;

      case Messages::BleFirmwareUpdateStarted:
        loadNewScreen(Apps::FirmwareUpdate, Screen::FullRefreshDirections::Down);
        break;
      case Messages::BleRadioEnableToggle:
        PushMessageToSystemTask(System::Messages::BleRadioEnableToggle);
        break;
      case Messages::UpdateDateTime:
        // Added to remove warning
        // What should happen here?
        break;
      case Messages::OnChargingEvent:
        RestoreBrightness();
        motorController.RunForDuration(15);
        break;
    }
  }

  if (touchHandler.IsTouching() && currentScreen) {
    currentScreen->OnTouchEvent(touchHandler.GetX(), touchHandler.GetY());
  }

  if (nextApp != Apps::None) {
    loadNewScreen(nextApp, nextDirection);
    nextApp = Apps::None;
  } else {
    Screens::Screen* screen = nextScreen;
    if (screen) {
      nextScreen = NULL;
      loadNewScreen(screen, screen->direction);
    }
  }
}

// use StartApp(Screens::Screen* screen, Screen::FullRefreshDirections direction) instead if possible
void DisplayApp::StartApp(Apps app, Screen::FullRefreshDirections direction) {
  nextApp = app;
  nextDirection = direction;
}

void DisplayApp::StartApp(Screens::Screen* screen, Screen::FullRefreshDirections direction) {
  screen->direction = direction;
  nextScreen = screen;  
}

void DisplayApp::loadScreen(Screens::Screen* screen, Screen::FullRefreshDirections direction) {
  lvgl.CancelTap();
  lv_disp_trig_activity(NULL);
  if (currentScreen) { 
    delete currentScreen;
    currentScreen = NULL;
  }
  SetFullRefresh(direction);
  currentScreen = screen;
  screen->Load();
}

void DisplayApp::loadNewScreen(Screens::Screen* screen, Screen::FullRefreshDirections direction) {
  // Don't add the same screen to the stack back to back.
  // This is mainly to fix an issue with receiving two notifications at the same time and shouldn't happen otherwise.
  if (currentScreen && screen->Id != currentScreen->Id) {
    if (currentScreen->UnLoad()) {
      if (screenStack.Push(currentScreen)) {
        currentScreen->direction = direction;
        currentScreen = NULL; // do not delete stored screen       
      }
    }
  } 
  loadScreen(screen, direction);
}

void DisplayApp::loadNewScreen(Apps app, Screen::FullRefreshDirections direction) {
  lvgl.CancelTap();
  lv_disp_trig_activity(NULL);
  Screens::Screen* screen;

  switch (app) {
    case Apps::Launcher: {
      std::array<Screens::Tile::Applications, UserAppTypes::Count> apps;
      int i = 0;
      for (const auto& userApp : userApps) {
        apps[i++] = Screens::Tile::Applications {userApp.icon, userApp.app, true};
      }
      screen = new Screens::ApplicationList(std::move(apps));
    } break;
    case Apps::Clock: {     
      screenStack.DeleteAll(Apps::Clock);
      const auto* watchFace =
        std::find_if(userWatchFaces.begin(), userWatchFaces.end(), [this](const WatchFaceDescription& watchfaceDescription) {
          return watchfaceDescription.watchFace == settingsController.GetWatchFace();
        });
      if (watchFace != userWatchFaces.end())
        screen = watchFace->create();
      else {
        screen = new WatchFaceDigital();
      }       
    } break;
    case Apps::Error:
      screen = new Screens::Error(bootError);
      break;
    case Apps::FirmwareValidation:
      screen = new Screens::FirmwareValidation(validator);
      break;
    case Apps::FirmwareUpdate:
      screen = new Screens::FirmwareUpdate(bleController);
      break;
    case Apps::PassKey:
      screen = new Screens::PassKey(bleController.GetPairingKey());
      break;
    case Apps::Notifications:
      screen = new Screens::Notifications(systemTask->nimble().alertService(), Apps::Notifications);
      break;
    case Apps::NotificationsPreview:
      screen = new Screens::Notifications(systemTask->nimble().alertService(), Apps::NotificationsPreview);
      break;
    case Apps::QuickSettings:
      screen = new Screens::QuickSettings();
      break;
    case Apps::Settings:
      screen = new Screens::Settings();
      break;
    case Apps::SettingWatchFace: {
      std::array<Screens::SettingWatchFace::Item, UserWatchFaceTypes::Count> items;
      int i = 0;
      for (const auto& userWatchFace : userWatchFaces) {
        items[i++] =
          Screens::SettingWatchFace::Item {userWatchFace.name, userWatchFace.watchFace, userWatchFace.isAvailable(filesystem)};
      }
      screen = new Screens::SettingWatchFace(std::move(items));
    } break;
    case Apps::SettingTimeFormat:
      screen = new Screens::SettingTimeFormat();
      break;
    case Apps::SettingWeatherFormat:
      screen = new Screens::SettingWeatherFormat();
      break;
    case Apps::SettingWakeUp:
      screen = new Screens::SettingWakeUp();
      break;
    case Apps::SettingDisplay:
      screen = new Screens::SettingDisplay();
      break;
    case Apps::SettingSteps:
      screen = new Screens::SettingSteps();
      break;
    case Apps::SettingSetDateTime:
      screen = new Screens::SettingSetDateTime();
      break;
    case Apps::SettingShakeThreshold:
      screen = new SettingShakeThreshold();
      break;
    case Apps::SettingBluetooth:
      screen = new Screens::SettingBluetooth();
      break;
    case Apps::BatteryInfo:
      screen = new Screens::BatteryInfo();
      break;
    case Apps::SysInfo:
      screen = new Screens::SystemInfo();
      break;
    case Apps::FlashLight:
      screen = new Screens::FlashLight();
      break;
    default: {
      const auto* d = std::find_if(userApps.begin(), userApps.end(), [app](const AppDescription& appDescription) {
        return appDescription.app == app;
      });
      if (d != userApps.end()) {
        screen = d->create();
      } else {
        screen = new WatchFaceDigital();
      }
      break;
    }
  }
  loadNewScreen(screen, direction);
}

void DisplayApp::PushMessage(Messages msg) {
#ifdef Log
  //  NRF_LOG_INFO("DisplayApp::PushMessage Id=%d, data=%d", id, data);
#endif
  if (in_isr()) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(msgQueue, &msg, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken == pdTRUE) {
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
  } else {
    TickType_t timeout = portMAX_DELAY;
    // Make xQueueSend() non-blocking if the message is a Notification message. We do this to avoid
    // deadlock between SystemTask and DisplayApp when their respective message queues are getting full
    // when a lot of notifications are received on a very short time span.
    if (msg == Messages::NewNotification) {
      timeout = static_cast<TickType_t>(0);
    }
    xQueueSend(msgQueue, &msg, timeout);
  }
}

void DisplayApp::SetFullRefresh(Screen::FullRefreshDirections direction) {
  switch (direction) {
    case Screen::FullRefreshDirections::Down:
      lvgl.SetFullRefresh(Components::LittleVgl::FullRefreshDirections::Down);
      break;
    case Screen::FullRefreshDirections::Up:
      lvgl.SetFullRefresh(Components::LittleVgl::FullRefreshDirections::Up);
      break;
    case Screen::FullRefreshDirections::Left:
      lvgl.SetFullRefresh(Components::LittleVgl::FullRefreshDirections::Left);
      break;
    case Screen::FullRefreshDirections::Right:
      lvgl.SetFullRefresh(Components::LittleVgl::FullRefreshDirections::Right);
      break;
    case Screen::FullRefreshDirections::LeftAnim:
      lvgl.SetFullRefresh(Components::LittleVgl::FullRefreshDirections::LeftAnim);
      break;
    case Screen::FullRefreshDirections::RightAnim:
      lvgl.SetFullRefresh(Components::LittleVgl::FullRefreshDirections::RightAnim);
      break;
    default:
      break;
  }
}

void DisplayApp::PushMessageToSystemTask(System::Messages id) {
   systemTask->PushMessage(id);
}

void DisplayApp::ApplyBrightness() {
  auto brightness = settingsController.GetBrightness();
  if (brightness != Controllers::BrightnessController::Levels::Low && brightness != Controllers::BrightnessController::Levels::Medium &&
      brightness != Controllers::BrightnessController::Levels::High) {
    brightness = Controllers::BrightnessController::Levels::High;
  }
  brightnessController.Set(brightness);
}
