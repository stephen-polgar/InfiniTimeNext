#include "SettingTimeFormat.h"
#include "systemtask/SystemTask.h"
#include "displayapp/screens/Symbols.h"

using namespace Pinetime::Applications::Screens;

SettingTimeFormat::SettingTimeFormat()
  : Screen(Apps::SettingTimeFormat),
    checkboxList(
      0,
      1,
      "Time format",
      Symbols::clock,
      [this]() {
        return GetDefaultOption(System::SystemTask::displayApp->settingsController.GetClockType());
      },
      [&settings = System::SystemTask::displayApp->settingsController](uint32_t index) {
        settings.SetClockType(options[index].clockType);
        settings.SaveSettings();
      },
      CreateOptionArray(),
      pageIndicator) {
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
}

std::array<CheckboxList::Item, CheckboxList::MaxItems> SettingTimeFormat::CreateOptionArray() {
  std::array<Applications::Screens::CheckboxList::Item, CheckboxList::MaxItems> optionArray;
  for (size_t i = 0; i < CheckboxList::MaxItems; i++) {
    if (i >= options.size()) {
      optionArray[i].name = NULL;
      optionArray[i].enabled = false;
    } else {
      optionArray[i].name = options[i].name;
      optionArray[i].enabled = true;
    }
  }
  return optionArray;
}

uint8_t SettingTimeFormat::GetDefaultOption(Controllers::Settings::ClockType currentOption) {
  for (uint8_t i = 0; i < options.size(); i++) {
    if (options[i].clockType == currentOption) {
      return i;
    }
  }
  return 0;
}
