#pragma once

#include "ScreenRefresh.h"
#ifdef UsePaddle
  #include "Symbols.h"

namespace Pinetime {
   namespace Applications {
    namespace Screens {
      class Paddle : public ScreenRefresh {
      public:
        Paddle();
        void Load() override;
        bool OnTouchEvent(TouchEvents event) override;
        bool OnTouchEvent(uint16_t x, uint16_t y) override;

      private:
        void Refresh() override;
        const uint8_t ballSize = 16;

        uint16_t paddlePos = 30; // Paddle center

        int16_t ballX = (LV_HOR_RES - ballSize) / 2;
        int16_t ballY = (LV_VER_RES - ballSize) / 2;

        int8_t dx = 2; // Velocity of the ball in the x_coordinate
        int8_t dy = 3; // Velocity of the ball in the y_coordinate

        uint16_t score = 0;
        lv_obj_t* points;
        lv_obj_t* paddle;
        lv_obj_t* ball;
        lv_obj_t* background;       
      };
    }

    template <>
    struct AppTraits<Apps::Paddle> {
      static constexpr Apps app = Apps::Paddle;
      static constexpr const char* icon = Screens::Symbols::paddle;

      static Screens::Screen* Create() {
        return new Screens::Paddle();
      };
    };
  }
}
#endif