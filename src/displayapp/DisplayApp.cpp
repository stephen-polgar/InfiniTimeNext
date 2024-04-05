#include "UserApps.h" // Must be first include

#include "displayapp/DisplayApp.h"
#include "displayapp/screens/WatchFaceScreen.h"
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
#include "displayapp/screens/AlarmSet.h"
#include "displayapp/screens/TimerSet.h"

#include "displayapp/screens/ApplicationList.h"
#include "displayapp/screens/FirmwareUpdate.h"
#include "displayapp/screens/FirmwareValidation.h"
#include "displayapp/screens/Notifications.h"
#include "displayapp/screens/SystemInfo.h"
#include "displayapp/screens/Tile.h"
#include "displayapp/screens/FlashLight.h"
#include "displayapp/screens/BatteryInfo.h"
#include "displayapp/screens/PassKey.h"
#include "displayapp/screens/Error.h"

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

DisplayApp::DisplayApp(Drivers::St7789& lcd, Drivers::Cst816S& touchPanel, Drivers::SpiNorFlash& spiNorFlash)
  : lcd {lcd}, touchPanel {touchPanel}, filesystem {spiNorFlash}, settingsController {filesystem}, lvgl {lcd, filesystem} {
}

void DisplayApp::Start(System::BootErrors error) {
  msgQueue = xQueueCreate(queueSize, itemSize);
  bootError = error;
  lvgl.Init();
  if (error == System::BootErrors::TouchController) {
    loadScreen(Apps::Error, Screen::FullRefreshDirections::None);
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
    if (screenStack.Empty())
      loadScreen(Apps::Clock, Screen::FullRefreshDirections::None);
    else {
      switch (screenStack.Top()->direction) {
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
      loadScreen(screenStack.Pop(), returnDirection, false);
    }
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
    //  NRF_LOG_INFO("DisplayApp msg=%d", msg);
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
        systemTask->PushMessage(System::Messages::OnDisplayTaskSleeping);
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
        if (currentScreen->Id == Apps::NotificationsPreview || currentScreen->Id == Apps::Notifications) {
          currentScreen->UnLoad();
          currentScreen->Id = Apps::NotificationsPreview;
          currentScreen->Load();
        } else {
          Screen* screen = screenStack.Get(Apps::NotificationsPreview);
          if (!screen)
            screen = screenStack.Get(Apps::Notifications);
          if (screen)
            screen->Id = Apps::NotificationsPreview;
          else
            screen = new Notifications(Apps::NotificationsPreview);
          loadScreen(screen, Screen::FullRefreshDirections::Down);
        }
        break;
      case Messages::TimerDone: {
        if (state != States::Running)
          systemTask->PushMessage(System::Messages::GoToRunning);
        // motorController.StartRinging();
        motorController.RunForDuration(255);
        if (currentScreen->Id == Apps::TimerSet) {
          lv_disp_trig_activity(NULL);
          (static_cast<Screens::TimerSet*>(currentScreen))->TimerDone();
        } else
          loadScreen(new TimerSet(), Screen::FullRefreshDirections::Up);
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
        loadScreen(new AlarmSet(true), Screen::FullRefreshDirections::None);
        break;
      case Messages::ShowPairingKey:
        loadScreen(Apps::PassKey, Screen::FullRefreshDirections::Up);
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
          if (!screenStack.Empty() && gesture == LoadDirToReturnSwipe(screenStack.Top()->direction)) {
            loadPreviousScreen();
          }
        } else {
          lvgl.CancelTap();
        }
      } break;
      case Messages::ButtonPushed:
        motorController.StopRinging();
        if (!currentScreen->OnButtonPushed()) {
          loadPreviousScreen();
        }
        break;
      case Messages::ButtonLongPressed:
        screenStack.Reset();
        if (currentScreen->Id != Apps::Clock) {
          if (currentScreen->Id == Apps::Notifications) {
            loadScreen(Apps::Clock, Screen::FullRefreshDirections::Up);
          } else if (currentScreen->Id == Apps::QuickSettings) {
            loadScreen(Apps::Clock, Screen::FullRefreshDirections::LeftAnim);
          } else {
            loadScreen(Apps::Clock, Screen::FullRefreshDirections::Down);
          }
        }
        break;
      case Messages::ButtonLongerPressed:
        // Create reboot app and open it instead
        // LoadScreen(Apps::SysInfo, Screen::FullRefreshDirections::Up);
        break;
      case Messages::ButtonDoubleClicked:
        if (currentScreen->Id != Apps::Weather && systemTask->nimbleController.weatherService.Current().has_value()) {
          loadScreen(Apps::Weather, Screen::FullRefreshDirections::Down);
        } else if (currentScreen->Id != Apps::Notifications) {
          loadScreen(Apps::Notifications, Screen::FullRefreshDirections::Down);
        }
        break;
      case Messages::BleFirmwareUpdateStarted:
        loadScreen(Apps::FirmwareUpdate, Screen::FullRefreshDirections::Down);
        break;
      case Messages::BleRadioEnableToggle:
        systemTask->PushMessage(System::Messages::BleRadioEnableToggle);
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
    loadScreen(nextApp, nextDirection);
    nextApp = Apps::None;
  } else {
    Screen* screen = nextScreen;
    if (screen) {
      nextScreen = NULL;
      loadScreen(screen, screen->direction);
    }
  }
}

