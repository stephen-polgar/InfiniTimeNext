#pragma once

#include "displayapp/screens/Screen.h"
#ifdef UseMetronome
  #include "Symbols.h"
  #include <portmacro_cmsis.h>

namespace Pinetime {
  namespace Applications {
    namespace Screens {

      class Metronome : public Screen {
      public:
        Metronome();
        ~Metronome() override;
        void Load() override;
        bool UnLoad() override;
        bool OnTouchEvent(TouchEvents event) override;

      private:
        void Refresh() override;
        void onEvent(lv_obj_t* obj, lv_event_t event);
        static void eventHandler(lv_obj_t* obj, lv_event_t event);
        static lv_obj_t* createLabel(const char* name, lv_obj_t* reference, lv_align_t align, lv_font_t* font, uint8_t x, uint8_t y);
        TickType_t startTime = 0;
        TickType_t tappedTime = 0;      
        int16_t bpm = 120;
        uint8_t bpb = 4;
        uint8_t counter = 1;

        bool metronomeStarted = false;
        bool allowExit = false;

        lv_obj_t *bpmArc, *bpmTap, *bpmValue;
        lv_obj_t *bpbDropdown, *currentBpbText;
        lv_obj_t* playPause;
        lv_obj_t* lblPlayPause;

        lv_task_t* taskRefresh;
      };
    }

    template <>
    struct AppTraits<Apps::Metronome> {
      static constexpr Apps app = Apps::Metronome;
      static constexpr const char* icon = Screens::Symbols::drum;

      static Screens::Screen* Create() {
        return new Screens::Metronome();
      };
    };
  }
}
#endif