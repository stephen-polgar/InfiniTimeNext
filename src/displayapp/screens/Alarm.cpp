#include "Alarm.h"
#include "AlarmSet.h"
#include "systemtask/SystemTask.h"

using namespace Pinetime::Applications::Screens;
using Pinetime::Controllers::AlarmController;

bool Alarm::changed = false;

Alarm::Alarm()
  : Screen(Apps::Alarm),
    buttonListItem {[this](bool enabled, void* data) {
                      on_checked(enabled, static_cast<AlarmController*>(data));
                    },
                    [this](void* data) {
                      on_open(static_cast<AlarmController*>(data));
                    },
                    [this](void* data) {
                      on_removed(static_cast<AlarmController*>(data));
                    },
                    [this]() {
                      on_addNew();
                    }} {
}

void Alarm::Load() {
  running = true;
  for (auto* alarmController : AlarmController::alarmControllers) {
    createNewItem(alarmController);
  }
  buttonListItem.CreateButtonNew();
  updateAddButton();
}

bool Alarm::UnLoad() {
  if (running) {
    lv_obj_clean(lv_scr_act());
    buttonListItem.Unload();
    running = false;
  }  
  return true;
}

Alarm::~Alarm() {
  UnLoad();
  if (Alarm::changed) {
    Alarm::changed = false;
    AlarmController::Save();
  }
}

void Alarm::updateAddButton() {
  buttonListItem.EnableAddButton(AlarmController::alarmControllers.size() < AlarmController::MaxElements);
}

void Alarm::createNewItem(AlarmController* alarmController) {
  lv_obj_t* label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text_fmt(label, "%.2d:%.2d", alarmController->Hours(), alarmController->Minutes());
  buttonListItem.CreateNewItem(label, alarmController->State() == AlarmController::AlarmState::Set, alarmController);
}

void Alarm::on_checked(bool checked, AlarmController* ac) {
  Alarm::changed = true;
  if (checked) {
    ac->ScheduleAlarm();
  } else {
    ac->DisableAlarm();
  }
}

void Alarm::on_open(AlarmController* ac) {
  System::SystemTask::displayApp->StartApp(new AlarmSet(ac));
}

void Alarm::on_removed(AlarmController* ac) {
  Alarm::changed = true;
  delete ac;
  updateAddButton();
};

void Alarm::on_addNew() {
  AlarmController* alarmController = new AlarmController();
  createNewItem(alarmController);
  System::SystemTask::displayApp->StartApp(new AlarmSet(alarmController));
  updateAddButton();
}
