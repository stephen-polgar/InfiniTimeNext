#include "SettingWeatherFormat.h"
#include "displayapp/widgets/Container.h"
#include "systemtask/SystemTask.h"
#include "displayapp/screens/Symbols.h"
#include "displayapp/screens/Styles.h"

using namespace Pinetime::Applications::Screens;

SettingWeatherFormat::SettingWeatherFormat() : Screen(Apps::SettingWeatherFormat) {
 
}

void SettingWeatherFormat::Load() {
  running = true;
  Widgets::Container container;
  container.Create("Weather format", Symbols::cloudSunRain);
  container.UpdateHeight(80 / options.size());
  container.container->user_data = this;

  Controllers::Settings::WeatherFormat format = System::SystemTask::displayApp->settingsController.GetWeatherFormat();

  for (uint8_t i = 0; i < options.size(); i++) {
    lv_obj_t* cb = lv_checkbox_create(container.container, NULL);
    cb->user_data = (void*) &options[i].weatherFormat;
    lv_checkbox_set_text(cb, options[i].name);
    lv_obj_set_event_cb(cb, buttonEventHandler);
    SetRadioButtonStyle(cb);
    if (format == options[i].weatherFormat) {
      lv_checkbox_set_checked(cb, true);
    }
  }
}

bool SettingWeatherFormat::UnLoad() {
  if (running) {
    running = false;
    lv_obj_clean(lv_scr_act());
  }
  return true;
}

SettingWeatherFormat::~SettingWeatherFormat() {
  UnLoad();
  System::SystemTask::displayApp->settingsController.SaveSettings();
}

void SettingWeatherFormat::onButtonEvent(lv_obj_t* obj) {
  Controllers::Settings::WeatherFormat format = *static_cast<Controllers::Settings::WeatherFormat*>(obj->user_data);
  System::SystemTask::displayApp->settingsController.SetWeatherFormat(format);
  for (uint8_t i = 0; i < options.size(); i++) {
    lv_obj_t* cb = NULL;
    while ((cb = lv_obj_get_child(obj->parent, cb))) {
      lv_checkbox_set_checked(cb, *static_cast<Controllers::Settings::WeatherFormat*>(cb->user_data) == format);
    }
  }
}

void SettingWeatherFormat::buttonEventHandler(lv_obj_t* obj, lv_event_t event) {
  if (event == LV_EVENT_CLICKED)
    static_cast<SettingWeatherFormat*>(obj->parent->user_data)->onButtonEvent(obj);
}
