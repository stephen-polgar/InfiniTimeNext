#include "SettingTimeFormat.h"
#include "displayapp/widgets/Container.h"
#include "systemtask/SystemTask.h"
#include "displayapp/screens/Symbols.h"
#include "displayapp/screens/Styles.h"

using namespace Pinetime::Applications::Screens;

SettingTimeFormat::SettingTimeFormat() : Screen(Apps::SettingTimeFormat) {
}

void SettingTimeFormat::Load() {
  running = true;
  Widgets::Container container;
  container.Create("Time format", Symbols::clock);
  container.UpdateHeight(80 / options.size());
  container.container->user_data = this;

  Controllers::Settings::ClockType format = System::SystemTask::displayApp->settingsController.GetClockType();

  for (uint8_t i = 0; i < options.size(); i++) {
    lv_obj_t* cb = lv_checkbox_create(container.container, NULL);
    cb->user_data = (void*) &options[i].clockType;
    lv_checkbox_set_text(cb, options[i].name);
    lv_obj_set_event_cb(cb, buttonEventHandler);
    SetRadioButtonStyle(cb);
    if (format == options[i].clockType) {
      lv_checkbox_set_checked(cb, true);
    }
  }
}

bool SettingTimeFormat::UnLoad() {
  if (running) {
    running = false;
    lv_obj_clean(lv_scr_act());
  }
  return true;
}

SettingTimeFormat::~SettingTimeFormat() {
  UnLoad();
  System::SystemTask::displayApp->settingsController.SaveSettings();
}

void SettingTimeFormat::onButtonEvent(lv_obj_t* obj) {
  Controllers::Settings::ClockType format = *static_cast<Controllers::Settings::ClockType*>(obj->user_data);
  System::SystemTask::displayApp->settingsController.SetClockType(format);
  lv_obj_t* cb = NULL;
  while ((cb = lv_obj_get_child(obj->parent, cb))) {
    lv_checkbox_set_checked(cb, *static_cast<Controllers::Settings::ClockType*>(cb->user_data) == format);
  }
}

void SettingTimeFormat::buttonEventHandler(lv_obj_t* obj, lv_event_t event) {
  if (event == LV_EVENT_CLICKED)
    static_cast<SettingTimeFormat*>(obj->parent->user_data)->onButtonEvent(obj);
}
