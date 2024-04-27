#include "SettingWatchFace.h"
#include "systemtask/SystemTask.h"
#include "displayapp/screens/Symbols.h"

// #include <nrf_log.h>

using namespace Pinetime::Applications::Screens;

SettingWatchFace::SettingWatchFace(std::array<Screens::SettingWatchFace::Item, UserWatchFaceTypes::Count>&& watchfaceItems)
  : Screen(Apps::SettingWatchFace),
    watchfaces {std::move(watchfaceItems)},
    arrayTouchHandler {maxItems,
                       [this](uint8_t indexBegin, uint8_t indexEnd, Screen::FullRefreshDirections direction) {
                         load(indexBegin, indexEnd, direction);
                       },
                       UserWatchFaceTypes::Count},
    pageIndicator {UserWatchFaceTypes::Count / maxItems + 1} {
}

void SettingWatchFace::Load() {
  container.Load("Watch face", Symbols::home);
  container.Container->user_data = this;
  currentWatchface = System::SystemTask::displayApp->settingsController.GetWatchFace();
  for (uint8_t i = 0; i < maxItems; i++) {
    lv_obj_t* cb = lv_checkbox_create(container.Container, NULL);
    lv_obj_set_event_cb(cb, buttonEventHandler);
  }
  arrayTouchHandler.LoadCurrentPos();
  running = true;
}

void SettingWatchFace::load(uint8_t indexBegin, uint8_t indexEnd, Screen::FullRefreshDirections direction) {
  if (running) {
    System::SystemTask::displayApp->SetFullRefresh(direction);
  }
  pageIndicator.Load(indexBegin >= maxItems ? indexBegin / maxItems : 0);
  container.UpdateHeight(80 / (indexEnd - indexBegin));
  lv_obj_t* cb = NULL;
  while (indexEnd-- > indexBegin) {
    cb = lv_obj_get_child(container.Container, cb);
    lv_obj_set_hidden(cb, false);
    Item& item = watchfaces[indexEnd];
    cb->user_data = &item.watchface;
    lv_checkbox_set_text_static(cb, item.name);
    if (currentWatchface == item.watchface) {
      lv_checkbox_set_checked(cb, true);
      lv_btn_set_checkable(cb, false);
    } else {
      lv_checkbox_set_checked(cb, false);
      lv_btn_set_checkable(cb, item.enabled);
    }
  }
  while (cb) {
    cb = lv_obj_get_child(container.Container, cb);
    if (cb) {
      lv_obj_set_hidden(cb, true);
    }
  }
}

bool SettingWatchFace::UnLoad() {
  if (running) {
    running = false;
    pageIndicator.UnLoad();
    lv_obj_clean(lv_scr_act());
  }
  return true;
}

SettingWatchFace::~SettingWatchFace() {
  UnLoad();
  System::SystemTask::displayApp->settingsController.SaveSettings();
}

bool SettingWatchFace::OnTouchEvent(TouchEvents event) {
  enableEvent = event == TouchEvents::Tap;
  return arrayTouchHandler.OnTouchEvent(event);
}

void SettingWatchFace::onButtonEvent(lv_obj_t* obj) {
#ifdef NRF_LOG_INFO
 // NRF_LOG_INFO("SettingWatchFace::onButtonEvent %d", obj);
#endif
  if (enableEvent) {
#ifdef NRF_LOG_INFO
   // NRF_LOG_INFO("SettingWatchFace::onButtonEvent enabled %d", obj);
#endif
    WatchFace watchface = *static_cast<WatchFace*>(obj->user_data);
    System::SystemTask::displayApp->settingsController.SetWatchFace(watchface);
    lv_obj_t* cb = NULL;
    while ((cb = lv_obj_get_child(obj->parent, cb))) {
      if (lv_obj_is_visible(cb)) {
        lv_checkbox_set_checked(cb, *static_cast<WatchFace*>(cb->user_data) == watchface);
      }
    }
    System::SystemTask::displayApp->StartApp(Apps::Clock, Screen::FullRefreshDirections::None);
  } else
    lv_checkbox_set_checked(obj, !lv_checkbox_is_checked(obj));
}

void SettingWatchFace::buttonEventHandler(lv_obj_t* obj, lv_event_t event) {
  if (event == LV_EVENT_VALUE_CHANGED)
    static_cast<SettingWatchFace*>(obj->parent->user_data)->onButtonEvent(obj);
}
