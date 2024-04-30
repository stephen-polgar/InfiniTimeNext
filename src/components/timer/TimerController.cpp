#include "TimerController.h"
#include "systemtask/SystemTask.h"

using namespace Pinetime::Controllers;

std::vector<TimerController*> TimerController::timers;

TimerController::TimerController(std::chrono::milliseconds duration) : duration {duration} {
  timer = xTimerCreate("Timer", 1, pdFALSE, this, displayAppCallbackTimer);
  timers.push_back(this);
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

void TimerController::displayAppCallbackTimer(TimerHandle_t xTimer) {
  (static_cast<Controllers::TimerController*>(pvTimerGetTimerID(xTimer)))->done = true;
  System::SystemTask::displayApp->PushMessage(Applications::Display::Messages::TimerDone);
}

void TimerController::Init() {
  timers.reserve(MaxElements);
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
  // fs->FileDelete(fileName);
  if (fs->FileOpen(&file, fileName, LFS_O_WRONLY | LFS_O_CREAT) == LFS_ERR_OK) {
    for (auto* timer : timers) {
      fs->FileWrite(&file, reinterpret_cast<uint8_t*>(&timer->duration), sizeof(std::chrono::milliseconds));
    }
    fs->FileClose(&file);
  }
}

TimerController::~TimerController() {
  // if (IsRunning()) StopTimer();
  xTimerDelete(timer, 0);
  timers.erase(std::remove(timers.begin(), timers.end(), this), timers.end());
}
