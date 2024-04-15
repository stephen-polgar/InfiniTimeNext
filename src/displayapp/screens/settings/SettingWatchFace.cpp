#include "SettingWatchFace.h"
#include "systemtask/SystemTask.h"

using namespace Pinetime::Applications::Screens;

SettingWatchFace::SettingWatchFace(std::array<Screens::SettingWatchFace::Item, UserWatchFaceTypes::Count>&& watchfaceItems)
  : Screen(Apps::SettingWatchFace), watchfaces {std::move(watchfaceItems)} {
  uint8_t i = 0;
  while (i < watchfaceItems.size()) {
    Screens::CheckboxList* list = new Screens::CheckboxList(
      MaxCheckboxItems,
      title,
      symbol,
      [this]() {
        return indexOf(System::SystemTask::displayApp->settingsController.GetWatchFace());
      },
      [this](uint8_t index) {
        System::SystemTask::displayApp->settingsController.SetWatchFace(watchfaces[index].watchface);
        System::SystemTask::displayApp->StartApp(Apps::Clock, Screen::FullRefreshDirections::None);
      },
      LV_LAYOUT_COLUMN_LEFT,
      &pageIndicator);
    if (i)
      screens->Add(list);
    else
      screens = list;    
    while (list->Add({watchfaceItems[i].name, watchfaceItems[i].enabled, i++})) {
      if (i == watchfaceItems.size())
        return;
    }
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

uint8_t SettingWatchFace::indexOf(const WatchFace watchface) {
  uint8_t i = 0;
  for (auto& item : watchfaces) {
    if (item.watchface == watchface)
      return i;
    i++;
  }
  return 0;
}
