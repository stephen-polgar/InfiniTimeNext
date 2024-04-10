#include "SettingDisplay.h"
#include "systemtask/SystemTask.h"
#include "displayapp/screens/Styles.h"
#include "displayapp/screens/Symbols.h"

using namespace Pinetime::Applications::Screens;

SettingDisplay::SettingDisplay()
  : Screen(Apps::SettingDisplay),
    checkboxList(
      "Display timeout",
      Symbols::sun,
      [this](uint8_t) {
        return getCurrentOption(System::SystemTask::displayApp->settingsController.GetScreenTimeOut());
      },
      [](uint8_t, uint8_t index) {
        System::SystemTask::displayApp->settingsController.SetScreenTimeOut(options[index]);
      },
      LV_LAYOUT_PRETTY_TOP) {
  checkboxList.Reserve(options.size());
  static char buffer[4];
  for (uint8_t i = 0; i < options.size(); i++) {
    snprintf(buffer, sizeof(buffer), "%2" PRIu16 "s", options[i] / 1000);
    checkboxList.Add({buffer, true});
  }
}

void SettingDisplay::Load() {
  running = true;
  checkboxList.Load();
}

bool SettingDisplay::UnLoad() {
  if (running) {
    running = false;
    checkboxList.UnLoad();
  }
  return false;
}

SettingDisplay::~SettingDisplay() {
  UnLoad();
  System::SystemTask::displayApp->settingsController.SaveSettings();
}

uint8_t SettingDisplay::getCurrentOption(uint32_t currentOption) {
  for (uint8_t i = 0; i < options.size(); i++) {
    if (options[i] == currentOption) {
      return i;
    }
  }
  return 0;
}