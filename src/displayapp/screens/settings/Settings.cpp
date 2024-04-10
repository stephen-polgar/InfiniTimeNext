#include "Settings.h"

using namespace Pinetime::Applications::Screens;

constexpr std::array<List::Applications, Settings::entries.size()> Settings::entries;

Settings::Settings() : Screen(Apps::Settings) {
  for (uint8_t i = 0; i < nScreens; i++) {
    if (i)
    screens->Add(createScreen(i));
    else screens = createScreen(i);
  }
}

void Settings::Load() { 
  screens->GetCurrent()->Load();
  running = true;;
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

ScreenTree* Settings::createScreen(uint8_t screenNum)  {
  std::array<List::Applications, entriesPerScreen> screens;
  for (uint8_t i = 0; i < entriesPerScreen; i++) {
    screens[i] = entries[screenNum * entriesPerScreen + i];
  }
  return new Screens::List(screenNum, screens, pageIndicator);
}
