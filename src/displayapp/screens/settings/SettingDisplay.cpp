#include "SettingDisplay.h"
#include "displayapp/widgets/Container.h"
#include "systemtask/SystemTask.h"
#include "displayapp/screens/Styles.h"
#include "displayapp/screens/Symbols.h"

using namespace Pinetime::Applications::Screens;

SettingDisplay::SettingDisplay() : Screen(Apps::SettingDisplay) {
}

void SettingDisplay::Load() {
  running = true;
  Widgets::Container container;
  container.Create("Display timeout", Symbols::sun, LV_LAYOUT_PRETTY_TOP);
  container.UpdateHeight(160 / options.size());
  container.container->user_data = this;

  uint16_t timeout = System::SystemTask::displayApp->settingsController.GetScreenTimeOut();
  static char buffer[4];
  for (uint8_t i = 0; i < options.size(); i++) {
    lv_obj_t* cb = lv_checkbox_create(container.container, NULL);
    cb->user_data = (void*) &options[i];
    snprintf(buffer, sizeof(buffer), "%2" PRIu16 "s", options[i] / 1000);
    lv_checkbox_set_text(cb, buffer);
    lv_obj_set_event_cb(cb, buttonEventHandler);
    SetRadioButtonStyle(cb);
    if (timeout == options[i]) {
      lv_checkbox_set_checked(cb, true);
    }
  }
}

bool SettingDisplay::UnLoad() {
  if (running) {
    running = false;
    lv_obj_clean(lv_scr_act());
  }
  return false;
}

SettingDisplay::~SettingDisplay() {
  UnLoad();
  System::SystemTask::displayApp->settingsController.SaveSettings();
}

void SettingDisplay::onButtonEvent(lv_obj_t* obj) {
  uint16_t timeout = *static_cast<uint16_t*>(obj->user_data);
  System::SystemTask::displayApp->settingsController.SetScreenTimeOut(timeout);
  lv_obj_t* cb = NULL;
  while ((cb = lv_obj_get_child(obj->parent, cb))) {
    lv_checkbox_set_checked(cb, *static_cast<uint16_t*>(cb->user_data) == timeout);
  }
}

void SettingDisplay::buttonEventHandler(lv_obj_t* obj, lv_event_t event) {
  if (event == LV_EVENT_CLICKED)
    static_cast<SettingDisplay*>(obj->parent->user_data)->onButtonEvent(obj);
}
