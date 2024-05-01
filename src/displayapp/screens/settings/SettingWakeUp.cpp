#include "SettingWakeUp.h"
#include "systemtask/SystemTask.h"
#include "displayapp/screens/Symbols.h"

using namespace Pinetime::Applications::Screens;

SettingWakeUp::SettingWakeUp() : Screen(Apps::SettingWakeUp) {
}

void SettingWakeUp::Load() {
  running = true;
  lv_obj_t* container = lv_cont_create(lv_scr_act(), nullptr);

  lv_obj_set_style_local_bg_opa(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
  lv_obj_set_style_local_pad_all(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 10);
  lv_obj_set_style_local_pad_inner(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 5);
  lv_obj_set_style_local_border_width(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);

  lv_obj_set_pos(container, 10, 35);
  lv_obj_set_width(container, LV_HOR_RES - 20);
  lv_obj_set_height(container, LV_VER_RES - 20);
  lv_cont_set_layout(container, LV_LAYOUT_COLUMN_LEFT);

  lv_obj_t* title = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(title, "Wake Up");
  lv_label_set_align(title, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(title, lv_scr_act(), LV_ALIGN_IN_TOP_MID, 15, 15);

  lv_obj_t* icon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(icon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
  lv_label_set_text_static(icon, Symbols::eye);
  lv_label_set_align(icon, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(icon, title, LV_ALIGN_OUT_LEFT_MID, -10, 0);
  container->user_data = this;

  for (uint8_t i = 0; i < options.size(); i++) {
    cbOption[i] = lv_checkbox_create(container, nullptr);
    lv_checkbox_set_text_static(cbOption[i], options[i].name);
    if (System::SystemTask::displayApp->settingsController.isWakeUpModeOn(Controllers::Settings::WakeUpMode(i))) {
      lv_checkbox_set_checked(cbOption[i], true);
    }
    lv_obj_set_event_cb(cbOption[i], event_handler);
  }
}

SettingWakeUp::~SettingWakeUp() {
  UnLoad();
  System::SystemTask::displayApp->settingsController.SaveSettings();
}

void SettingWakeUp::event_handler(lv_obj_t* obj, lv_event_t event) {
  if (event == LV_EVENT_VALUE_CHANGED) {
    static_cast<SettingWakeUp*>(obj->parent->user_data)->updateSelected(obj);
  }
}

void SettingWakeUp::updateSelected(lv_obj_t* object) {
  // Find the index of the checkbox that triggered the event
  for (uint8_t i = 0; i < options.size(); i++) {
    if (cbOption[i] == object) {
      bool currentState = System::SystemTask::displayApp->settingsController.isWakeUpModeOn(options[i].wakeUpMode);
      System::SystemTask::displayApp->settingsController.setWakeUpMode(options[i].wakeUpMode, !currentState);
      break;
    }
  }

  // Update checkbox according to current wakeup modes.
  // This is needed because we can have extra logic when setting or unsetting wakeup modes,
  // for example, when setting SingleTap, DoubleTap is unset and vice versa.
  auto modes = System::SystemTask::displayApp->settingsController.getWakeUpModes();
  for (uint8_t i = 0; i < options.size(); ++i) {
    lv_checkbox_set_checked(cbOption[i], modes[i]);
  }
}
