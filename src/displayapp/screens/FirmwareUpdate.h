#pragma once

#include "ScreenRefresh.h"
#include "components/ble/BleController.h"
#include "FreeRTOS.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {

      class FirmwareUpdate : public ScreenRefresh {
      public:
        FirmwareUpdate(const Controllers::Ble& bleController);
        void Load() override;
       
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

        TickType_t startTime;
      };
    }
  }
}
