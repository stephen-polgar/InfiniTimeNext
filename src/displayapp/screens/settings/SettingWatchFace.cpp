#include "SettingWatchFace.h"
#include "systemtask/SystemTask.h"

using namespace Pinetime::Applications::Screens;

SettingWatchFace::SettingWatchFace(std::array<Screens::SettingWatchFace::Item, UserWatchFaceTypes::Count>&& watchfaceItems)
  : Screen(Apps::SettingWatchFace), watchfaces {std::move(watchfaceItems)} {
  for (uint8_t i = 0; i < nScreens; i++) {
    if (i)
      screens->Add(createScreen(i));
    else
      screens = createScreen(i);
  }
}

void SettingWatchFace::Load() {
  screens->GetCurrent()->Load();
  running = true;
}

bool SettingWatchFace::UnLoad() {
  if (running) {
    running = false;
    screens->GetCurrent()->UnLoad();
  }
  return true;
}

SettingWatchFace::~SettingWatchFace() {
  screens->DeleteAll();
  System::SystemTask::displayApp->settingsController.SaveSettings();
}

bool SettingWatchFace::OnTouchEvent(TouchEvents event) {
  return screens->OnTouchEvent(event);
}

ScreenTree* SettingWatchFace::createScreen(uint8_t screenNum) {
  CheckboxList* checkboxList = new Screens::CheckboxList(
    screenNum,
    &pageIndicator,
    title,
    symbol,
    [this](uint8_t screenId) {
      return indexOf(System::SystemTask::displayApp->settingsController.GetWatchFace()) - screenId * MaxCheckboxItems;
    },
    [this](uint8_t screenId, uint8_t index) {
      System::SystemTask::displayApp->settingsController.SetWatchFace(watchfaces[screenId * MaxCheckboxItems + index].watchface);
      System::SystemTask::displayApp->StartApp(Apps::Clock, Screen::FullRefreshDirections::None);
    });
  uint8_t n = watchfaces.size() - screenNum * MaxCheckboxItems;
  checkboxList->Reserve(n >= MaxCheckboxItems ? MaxCheckboxItems : n);
  n = 0;
  for (uint8_t i = (screenNum * MaxCheckboxItems); i < watchfaces.size() && n < MaxCheckboxItems; i++, n++) {
    auto& item = watchfaces[i];
    checkboxList->Add({item.name, item.enabled});
  }
  return checkboxList;
}

uint8_t SettingWatchFace::indexOf(const WatchFace watchface) {
  uint8_t i = 0;
  for (auto& item : watchfaces) {
    if (item.watchface == watchface)
      return i;
    i++;
  }
  return 0;
}
