#include "Settings.h"

using namespace Pinetime::Applications::Screens;

Settings::Settings() : Screen(Apps::Settings) {
  uint8_t i = 0;
  while (i < apps.size()) {
    Screens::List* list = new Screens::List(pageIndicator.nScreens++, pageIndicator);
    if (i)
      screens->Add(list);
    else
      screens = list;
    while (list->Add(apps[i++])) {
      if (i == apps.size())
        return;
    }
  }
}

void Settings::Load() {
  screens->GetCurrent()->Load();
  running = true;
}

bool Settings::UnLoad() {
  if (running) {
    running = false;
    screens->GetCurrent()->UnLoad();
  }
  return true;
}

Settings::~Settings() {
  screens->DeleteAll();
}

bool Settings::OnTouchEvent(Applications::TouchEvents event) {
  return screens->OnTouchEvent(event);
}
