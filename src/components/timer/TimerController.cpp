#include "TimerController.h"
#include "systemtask/SystemTask.h"

#ifdef Log
  #include <libraries/log/nrf_log.h>
#endif

using namespace Pinetime::Controllers;

const char* TimerController::fileName = "/timers.bin";
// std::vector<Timer*> Timer::timers(Elements);
std::vector<TimerController*> TimerController::timers;
TimerCallbackFunction_t TimerController::pxCallbackFunction;

TimerController::TimerController(std::chrono::milliseconds duration) : duration {duration} {
  timer = xTimerCreate("Timer", 1, pdFALSE, this, pxCallbackFunction);
  timers.push_back(this);
#ifdef Log
  NRF_LOG_INFO("Timer::Timer()=%d", this);
#endif
}

void TimerController::StartTimer(std::chrono::milliseconds duration) {
  xTimerChangePeriod(timer, pdMS_TO_TICKS(duration.count()), 0);
  xTimerStart(timer, 0);
}

std::chrono::milliseconds TimerController::GetTimeRemaining() {
  if (IsRunning()) {
    TickType_t remainingTime = xTimerGetExpiryTime(timer) - xTaskGetTickCount();
    return std::chrono::milliseconds(remainingTime * 1000 / configTICK_RATE_HZ);
  }
  return std::chrono::milliseconds(0);
}

void TimerController::StopTimer() {
  xTimerStop(timer, 0);
}

bool TimerController::IsRunning() {
  return (xTimerIsTimerActive(timer) == pdTRUE);
}

void TimerController::Init(TimerCallbackFunction_t pxCallbackFunction) {
  TimerController::pxCallbackFunction = pxCallbackFunction;
  std::chrono::milliseconds buffer;
  lfs_file_t file;
  auto* fs = &System::SystemTask::displayApp->filesystem;
  if (fs->FileOpen(&file, fileName, LFS_O_RDONLY) == LFS_ERR_OK) {
    while (fs->FileRead(&file, reinterpret_cast<uint8_t*>(&buffer), sizeof(std::chrono::milliseconds)) ==
           sizeof(std::chrono::milliseconds)) {
      new TimerController(buffer);
    }
    fs->FileClose(&file);
  }
}

void TimerController::Save() {
  lfs_file_t file;
  auto* fs = &System::SystemTask::displayApp->filesystem;
  fs->FileDelete(fileName);
  if (fs->FileOpen(&file, fileName, LFS_O_WRONLY | LFS_O_CREAT) == LFS_ERR_OK) {
    for (auto* timer : timers) {
      fs->FileWrite(&file, reinterpret_cast<uint8_t*>(&timer->duration), sizeof(std::chrono::milliseconds));
    }
    fs->FileClose(&file);
  }
}

TimerController::~TimerController() {
  // xTimerDelete
#ifdef Log
  NRF_LOG_INFO("Timer::~Timer()=%d", this);
#endif
  if (IsRunning())
    StopTimer();
  timers.erase(std::remove(timers.begin(), timers.end(), this), timers.end());
}
