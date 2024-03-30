#pragma once

#include "Screen.h"
#ifdef UseTwos

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class Twos : public Screen {
      public:
        Twos();
        ~Twos() override;
        void Load() override;
        bool UnLoad() override;

        bool OnTouchEvent(TouchEvents event) override;

      private:
        static constexpr uint8_t nColors = 5;
        lv_style_t cellStyles[nColors];

        struct TwosTile {
          bool merged = false;
          uint8_t value = 0;
        };

        struct colorPair {
          lv_color_t bg;
          lv_color_t fg;
        };

        static constexpr colorPair colors[nColors] = {
          {LV_COLOR_MAKE(0xcd, 0xc0, 0xb4), LV_COLOR_BLACK},
          {LV_COLOR_MAKE(0xef, 0xdf, 0xc6), LV_COLOR_BLACK},
          {LV_COLOR_MAKE(0xef, 0x92, 0x63), LV_COLOR_WHITE},
          {LV_COLOR_MAKE(0xf7, 0x61, 0x42), LV_COLOR_WHITE},
          {LV_COLOR_MAKE(0x00, 0x7d, 0xc5), LV_COLOR_WHITE},
        };

        lv_obj_t* scoreText;
        lv_obj_t* gridDisplay;
        static constexpr uint8_t nCols = 4;
        static constexpr uint8_t nRows = 4;
        static constexpr uint8_t nCells = nCols * nRows;
        TwosTile grid[nRows][nCols];
        uint8_t score = 0;
        void updateGridDisplay();
        bool tryMerge(uint8_t newRow, uint8_t newCol, uint8_t oldRow, uint8_t oldCol);
        bool tryMove(int8_t newRow, int8_t newCol, int8_t oldRow, int8_t oldCol);
        bool placeNewTile();
      };
    }

    template <>
    struct AppTraits<Apps::Twos> {
      static constexpr Apps app = Apps::Twos;
      static constexpr const char* icon = "2";

      static Screens::Screen* Create() {
        return new Screens::Twos();
      };
    };
  }
}
#endif