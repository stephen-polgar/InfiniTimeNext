#include "SettingWatchFace.h"
#include "systemtask/SystemTask.h"

constexpr const char* SettingWatchFace::title;
constexpr const char* SettingWatchFace::symbol;

using namespace Pinetime::Applications::Screens;

SettingWatchFace::SettingWatchFace(std::array<Screens::SettingWatchFace::Item, UserWatchFaceTypes::Count>&& watchfaceItems)
  : Screen(Apps::SettingWatchFace), watchfaceItems {std::move(watchfaceItems)} {
  for (uint8_t i = 0; i < nScreens; i++) {
    screens.Add(createScreen(i));
  }
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

Screen* SettingWatchFace::createScreen(uint8_t screenNum) {
  std::array<Screens::CheckboxList::Item, CheckboxList::MaxItems> watchfacesOnThisScreen;
  for (uint8_t i = 0; i < CheckboxList::MaxItems; i++) {
    if (i + (screenNum * CheckboxList::MaxItems) >= (uint8_t) watchfaceItems.size()) {
      watchfacesOnThisScreen[i] = {NULL, false};
    } else {
      auto& item = watchfaceItems[i + (screenNum * CheckboxList::MaxItems)];
      watchfacesOnThisScreen[i] = Screens::CheckboxList::Item {item.name, item.enabled};
    }
  }
  return new Screens::CheckboxList(
    screenNum,
    nScreens,
    title,
    symbol,
    [this]() {
      return indexOf(watchfaceItems, System::SystemTask::displayApp->settingsController.GetWatchFace());
    },
    [this, &settings = System::SystemTask::displayApp->settingsController](uint8_t index) {
      settings.SetWatchFace(indexToWatchFace(watchfaceItems, index));
      settings.SaveSettings();
      System::SystemTask::displayApp->StartApp(Apps::Clock, Screen::FullRefreshDirections::None);
    },
    watchfacesOnThisScreen,
    pageIndicator);
}

uint8_t SettingWatchFace::indexOf(const std::array<SettingWatchFace::Item, UserWatchFaceTypes::Count>& watchfaces, WatchFace watchface) {
  uint8_t index = 0;
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
SettingWatchFace::indexToWatchFace(const std::array<SettingWatchFace::Item, UserWatchFaceTypes::Count>& watchfaces, uint8_t index) {
  if (index >= watchfaces.size()) {
    return watchfaces[0].watchface;
  }
  return watchfaces[index].watchface;
}
