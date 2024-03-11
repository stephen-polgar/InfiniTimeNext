#include "SettingWeatherFormat.h"
#include "systemtask/SystemTask.h"
#include "displayapp/screens/Symbols.h"

using namespace Pinetime::Applications::Screens;

SettingWeatherFormat::SettingWeatherFormat()
  : Screen(Apps::SettingWeatherFormat),
    checkboxList(
      0,
      1,
      "Weather format",
      Symbols::cloudSunRain,
      GetDefaultOption(System::SystemTask::displayApp->settingsController.GetWeatherFormat()),
      [&settings = System::SystemTask::displayApp->settingsController](uint32_t index) {
        settings.SetWeatherFormat(options[index].weatherFormat);
        settings.SaveSettings();
      },
      CreateOptionArray()) {
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
}

std::array<CheckboxList::Item, CheckboxList::MaxItems> SettingWeatherFormat::CreateOptionArray() {
  std::array<Applications::Screens::CheckboxList::Item, CheckboxList::MaxItems> optionArray;
  for (size_t i = 0; i < CheckboxList::MaxItems; i++) {
    if (i >= options.size()) {
      optionArray[i].name = "";
      optionArray[i].enabled = false;
    } else {
      optionArray[i].name = options[i].name;
      optionArray[i].enabled = true;
    }
  }
  return optionArray;
}

uint32_t SettingWeatherFormat::GetDefaultOption(Controllers::Settings::WeatherFormat currentOption) {
  for (size_t i = 0; i < options.size(); i++) {
    if (options[i].weatherFormat == currentOption) {
      return i;
    }
  }
  return 0;
}