void DisplayApp::StartApp(Apps app, Screen::FullRefreshDirections direction) {
  nextApp = app;
  nextDirection = direction;
}

void DisplayApp::StartApp(Screen* screen, Screen::FullRefreshDirections direction) {
  screen->direction = direction;
  nextScreen = screen;
}

void DisplayApp::loadScreen(Screen* screen, Screen::FullRefreshDirections direction, bool store) {
#ifdef Log
  NRF_LOG_INFO("DisplayApp:loadScreen screen=%d", uint8_t(screen->Id));
#endif
  lvgl.CancelTap();
  lv_disp_trig_activity(NULL);
  if (currentScreen) {
    if (currentScreen != screen) {
      if (currentScreen->UnLoad() && store) {
        screenStack.Push(currentScreen);
        currentScreen->direction = direction;
      } else
        delete currentScreen;
    } else
      currentScreen->UnLoad();
  }
  currentScreen = screen;
  SetFullRefresh(direction);
  screen->Load();
}

void DisplayApp::loadScreen(Apps app, Screen::FullRefreshDirections direction) {
#ifdef Log
  NRF_LOG_INFO("DisplayApp:loadScreen app=%d", uint8_t(app));
#endif
  lvgl.CancelTap();
  lv_disp_trig_activity(NULL);
  if (currentScreen && currentScreen->Id == app) {
    loadScreen(currentScreen, direction, false);
    return;
  }
  Screen* screen = (app != Apps::None) ? screenStack.Get(app) : NULL;
  if (!screen)
    switch (app) {
      case Apps::Launcher: {
        std::array<Tile::Applications, UserAppTypes::Count> apps;
        int i = 0;
        for (const auto& userApp : userApps) {
          apps[i++] = Tile::Applications {userApp.icon, userApp.app, true};
        }
        screen = new ApplicationList(std::move(apps));
      } break;
      case Apps::Clock:
        screen = new WatchFaceScreen();
        break;
      case Apps::Error:
        screen = new Error(bootError);
        break;
      case Apps::FirmwareValidation:
        screen = new FirmwareValidation(validator);
        break;
      case Apps::FirmwareUpdate:
        screen = new FirmwareUpdate(bleController);
        break;
      case Apps::PassKey:
        screen = new PassKey(bleController.GetPairingKey());
        break;
      case Apps::NotificationsPreview:
      case Apps::Notifications:
        screen = new Notifications(app);
        break;
      case Apps::QuickSettings:
        screen = new QuickSettings();
        break;
      case Apps::Settings:
        screen = new Settings();
        break;
      case Apps::SettingWatchFace: {
        std::array<SettingWatchFace::Item, UserWatchFaceTypes::Count> items;
        uint8_t i = 0;
        for (const auto& userWatchFace : userWatchFaces) {
          items[i++] = SettingWatchFace::Item {userWatchFace.name, userWatchFace.watchFace, userWatchFace.isAvailable(filesystem)};
        }
        screen = new SettingWatchFace(std::move(items));
      } break;
      case Apps::SettingTimeFormat:
        screen = new SettingTimeFormat();
        break;
      case Apps::SettingWeatherFormat:
        screen = new SettingWeatherFormat();
        break;
      case Apps::SettingWakeUp:
        screen = new SettingWakeUp();
        break;
      case Apps::SettingDisplay:
        screen = new SettingDisplay();
        break;
      case Apps::SettingSteps:
        screen = new SettingSteps();
        break;
      case Apps::SettingSetDateTime:
        screen = new SettingSetDateTime();
        break;
      case Apps::SettingShakeThreshold:
        screen = new SettingShakeThreshold();
        break;
      case Apps::SettingBluetooth:
        screen = new SettingBluetooth();
        break;
      case Apps::BatteryInfo:
        screen = new BatteryInfo();
        break;
      case Apps::SysInfo:
        screen = new SystemInfo();
        break;
      case Apps::FlashLight:
        screen = new FlashLight();
        break;
      default: {
        const auto* d = std::find_if(userApps.begin(), userApps.end(), [app](const AppDescription& appDescription) {
          return appDescription.app == app;
        });
        if (d != userApps.end()) {
          screen = d->create();
        } else {
          screen = new WatchFaceScreen();
        }
        break;
      }
    }
  loadScreen(screen, direction);
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

Screen* DisplayApp::GetSelectedWatchFace() {
  const WatchFace selected = settingsController.GetWatchFace();
  for (WatchFaceDescription watchFaceDescription : userWatchFaces) {
    if (watchFaceDescription.watchFace == selected)
      return watchFaceDescription.create();
  }
  return NULL;
}

void DisplayApp::ApplyBrightness() {
  auto brightness = settingsController.GetBrightness();
  if (brightness != Controllers::BrightnessController::Levels::Low && brightness != Controllers::BrightnessController::Levels::Medium &&
      brightness != Controllers::BrightnessController::Levels::High) {
    brightness = Controllers::BrightnessController::Levels::High;
  }
  brightnessController.Set(brightness);
}
