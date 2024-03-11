#include "SettingWatchFace.h"
#include "systemtask/SystemTask.h"
#include "displayapp/screens/CheckboxList.h"

constexpr const char* SettingWatchFace::title;
constexpr const char* SettingWatchFace::symbol;

using namespace Pinetime::Applications::Screens;


auto SettingWatchFace::createScreenList() const {
  std::array<std::function<std::unique_ptr<Screen>()>, nScreens> screens;
  for (size_t i = 0; i < screens.size(); i++) {
    screens[i] = [this, i]() -> std::unique_ptr<Screen> {
      return createScreen(i);
    };
  }
  return screens;
}

SettingWatchFace::SettingWatchFace(std::array<Screens::SettingWatchFace::Item, UserWatchFaceTypes::Count>&& watchfaceItems)
  : Screen(Apps::SettingWatchFace), watchfaceItems {std::move(watchfaceItems)}, screens {createScreenList()} {
}

void SettingWatchFace::Load() {
  running = true;
  screens.Load();
}

bool SettingWatchFace::UnLoad() {
  if (running) {
    running = false;
    screens.UnLoad();  
  }
  return true;
}

SettingWatchFace::~SettingWatchFace() {
  UnLoad();
}

bool SettingWatchFace::OnTouchEvent(TouchEvents event) {
  return screens.OnTouchEvent(event);
}

std::unique_ptr<Screen> SettingWatchFace::createScreen(unsigned int screenNum) const {
  std::array<Screens::CheckboxList::Item, settingsPerScreen> watchfacesOnThisScreen;
  for (int i = 0; i < settingsPerScreen; i++) {
    if (i + (screenNum * settingsPerScreen) >= watchfaceItems.size()) {
      watchfacesOnThisScreen[i] = {"", false};
    } else {
      auto& item = watchfaceItems[i + (screenNum * settingsPerScreen)];
      watchfacesOnThisScreen[i] = Screens::CheckboxList::Item {item.name, item.enabled};
    }
  }
  return std::make_unique<Screens::CheckboxList>(
    screenNum,
    nScreens,
    title,
    symbol,
    static_cast<uint32_t>(indexOf(watchfaceItems, System::SystemTask::displayApp->settingsController.GetWatchFace())),
    [this, &settings = System::SystemTask::displayApp->settingsController](uint32_t index) {
      settings.SetWatchFace(indexToWatchFace(watchfaceItems, index));
      settings.SaveSettings();
      System::SystemTask::displayApp->StartApp(Apps::Clock);
    },
    watchfacesOnThisScreen);
}

uint32_t SettingWatchFace::indexOf(const std::array<SettingWatchFace::Item, UserWatchFaceTypes::Count>& watchfaces, WatchFace watchface) {
  size_t index = 0;
  auto found = std::find_if(watchfaces.begin(), watchfaces.end(), [&index, &watchface](const SettingWatchFace::Item& item) {
    const bool result = item.watchface == watchface;
    if (!result) {
      index++;
    }
    return result;
  });
  if (found == watchfaces.end()) {
    index = 0;
  }
  return index;
}

Pinetime::Applications::WatchFace
SettingWatchFace::indexToWatchFace(const std::array<SettingWatchFace::Item, UserWatchFaceTypes::Count>& watchfaces, size_t index) {
  if (index >= watchfaces.size()) {
    return watchfaces[0].watchface;
  }
  return watchfaces[index].watchface;
}
