#include "displayapp/screens/Screen.h"

using namespace Pinetime::Applications::Screens;

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

bool Screen::UnLoad() {
  if (running) {   
    running = false;
    lv_obj_clean(lv_scr_act());
  }
  return Id != Apps::None;
}

Screen::~Screen() {
  UnLoad();
}