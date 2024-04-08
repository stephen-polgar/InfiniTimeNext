#include "SettingWeatherFormat.h"
#include "systemtask/SystemTask.h"
#include "displayapp/screens/Symbols.h"

using namespace Pinetime::Applications::Screens;

SettingWeatherFormat::SettingWeatherFormat()
  : Screen(Apps::SettingWeatherFormat),
    checkboxList(
      "Weather format",
      Symbols::cloudSunRain,
      [this](uint8_t) {
        return getCurrentOption(System::SystemTask::displayApp->settingsController.GetWeatherFormat());
      },
      [](uint8_t, uint8_t index) {
        System::SystemTask::displayApp->settingsController.SetWeatherFormat(options[index].weatherFormat);
      }) {

  for (uint8_t i = 0; i < options.size(); i++) {
    CheckboxList::Item item;
    item.name = options[i].name;
    item.enabled = true;
    checkboxList.Add(item);
  }
}

void SettingWeatherFormat::Load() {
  running = true;
  checkboxList.Load();
}

bool SettingWeatherFormat::UnLoad() {
  if (running) {
    running = false;
    checkboxList.UnLoad();
  }
  return true;
}

SettingWeatherFormat::~SettingWeatherFormat() {
  UnLoad();
  System::SystemTask::displayApp->settingsController.SaveSettings();
}

uint8_t SettingWeatherFormat::getCurrentOption(Controllers::Settings::WeatherFormat currentOption) {
  for (uint8_t i = 0; i < options.size(); i++) {
    if (options[i].weatherFormat == currentOption) {
      return i;
    }
  }
  return 0;
}