#include "AlarmController.h"
#include "systemtask/SystemTask.h"

// #define Log

#ifdef Log
  #include <nrf_log.h>
#endif

using namespace Pinetime::Controllers;
using namespace std::chrono_literals;

std::vector<AlarmController*> AlarmController::alarmControllers;

AlarmController::AlarmController(timeData& time) {
  t = time;
  createTimer();
  alarmControllers.push_back(this);
}

AlarmController::AlarmController() {
  createTimer();
  alarmControllers.push_back(this);
}

AlarmController::~AlarmController() {
  // if (t.state != AlarmState::Not_Set)  xTimerStop(alarmTimer, 0);
  xTimerDelete(alarmTimer, 0);
  alarmControllers.erase(std::remove(alarmControllers.begin(), alarmControllers.end(), this), alarmControllers.end());
}

void timerCallback(TimerHandle_t xTimer) {
  (static_cast<Pinetime::Controllers::AlarmController*>(pvTimerGetTimerID(xTimer)))->alarmEvent();
}

void AlarmController::createTimer() {
  alarmTimer = xTimerCreate("Alarm", 1, pdFALSE, this, timerCallback);
}

void AlarmController::SetAlarmTime(uint8_t alarmHr, uint8_t alarmMin) {
  t.hours = alarmHr;
  t.minutes = alarmMin;
}

void AlarmController::ScheduleAlarm() {
#ifdef Log
  NRF_LOG_INFO("AlarmController::ScheduleAlarm=%d state=%d", this, t.state);
#endif
  // Determine the next time the alarm needs to go off and set the timer
  xTimerStop(alarmTimer, 0);
  auto dtc = System::SystemTask::displayApp->dateTimeController;
  auto now = dtc.CurrentDateTime();
  time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::time_point_cast<std::chrono::system_clock::duration>(now));
  tm* localTime = std::localtime(&currentTime);

  // If the time being set has already passed today,the alarm should be set for tomorrow
  uint8_t h = dtc.Hours();
  if (t.hours < h || (t.hours == h && t.minutes <= dtc.Minutes())) {
    localTime->tm_mday += 1;
    // tm_wday doesn't update automatically
    localTime->tm_wday = (localTime->tm_wday + 1) % 7;
  }

  localTime->tm_hour = t.hours;
  localTime->tm_min = t.minutes;
  localTime->tm_sec = 0;
  localTime->tm_isdst = -1; // use system timezone setting to determine DST
  // now can convert back to a time_point
  t.alarmTime = std::chrono::system_clock::from_time_t(std::mktime(localTime));
  auto secondsToAlarm = std::chrono::duration_cast<std::chrono::seconds>(t.alarmTime - now).count();
  xTimerChangePeriod(alarmTimer, secondsToAlarm * configTICK_RATE_HZ, 0);
  xTimerStart(alarmTimer, 0);
  t.state = AlarmState::Set;
}

uint32_t AlarmController::SecondsToAlarm() const {
  return std::chrono::duration_cast<std::chrono::seconds>(t.alarmTime -
                                                          System::SystemTask::displayApp->dateTimeController.CurrentDateTime())
    .count();
}

void AlarmController::DisableAlarm() {
#ifdef Log
  NRF_LOG_INFO("AlarmController::DisableAlarm=%d state=%d", this, t.state);
#endif
  xTimerStop(alarmTimer, 0);
  t.state = AlarmState::Not_Set;
}

bool AlarmController::hasMoreAlarmSet() {
  return (t.recurrence.Sun || t.recurrence.Mon || t.recurrence.Tue || t.recurrence.Wed || t.recurrence.Thu || t.recurrence.Fri ||
          t.recurrence.Sat);
}

void AlarmController::alarmEvent() {
  auto now = System::SystemTask::displayApp->dateTimeController.CurrentDateTime();
  time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::time_point_cast<std::chrono::system_clock::duration>(now));
  tm* localTime = std::localtime(&currentTime);
  bool enable;
  switch (localTime->tm_wday) {
    case 0:
      enable = t.recurrence.Sun;
      if (enable && t.recurrence.Once)
        t.recurrence.Sun = false;
      break;
    case 1:
      enable = t.recurrence.Mon;
      if (enable && t.recurrence.Once)
        t.recurrence.Mon = false;
      break;
    case 2:
      enable = t.recurrence.Tue;
      if (enable && t.recurrence.Once)
        t.recurrence.Tue = false;
      break;
    case 3:
      enable = t.recurrence.Wed;
      if (enable && t.recurrence.Once)
        t.recurrence.Wed = false;
      break;
    case 4:
      enable = t.recurrence.Thu;
      if (enable && t.recurrence.Once)
        t.recurrence.Thu = false;
      break;
    case 5:
      enable = t.recurrence.Fri;
      if (enable && t.recurrence.Once)
        t.recurrence.Fri = false;
      break;
    case 6:
      enable = t.recurrence.Sat;
      if (enable && t.recurrence.Once)
        t.recurrence.Sat = false;
      break;
    default:
      enable = false;
      break;
  }
  if (enable) {
    t.state = AlarmState::Alerting;
    System::SystemTask::displayApp->systemTask->PushMessage(System::Messages::SetOffAlarm);
  } else if (hasMoreAlarmSet())
    ScheduleAlarm();
}

void AlarmController::StopAlerting() {
#ifdef Log
  NRF_LOG_INFO("AlarmController::StopAlerting=%d state=%d", this, t.state);
#endif
  if (hasMoreAlarmSet())
    ScheduleAlarm();
  else
    t.state = AlarmState::Not_Set;
}

void AlarmController::Init() {
  alarmControllers.reserve(MaxElements);
  timeData buffer;
  lfs_file_t file;
  auto* fs = &System::SystemTask::displayApp->filesystem;
  if (fs->FileOpen(&file, fileName, LFS_O_RDONLY) == LFS_ERR_OK) {
    while (fs->FileRead(&file, reinterpret_cast<uint8_t*>(&buffer), sizeof(timeData)) == sizeof(timeData)) {
      new AlarmController(buffer);
    }
    fs->FileClose(&file);
  }
}

void AlarmController::Save() {
  lfs_file_t file;
  auto* fs = &System::SystemTask::displayApp->filesystem;
  // fs->FileDelete(fileName);
  if (fs->FileOpen(&file, fileName, LFS_O_WRONLY | LFS_O_CREAT) == LFS_ERR_OK) {
    for (auto* alarmController : alarmControllers) {
      fs->FileWrite(&file, reinterpret_cast<uint8_t*>(&alarmController->t), sizeof(timeData));
    }
    fs->FileClose(&file);
  }
}
