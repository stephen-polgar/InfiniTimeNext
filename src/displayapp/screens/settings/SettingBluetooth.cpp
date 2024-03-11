#include "SettingBluetooth.h"
#include "systemtask/SystemTask.h"
#include "displayapp/screens/Symbols.h"

using namespace Pinetime::Applications::Screens;

SettingBluetooth::SettingBluetooth() : Screen(Apps::SettingBluetooth) {
}

void SettingBluetooth::Load() {
  running = true;
  lv_obj_t* title = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text_static(title, "Bluetooth");
  lv_label_set_align(title, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(title, lv_scr_act(), LV_ALIGN_IN_TOP_MID, 10, 15);

  lv_obj_t* icon = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(icon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
  lv_label_set_text_static(icon, Symbols::bluetooth);
  lv_label_set_align(icon, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(icon, title, LV_ALIGN_OUT_LEFT_MID, -8, 0);

  lv_obj_t* cb = lv_checkbox_create(lv_scr_act(), NULL);
  lv_checkbox_set_text_static(cb, "Enable");
  lv_obj_align(cb, title, LV_ALIGN_OUT_BOTTOM_MID, -8, 20);
  lv_checkbox_set_checked(cb, System::SystemTask::displayApp->settingsController.GetBleRadioEnabled());
  lv_obj_set_event_cb(cb, [](lv_obj_t* obj, lv_event_t event) {
    if (event == LV_EVENT_VALUE_CHANGED) {
      System::SystemTask::displayApp->settingsController.SetBleRadioEnabled(lv_checkbox_is_checked(obj));
      System::SystemTask::displayApp->PushMessage(Applications::Display::Messages::BleRadioEnableToggle);
    }
  });
}

bool SettingBluetooth::UnLoad() {
  if (running) {
    running = false;
    lv_obj_clean(lv_scr_act());   
  }
  return true;
}

SettingBluetooth::~SettingBluetooth() {
  UnLoad();
}
