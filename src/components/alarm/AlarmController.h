#pragma once

#include <FreeRTOS.h>
#include <timers.h>
#include <chrono>
#include <vector>

namespace Pinetime {
  namespace Controllers {
    class AlarmController {

    public:
      enum class AlarmState : uint8_t { Not_Set, Set, Alerting };

      struct RecurType {
        bool Sun = true, Mon = true, Tue = true, Wed = true, Thu = true, Fri = true, Sat = true, Once = false;
      };

      AlarmController();
      ~AlarmController();

      void SetAlarmTime(uint8_t alarmHr, uint8_t alarmMin);
      void ScheduleAlarm();
      void DisableAlarm();
      void alarmEvent();
      uint32_t SecondsToAlarm() const;
      void StopAlerting();

      uint8_t Hours() const {
        return t.hours;
      }

      uint8_t Minutes() const {
        return t.minutes;
      }

      AlarmState State() const {
        return t.state;
      }

      RecurType Recurrence() const {
        return t.recurrence;
      }

      void SetRecurrence(RecurType recurrence) {
        t.recurrence = recurrence;
      }

      static constexpr uint8_t MaxElements = 4;
      static std::vector<AlarmController*> alarmControllers;
      static void Init();
      static void Save();

    private:
      struct timeData {
        uint8_t hours = 7;
        uint8_t minutes = 0;
        AlarmState state = AlarmState::Not_Set;
        RecurType recurrence;
        std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> alarmTime;       
      } t;

      AlarmController(timeData& time);
      bool hasMoreAlarmSet();
      void createTimer();
      TimerHandle_t alarmTimer;
      static constexpr const char* fileName = "/alarms.bin";
    };
  }
}
