#include "SettingSetDateTime.h"
#include "SettingSetDate.h"
#include "SettingSetTime.h"

using namespace Pinetime::Applications::Screens;

bool SettingSetDateTime::OnTouchEvent(Pinetime::Applications::TouchEvents event) {
  return screens->OnTouchEvent(event);
}

SettingSetDateTime::SettingSetDateTime() : Screen(Apps::SettingSetDateTime) {
    screens = new Screens::SettingSetDate(*this);
    screens->Add(new Screens::SettingSetTime(*this));           
}

void SettingSetDateTime::Load() {
  screens->GetCurrent()->Load();
  running = true;
}

bool SettingSetDateTime::UnLoad() {
  if (running) {
    running = false;
    screens->GetCurrent()->UnLoad();
  }
  return true;
}

SettingSetDateTime::~SettingSetDateTime() {
 screens->DeleteAll();
}

void SettingSetDateTime::Advance() {
  screens->OnTouchEvent(Applications::TouchEvents::SwipeUp);
}
