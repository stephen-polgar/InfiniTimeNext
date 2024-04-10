#pragma once

#include "ScreenTree.h"
#include "Label.h"
#include "systemtask/SystemTask.h"
#include "components/ble/BleController.h"
#include "components/datetime/DateTimeController.h"
#include "components/motion/MotionController.h"
#include "drivers/Watchdog.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class SystemInfo : public Screen {
      public:
        explicit SystemInfo();
        ~SystemInfo() override;
        void Load() override;
        bool UnLoad() override;
        bool OnTouchEvent(TouchEvents event) override;

      private:
        static int mallocFailedCount, stackOverflowCount;
        static constexpr uint8_t screenNumber = 5;
        Widgets::PageIndicator pageIndicator {screenNumber};
        ScreenTree* screens;
        static const char* toString(const Controllers::MotionController::DeviceTypes deviceType);
        static bool sortById(const TaskStatus_t& lhs, const TaskStatus_t& rhs);

        class MemoryInfo : public Label {
        public:
          MemoryInfo(uint8_t screenID, Widgets::PageIndicator& pageIndicator);
          void Load() override;
        };

        class TasksScreen : public Label {
        public:
          TasksScreen(uint8_t screenID, Widgets::PageIndicator& pageIndicator);
          void Load() override;

        private:
          static constexpr uint8_t maxTaskCount = 9;
          TaskStatus_t tasksStatus[maxTaskCount];
        };

        class HardverScreen : public Label {
        public:
          HardverScreen(uint8_t screenID, Widgets::PageIndicator& pageIndicator) : Label(screenID, &pageIndicator) {
            uptimeSeconds = uptimeSeconds % secondsInADay;
            uptimeHours = uptimeSeconds / secondsInAnHour;
            uptimeSeconds = uptimeSeconds % secondsInAnHour;
            uptimeMinutes = uptimeSeconds / secondsInAMinute;
            uptimeSeconds = uptimeSeconds % secondsInAMinute;
          }

          void Load() override;

        private:
          // uptime
          static constexpr uint32_t secondsInADay = 60 * 60 * 24;
          static constexpr uint32_t secondsInAnHour = 60 * 60;
          static constexpr uint32_t secondsInAMinute = 60;
          uint32_t uptimeSeconds = System::SystemTask::displayApp->dateTimeController.Uptime().count();
          uint32_t uptimeDays = (uptimeSeconds / secondsInADay);
          uint32_t uptimeHours, uptimeMinutes;
          // TODO handle more than 100 days of uptime

          const Controllers::Ble::BleAddress& bleAddr = System::SystemTask::displayApp->bleController.Address();
          const char* resetReason = [this]() {
            switch (System::SystemTask::displayApp->watchdog.GetResetReason()) {
              case Drivers::Watchdog::ResetReason::Watchdog:
                return "wtdg";
              case Drivers::Watchdog::ResetReason::HardReset:
                return "hardr";
              case Drivers::Watchdog::ResetReason::NFC:
                return "nfc";
              case Drivers::Watchdog::ResetReason::SoftReset:
                return "softr";
              case Drivers::Watchdog::ResetReason::CpuLockup:
                return "cpulock";
              case Drivers::Watchdog::ResetReason::SystemOff:
                return "off";
              case Drivers::Watchdog::ResetReason::LpComp:
                return "lpcomp";
              case Drivers::Watchdog::ResetReason::DebugInterface:
                return "dbg";
              case Drivers::Watchdog::ResetReason::ResetPin:
                return "rst";
              default:
                return "?";
            }
          }();
        };

        class FirmwareScreen : public Label {
        public:
          FirmwareScreen(uint8_t screenID, Widgets::PageIndicator& pageIndicator) : Label(screenID, &pageIndicator) {
          }

          void Load() override;
        };

        class LicenseScreen : public Label {
        public:
          LicenseScreen(uint8_t screenID, Widgets::PageIndicator& pageIndicator) : Label(screenID, &pageIndicator) {
          }

          void Load() override;
        };
      };
    }
  }
}
