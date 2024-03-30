#pragma once

#include <FreeRTOS.h>
#include <queue.h>
#include "BootErrors.h"
#include "displayapp/screens/Screen.h"
#include "drivers/Cst816s.h"
#include "drivers/St7789.h"
#include "drivers/Watchdog.h"
#include "systemtask/Messages.h"
#include "displayapp/Messages.h"
#include "displayapp/LittleVgl.h"

#include "components/heartrate/HeartRateController.h"
#include "components/settings/Settings.h"
#include "components/battery/BatteryController.h"
#include "components/ble/BleController.h"
#include "components/datetime/DateTimeController.h"
#include "components/ble/NotificationManager.h"
#include "components/motion/MotionController.h"
#include "components/motor/MotorController.h"
#include "components/brightness/BrightnessController.h"
#include "components/firmwarevalidator/FirmwareValidator.h"
#include "touchhandler/TouchHandler.h"

#include "utility/ScreenStack.h"

namespace Pinetime {

  namespace System {
    class SystemTask;
  };

  namespace Applications {
    class DisplayApp {
    public:
      enum class States : uint8_t { Idle, Running };

      DisplayApp(Drivers::St7789& lcd,
                 Drivers::Cst816S& touchPanel,                                            
                  Drivers::SpiNorFlash& spiNorFlash);

      void Start(System::BootErrors error);
      void PushMessage(Display::Messages id);

      void StartApp(Apps app, Screen::FullRefreshDirections direction = Screen::FullRefreshDirections::Up);
      void StartApp(Screen* screen, Screen::FullRefreshDirections direction = Screen::FullRefreshDirections::Up);
      void SetFullRefresh(Screen::FullRefreshDirections direction);
      Screen * GetSelectedWatchFace();

      Controllers::DateTime dateTimeController;
      System::SystemTask* systemTask;
      Controllers::NotificationManager notificationManager;
      Controllers::HeartRateController heartRateController;
      Controllers::Settings settingsController;
      Controllers::MotorController motorController;
      Controllers::MotionController motionController;
      Controllers::BrightnessController brightnessController;
      Controllers::TouchHandler touchHandler;
      Controllers::FS filesystem;
      Drivers::Watchdog watchdog;
      Drivers::Cst816S& touchPanel;
      Controllers::Battery batteryController;
      Controllers::Ble bleController;
      Controllers::FirmwareValidator validator;
      Components::LittleVgl lvgl;

    private:
      Drivers::St7789& lcd;

      TaskHandle_t taskHandle;

      States state = States::Running;
      QueueHandle_t msgQueue;

      static constexpr uint8_t queueSize = 10;
      static constexpr uint8_t itemSize = 1;

      Screen *currentScreen = NULL, *nextScreen = NULL;
      Utility::ScreenStack<10> screenStack;
      Apps nextApp = Apps::None;
      Screen::FullRefreshDirections nextDirection;

      // TouchEvents GetGesture();
      static void Process(void* instance);
      void InitHw();
      void refresh();
      void loadScreen(Apps app, Screen::FullRefreshDirections direction);
      void loadScreen(Screen* screen, Screen::FullRefreshDirections direction, bool store = true);
      void PushMessageToSystemTask(System::Messages id);

      System::BootErrors bootError;
      void ApplyBrightness();
      bool isDimmed = false;
    };
  }
}
