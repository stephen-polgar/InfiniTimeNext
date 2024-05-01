#pragma once

#include "displayapp/screens/Screen.h"
#ifdef UseInfiniPaint
  #include "Symbols.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class InfiniPaint : public Screen {
      public:
        InfiniPaint();
        void Load() override;
        bool OnTouchEvent(TouchEvents event) override;
        bool OnTouchEvent(uint16_t x, uint16_t y) override;

      private:
        static constexpr uint8_t width = 10;
        static constexpr uint8_t height = 10;
        static constexpr uint8_t bufferSize = width * height;
        lv_color_t b[bufferSize];
        lv_color_t selectColor = LV_COLOR_WHITE;
        uint8_t color = 2;
      };
    }

    template <>
    struct AppTraits<Apps::Paint> {
      static constexpr Apps app = Apps::Paint;
      static constexpr const char* icon = Screens::Symbols::paintbrush;

      static Screens::Screen* Create() {
        return new Screens::InfiniPaint();
      };
    };
  }
}
#endif