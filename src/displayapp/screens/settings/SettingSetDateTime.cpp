#include "SettingSetDateTime.h"
#include "SettingSetDate.h"
#include "SettingSetTime.h"
#include "systemtask/SystemTask.h"

using namespace Pinetime::Applications::Screens;

SettingSetDateTime::SettingSetDateTime()
  : Screen(Apps::SettingSetDateTime),
    arrayTouchHandler {items,
                       1,
                       [this](uint8_t indexBegin, uint8_t indexEnd, Screen::FullRefreshDirections direction) {
                         load(indexBegin, indexEnd, direction);
                       }},
    dotIndicator {items} {
  list[0] = new Screens::SettingSetDate(*this);
  list[1] = new Screens::SettingSetTime(*this);
}

void SettingSetDateTime::Load() {
  arrayTouchHandler.LoadCurrentPos();
  running = true;
}

void SettingSetDateTime::load(uint8_t indexBegin, uint8_t, Screen::FullRefreshDirections direction) {
  if (running) {
    dotIndicator.UnLoad();
    lv_obj_clean(lv_scr_act());
    System::SystemTask::displayApp->SetFullRefresh(direction);
  }
  dotIndicator.Load(indexBegin);
  list[indexBegin]->Load();
}

bool SettingSetDateTime::UnLoad() {
  if (running) {
    running = false;
    dotIndicator.UnLoad();
    lv_obj_clean(lv_scr_act());
  }
  return true;
}

SettingSetDateTime::~SettingSetDateTime() {
  UnLoad();
  for (uint8_t i = 0; i < items; i++) {
    delete list[i];
  }
}

void SettingSetDateTime::SwipeUp() {
  arrayTouchHandler.OnTouchEvent(TouchEvents::SwipeUp);
}

bool SettingSetDateTime::OnTouchEvent(TouchEvents event) {
  return arrayTouchHandler.OnTouchEvent(event);
}
