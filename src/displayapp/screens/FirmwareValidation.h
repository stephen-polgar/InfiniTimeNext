#pragma once

#include "Screen.h"
#include "components/firmwarevalidator/FirmwareValidator.h"

namespace Pinetime { 
  namespace Applications {
    namespace Screens {
      class FirmwareValidation : public Screen {
      public:
        FirmwareValidation(Controllers::FirmwareValidator& validator);
        ~FirmwareValidation() override;
        void Load() override;
        bool UnLoad() override;

      private:
        Controllers::FirmwareValidator& validator;
        void onButtonEvent(lv_obj_t* object);
        static void buttonEventHandler(lv_obj_t* obj, lv_event_t event);
        lv_obj_t* buttonValidate;
        lv_obj_t* buttonReset = NULL;    
      };
    }
  }
}
