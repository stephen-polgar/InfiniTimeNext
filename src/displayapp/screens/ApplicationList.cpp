#include "ApplicationList.h"

using namespace Pinetime::Applications::Screens;

// System::SystemTask::displayApp->settingsController.GetAppMenu()
ApplicationList::ApplicationList(std::array<Tile::Applications, UserAppTypes::Count>&& apps)
  : Screen(Apps::Launcher), apps {std::move(apps)} {
  for (uint8_t i = 0; i < nScreens; i++) {
    screens.Add(CreateScreen(i));
  }
}

void ApplicationList::Load() {
  running = true;
  screens.Load();
}

bool ApplicationList::UnLoad() {
  if (running) {
    running = false;
    screens.UnLoad();
  }
  return true;
}

ApplicationList::~ApplicationList() {
  UnLoad();
}

bool ApplicationList::OnTouchEvent(Applications::TouchEvents event) {
  return screens.OnTouchEvent(event);
}

Screen* ApplicationList::CreateScreen(uint8_t screenNum) {
  std::array<Tile::Applications, appsPerScreen> pageApps;
  for (uint8_t i = 0; i < appsPerScreen; i++) {
    if (i + (screenNum * appsPerScreen) >= (uint8_t)apps.size()) {
      pageApps[i] = {"", Applications::Apps::None, false};
    } else {
      pageApps[i] = apps[i + (screenNum * appsPerScreen)];
    }
  }
  return new Screens::Tile(screenNum, nScreens, pageApps, pageIndicator);
}
