#pragma once

#include "ScreenRefresh.h"
#ifdef UseDice
  #include "displayapp/widgets/Counter.h"
  #include "Symbols.h"
  #include <array>
  #include <random>

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class Dice : public ScreenRefresh {
      public:
        Dice();
        ~Dice() override;
        void Load() override;
        bool UnLoad() override;

      private:
        void Refresh() override;
        void roll();
        static void btnRollEventHandler(lv_obj_t* obj, lv_event_t event);
        lv_obj_t* btnRoll;
        lv_obj_t* btnRollLabel;
        lv_obj_t* resultTotalLabel;
        lv_obj_t* resultIndividualLabel;
        bool enableShakeForDice = false;

        std::mt19937 gen;

        std::array<lv_color_t, 3> resultColors = {LV_COLOR_YELLOW, LV_COLOR_MAGENTA, LV_COLOR_AQUA};
        uint8_t currentColorIndex;
        void NextColor();

        Widgets::Counter nCounter = Widgets::Counter(1, 9, jetbrains_mono_42);
        Widgets::Counter dCounter = Widgets::Counter(2, 99, jetbrains_mono_42);

        bool openingRoll = true;
        uint8_t currentRollHysteresis = 0;
        static constexpr uint8_t rollHysteresis = 10;
      };
    }

    template <>
    struct AppTraits<Apps::Dice> {
      static constexpr Apps app = Apps::Dice;
      static constexpr const char* icon = Screens::Symbols::dice;

      static Screens::Screen* Create() {
        return new Screens::Dice();
      };
    };
  }
}
#endif