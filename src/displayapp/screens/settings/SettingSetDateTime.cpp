#include "SettingSetDateTime.h"
#include "SettingSetDate.h"
#include "SettingSetTime.h"

using namespace Pinetime::Applications::Screens;

bool SettingSetDateTime::OnTouchEvent(Pinetime::Applications::TouchEvents event) {
  return screens.OnTouchEvent(event);
}

SettingSetDateTime::SettingSetDateTime()
  : Screen(Apps::SettingSetDateTime),
    screens {{[this]() -> std::unique_ptr<Screen> {
                return screenSetDate();
              },
              [this]() -> std::unique_ptr<Screen> {
                return screenSetTime();
              }}} {
}

void SettingSetDateTime::Load() {
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

std::unique_ptr<Screen> SettingSetDateTime::screenSetDate() {
  return std::make_unique<Screens::SettingSetDate>(*this);
}

std::unique_ptr<Screen> SettingSetDateTime::screenSetTime() { 
  return std::make_unique<Screens::SettingSetTime>(*this);
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
