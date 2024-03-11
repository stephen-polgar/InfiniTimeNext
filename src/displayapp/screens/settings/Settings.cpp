#include "Settings.h"

using namespace Pinetime::Applications::Screens;

constexpr std::array<List::Applications, Settings::entries.size()> Settings::entries;

auto Settings::CreateScreenList() const {
  std::array<std::function<std::unique_ptr<Screen>()>, nScreens> screens;
  for (size_t i = 0; i < screens.size(); i++) {
    screens[i] = [this, i]() -> std::unique_ptr<Screen> {
      return CreateScreen(i);
    };
  }
  return screens;
}

Settings::Settings() : Screen(Apps::Settings),
   screens {CreateScreenList(), System::SystemTask::displayApp->settingsController.GetSettingsMenu()} {
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

bool Settings::OnTouchEvent(Pinetime::Applications::TouchEvents event) {
  return screens.OnTouchEvent(event);
}

std::unique_ptr<Screen> Settings::CreateScreen(unsigned int screenNum) const {
  std::array<List::Applications, entriesPerScreen> screens;
  for (int i = 0; i < entriesPerScreen; i++) {
    screens[i] = entries[screenNum * entriesPerScreen + i];
  }
  return std::make_unique<Screens::List>(screenNum, nScreens, screens);
}
