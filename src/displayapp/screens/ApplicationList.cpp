#include "ApplicationList.h"

using namespace Pinetime::Applications::Screens;

auto ApplicationList::CreateScreenList() const {
  std::array<std::function<std::unique_ptr<Screen>()>, nScreens> screens;
  for (size_t i = 0; i < screens.size(); i++) {
    screens[i] = [this, i]() -> std::unique_ptr<Screen> { return CreateScreen(i);  };
  }
  return screens;
}

ApplicationList::ApplicationList(std::array<Tile::Applications, UserAppTypes::Count>&& apps) : Screen(Apps::Launcher), apps {std::move(apps)},
    screens {CreateScreenList(), System::SystemTask::displayApp->settingsController.GetAppMenu()} {
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

std::unique_ptr<Screen> ApplicationList::CreateScreen(unsigned int screenNum) const {
  std::array<Tile::Applications, appsPerScreen> pageApps;

  for (int i = 0; i < appsPerScreen; i++) {
    if (i + (screenNum * appsPerScreen) >= apps.size()) {
      pageApps[i] = {"", Applications::Apps::None, false};
    } else {
      pageApps[i] = apps[i + (screenNum * appsPerScreen)];
    }
  }

  return std::make_unique<Screens::Tile>(screenNum, nScreens, pageApps);
}
