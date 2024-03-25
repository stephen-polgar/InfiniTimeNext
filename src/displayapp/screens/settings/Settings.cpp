#include "Settings.h"

using namespace Pinetime::Applications::Screens;

constexpr std::array<List::Applications, Settings::entries.size()> Settings::entries;

Settings::Settings() : Screen(Apps::Settings) {
  for (uint8_t i = 0; i < nScreens; i++) {
    screens.Add(CreateScreen(i));
  }
}

void Settings::Load() {
  running = true;
  screens.Load();
}

bool Settings::UnLoad() {
  if (running) {
    running = false;
    screens.UnLoad();
  }
  return true;
}

Settings::~Settings() {
  UnLoad();
}

bool Settings::OnTouchEvent(Applications::TouchEvents event) {
  return screens.OnTouchEvent(event);
}

Screen* Settings::CreateScreen(uint8_t screenNum)  {
  std::array<List::Applications, entriesPerScreen> screens;
  for (int i = 0; i < entriesPerScreen; i++) {
    screens[i] = entries[screenNum * entriesPerScreen + i];
  }
  return new Screens::List(screenNum, nScreens, screens, pageIndicator);
}
