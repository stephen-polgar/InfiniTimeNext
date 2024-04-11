#include "ApplicationList.h"

using namespace Pinetime::Applications::Screens;

ApplicationList::ApplicationList(std::array<Tile::Applications, UserAppTypes::Count>& apps)
  : Screen(Apps::Launcher) {
  uint8_t i = 0;
  while (i < apps.size()) {
    Screens::Tile* list = new Screens::Tile(pageIndicator.nScreens++, &pageIndicator);
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

void ApplicationList::Load() {
  running = true;
  screens->GetCurrent()->Load();
}

bool ApplicationList::UnLoad() {
  if (running) {
    running = false;
    screens->GetCurrent()->UnLoad();
  }
  return true;
}

ApplicationList::~ApplicationList() {
  screens->DeleteAll();
}

bool ApplicationList::OnTouchEvent(Applications::TouchEvents event) {
  return screens->OnTouchEvent(event);
}
