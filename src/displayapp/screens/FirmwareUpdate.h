#pragma once

#include "Screen.h"
#include "components/ble/BleController.h"
#include "FreeRTOS.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {

      class FirmwareUpdate : public Screen {
      public:
        FirmwareUpdate(const Controllers::Ble& bleController);
        ~FirmwareUpdate() override;
        void Load() override;
        bool UnLoad() override;

      private:
        void Refresh() override;
        enum class States : uint8_t { Idle, Running, Validated, Error };
        const Controllers::Ble& bleController;
        lv_obj_t* bar1;
        lv_obj_t* percentLabel;
        lv_obj_t* titleLabel;

        States state = States::Idle;

        void DisplayProgression() const;

        bool OnButtonPushed() override;

        void UpdateValidated();

        void UpdateError();

        lv_task_t* taskRefresh;
        TickType_t startTime;
      };
    }
  }
}
