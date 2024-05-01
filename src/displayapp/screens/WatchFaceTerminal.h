#pragma once

#include "Screen.h"
#ifdef UseWatchFaceTerminal
  #include "components/fs/FS.h"
  #include "utility/DirtyValue.h"
  #include <chrono>

namespace Pinetime {
  namespace Applications {
    namespace Screens {

      class WatchFaceTerminal : public Screen {
      public:
        WatchFaceTerminal();
       void Load() override;
       void Refresh() override;

      private:
        Utility::DirtyValue<int> batteryPercentRemaining;
        Utility::DirtyValue<bool> powerPresent;
        Utility::DirtyValue<bool> bleState;
        Utility::DirtyValue<bool> bleRadioEnabled;
        Utility::DirtyValue<std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>> currentDateTime;
        Utility::DirtyValue<uint32_t> stepCount;
        Utility::DirtyValue<uint8_t> heartbeat;
        Utility::DirtyValue<bool> heartbeatRunning;
        Utility::DirtyValue<bool> notificationState;
        Utility::DirtyValue<std::chrono::time_point<std::chrono::system_clock, std::chrono::days>> currentDate;

        lv_obj_t* label_time;
        lv_obj_t* label_date;
        lv_obj_t* label_prompt_1;
        lv_obj_t* label_prompt_2;
        lv_obj_t* batteryValue;
        lv_obj_t* heartbeatValue;
        lv_obj_t* stepValue;
        lv_obj_t* notificationIcon;
        lv_obj_t* connectState;
      };
    }

    template <>
    struct WatchFaceTraits<WatchFace::Terminal> {
      static constexpr WatchFace watchFace = WatchFace::Terminal;
      static constexpr const char* name = "Terminal";

      static Screens::Screen* Create() {
        return new Screens::WatchFaceTerminal();
      };

      static bool IsAvailable(Pinetime::Controllers::FS& /*filesystem*/) {
        return true;
      }
    };
  }
}
#endif