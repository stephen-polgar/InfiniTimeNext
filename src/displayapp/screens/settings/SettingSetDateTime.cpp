#include "SettingSetDateTime.h"
#include "SettingSetDate.h"
#include "SettingSetTime.h"

using namespace Pinetime::Applications::Screens;

bool SettingSetDateTime::OnTouchEvent(Pinetime::Applications::TouchEvents event) {
  return screens.OnTouchEvent(event);
}

SettingSetDateTime::SettingSetDateTime() : Screen(Apps::SettingSetDateTime) {
    screens.Add(new Screens::SettingSetDate(*this));
    screens.Add(new Screens::SettingSetTime(*this));           
}

void SettingSetDateTime::Load() {
  running = true;
  loaded = true;
  screens.Load();
}

bool SettingSetDateTime::UnLoad() {
  if (loaded) {
    loaded = false;
    screens.UnLoad();
  }
  return true;
}

SettingSetDateTime::~SettingSetDateTime() {
  UnLoad();
}

void SettingSetDateTime::Advance() {
  screens.OnTouchEvent(Applications::TouchEvents::SwipeUp);
}

void SettingSetDateTime::Quit() {
  running = false;
}
