#pragma once

#include "ScreenRefresh.h"
#include <portmacro_cmsis.h>
#include "displayapp/apps/Apps.h"
#include "Symbols.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      enum class States : uint8_t { Init, Running, Halted };

      struct TimeSeparated_t {
        int hours;
        int mins;
        int secs;
        int hundredths;
      };

      class StopWatch : public ScreenRefresh {
      public:
        explicit StopWatch();
        ~StopWatch() override;
        void Load() override;
        bool UnLoad() override;
        bool OnButtonPushed() override;

      private:
        void refresh(TimeSeparated_t& currentTimeSeparated);
        void Refresh() override;
        void SetInterfacePaused();
        void SetInterfaceRunning();
        void SetInterfaceStopped();

        void Reset();
        void Start();
        void Pause();

        static TickType_t startTime;
        static TimeSeparated_t convertTicksToTimeSegments(const TickType_t timeElapsed);
        void playPauseBtnEventHandler();
        void stopLapBtnEventHandler();
        static void play_pause_event_handler(lv_obj_t* obj, lv_event_t event);
        static void stop_lap_event_handler(lv_obj_t* obj, lv_event_t event);
        static constexpr int maxLapCount = 20;
        static TickType_t laps[];
        static constexpr int displayedLaps = 2;

        static TickType_t oldTimeElapsed;
        static TickType_t blinkTime;
        static States currentState;
        static int lapsDone;
        static TimeSeparated_t currentTimeSeparated;
        bool isHoursLabelUpdated;

        lv_obj_t *time, *msecTime, *btnPlayPause, *btnStopLap, *txtPlayPause, *txtStopLap;
        lv_obj_t* lapText;       
      };
    }

    template <>
    struct AppTraits<Apps::StopWatch> {
      static constexpr Apps app = Apps::StopWatch;
      static constexpr const char* icon = Screens::Symbols::stopWatch;

      static Screens::Screen* Create() {
        return new Screens::StopWatch();
      };
    };
  }
}
