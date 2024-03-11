#pragma once

#include "Screen.h"
#include "components/brightness/BrightnessController.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class FlashLight : public Screen {
      public:
        FlashLight();
        ~FlashLight()override;
        void Load() override;
        bool UnLoad() override;
        bool OnTouchEvent(Pinetime::Applications::TouchEvents event) override;       

      private:     
        uint32_t screenTimeout;
        void toggle();
        void setIndicators();
        void setColors();
        static void eventHandler(lv_obj_t* obj, lv_event_t event);
        Controllers::BrightnessController::Levels brightnessLevel = Controllers::BrightnessController::Levels::High;

        lv_obj_t* flashLight;
        lv_obj_t* backgroundAction;
        lv_obj_t* indicators[3];
        bool isOn = false;
      };
    }
  }
}
