#include "SettingWatchFace.h"
#include "systemtask/SystemTask.h"
#include "displayapp/screens/Symbols.h"
#include "displayapp/screens/Styles.h"

using namespace Pinetime::Applications::Screens;

SettingWatchFace::SettingWatchFace(std::array<Screens::SettingWatchFace::Item, UserWatchFaceTypes::Count>&& watchfaceItems)
  : Screen(Apps::SettingWatchFace),
    watchfaces {std::move(watchfaceItems)},
    arrayTouchHandler {UserWatchFaceTypes::Count,
                       maxItems,
                       [this](uint8_t indexBegin, uint8_t indexEnd, Screen::FullRefreshDirections direction) {
                         load(indexBegin, indexEnd, direction);
                       }}, pageIndicator{uint8_t(watchfaces.size() / maxItems + 1)} {
}

void SettingWatchFace::Load() {
  container.Create("Watch face", Symbols::home);
  container.container->user_data = this;
  watchface = System::SystemTask::displayApp->settingsController.GetWatchFace();
  arrayTouchHandler.LoadCurrentPos();
  running = true;
}

void SettingWatchFace::load(uint8_t indexBegin, uint8_t indexEnd, Screen::FullRefreshDirections direction) {
  if (running) {
    unload();
    System::SystemTask::displayApp->SetFullRefresh(direction);
  }
  pageIndicator.Load(indexBegin >= maxItems ? indexBegin / maxItems : 0);
  for (uint8_t i = indexBegin; i < indexEnd; i++) {
    lv_obj_t* cb = lv_checkbox_create(container.container, NULL);
    cb->user_data = &watchfaces[i].watchface;
    lv_checkbox_set_text(cb, watchfaces[i].name);
    lv_obj_set_event_cb(cb, buttonEventHandler);
    SetRadioButtonStyle(cb);
    if (!watchfaces[i].enabled)
      lv_checkbox_set_disabled(cb);
    if (watchface == watchfaces[i].watchface)
      lv_checkbox_set_checked(cb, true);
  }
  container.UpdateHeight(80 / (indexEnd - indexBegin));
}

bool SettingWatchFace::UnLoad() {
  if (running) {
    running = false;
    lv_obj_clean(lv_scr_act());
  }
  return true;
}

SettingWatchFace::~SettingWatchFace() {
  UnLoad();
  System::SystemTask::displayApp->settingsController.SaveSettings();
}

bool SettingWatchFace::OnTouchEvent(TouchEvents event) {
  return arrayTouchHandler.OnTouchEvent(event); 
}

void SettingWatchFace::onButtonEvent(lv_obj_t* obj) {
  WatchFace watchface = *static_cast<WatchFace*>(obj->user_data);
  if (this->watchface != watchface) {
    System::SystemTask::displayApp->settingsController.SetWatchFace(watchface);
    lv_obj_t* cb = NULL;
    while ((cb = lv_obj_get_child(obj->parent, cb))) {
      lv_checkbox_set_checked(cb, *static_cast<WatchFace*>(cb->user_data) == watchface);
    }
    System::SystemTask::displayApp->StartApp(Apps::Clock, Screen::FullRefreshDirections::None);
  } else
    lv_checkbox_set_checked(obj, true);
}

void SettingWatchFace::unload() {
  pageIndicator.UnLoad();
  lv_obj_t* cb = NULL;
  while ((cb = lv_obj_get_child(container.container, cb))) {
    lv_obj_del(cb);
  }
}

void SettingWatchFace::buttonEventHandler(lv_obj_t* obj, lv_event_t event) {
  if (event == LV_EVENT_CLICKED)
    static_cast<SettingWatchFace*>(obj->parent->user_data)->onButtonEvent(obj);
}
