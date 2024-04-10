#include "Timer.h"
#include "TimerSet.h"
#include "systemtask/SystemTask.h"

using namespace Pinetime::Applications::Screens;
using Pinetime::Controllers::TimerController;

bool Timer::changed = false;

Timer::Timer()
  : Screen(Apps::Timer),
    buttonListItem {[this](bool enabled, void* data) {
                      on_checked(enabled, static_cast<TimerController*>(data));
                    },
                    [this](void* data) {
                      on_open(static_cast<TimerController*>(data));
                    },
                    [this](void* data) {
                      on_removed(static_cast<TimerController*>(data));
                    },
                    [this]() {
                      on_addNew();
                    }} {
}

void Timer::Load() {
  running = true;
  for (auto* timerController : TimerController::timers) {
    createNewItem(timerController);
  }
  buttonListItem.CreateButtonNew();
  updateAddButton();
}

bool Timer::UnLoad() {
  if (running) {
    lv_obj_clean(lv_scr_act());
    buttonListItem.Unload();
    running = false;
  }
  return true;
}

Timer::~Timer() {
  UnLoad();
  if (Timer::changed) {
    Timer::changed = false;
    TimerController::Save();
  }
}

void Timer::updateAddButton() {
  buttonListItem.EnableAddButton(TimerController::timers.size() < TimerController::MaxElements);
}

void Timer::createNewItem(TimerController* timerController) {
  lv_obj_t* label = lv_label_create(lv_scr_act(), NULL);
  auto secondsRemaining = std::chrono::duration_cast<std::chrono::seconds>(timerController->duration);
  lv_label_set_text_fmt(label, "%.2d:%.2d", secondsRemaining.count() / 60, secondsRemaining.count() % 60);  
  buttonListItem.CreateNewItem(label, timerController->IsRunning(), timerController);
}

void Timer::on_checked(bool checked, TimerController* timerController) {
  if (checked)
    timerController->StartTimer(timerController->duration);
  else
    timerController->StopTimer();
}

void Timer::on_open(TimerController* timerController) {
  System::SystemTask::displayApp->StartApp(new TimerSet(timerController));
}

void Timer::on_removed(TimerController* timerController) {
  Timer::changed = true;
  delete timerController;
  updateAddButton();
};

void Timer::on_addNew() {
  TimerController* timerController = new TimerController();
  createNewItem(timerController);
  System::SystemTask::displayApp->StartApp(new TimerSet(timerController));
  updateAddButton();
}