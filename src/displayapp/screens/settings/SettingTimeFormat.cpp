#include "SettingTimeFormat.h"
#include "systemtask/SystemTask.h"
#include "displayapp/screens/Symbols.h"

using namespace Pinetime::Applications::Screens;

SettingTimeFormat::SettingTimeFormat()
  : Screen(Apps::SettingTimeFormat),
    checkboxList(
      options.size(),
      "Time format",
      Symbols::clock,
      [this]() {
        return getCurrentOption(System::SystemTask::displayApp->settingsController.GetClockType());
      },
      [](uint8_t index) {
        System::SystemTask::displayApp->settingsController.SetClockType(options[index].clockType);
      }) {
  uint8_t i = 0;
  while (checkboxList.Add({options[i].name, true, i++})) {
  };
}

void SettingTimeFormat::Load() {
  running = true;
  checkboxList.Load();
}

bool SettingTimeFormat::UnLoad() {
  if (running) {
    running = false;
    checkboxList.UnLoad();
  }
  return true;
}

SettingTimeFormat::~SettingTimeFormat() {
  UnLoad();
  System::SystemTask::displayApp->settingsController.SaveSettings();
}

uint8_t SettingTimeFormat::getCurrentOption(Controllers::Settings::ClockType currentOption) {
  for (uint8_t i = 0; i < options.size(); i++) {
    if (options[i].clockType == currentOption) {
      return i;
    }
  }
  return 0;
}
