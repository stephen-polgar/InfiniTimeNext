#include "displayapp/screens/Screen.h"

using namespace Pinetime::Applications::Screens;

void Screen::RefreshTaskCallback(lv_task_t* task) {
  static_cast<Screen*>(task->user_data)->Refresh();
}

Screen::Screen(Apps Id) : Id {Id} { 
}

Screen::Screen(WatchFace Id) : Id {Apps(Id)} {
}

bool Screen::operator==(const Screen& other) const {
  return Id == other.Id && other.Id != Apps::None;
}

bool Screen::operator==(const Apps other) const {
  return Id == other && other != Apps::None;
}