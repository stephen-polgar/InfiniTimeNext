#include "SettingWeatherFormat.h"
#include "systemtask/SystemTask.h"
#include "displayapp/screens/Symbols.h"

using namespace Pinetime::Applications::Screens;

SettingWeatherFormat::SettingWeatherFormat()
  : Screen(Apps::SettingWeatherFormat),
    checkboxList(
      options.size(),
      "Weather format",
      Symbols::cloudSunRain,
      [this]() {
        return getCurrentOption(System::SystemTask::displayApp->settingsController.GetWeatherFormat());
      },
      [](uint8_t index) {
        System::SystemTask::displayApp->settingsController.SetWeatherFormat(options[index].weatherFormat);
      }) { 
  for (uint8_t i = 0; i < options.size(); i++) {
    checkboxList.Add({options[i].name, true, i});
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