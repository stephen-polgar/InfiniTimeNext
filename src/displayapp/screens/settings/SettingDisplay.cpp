#include "SettingDisplay.h"
#include "systemtask/SystemTask.h"
#include "displayapp/screens/Styles.h"
#include "displayapp/screens/Symbols.h"

using namespace Pinetime::Applications::Screens;

constexpr std::array<uint16_t, 6> SettingDisplay::options;

SettingDisplay::SettingDisplay() : Screen(Apps::SettingDisplay) {
}

void SettingDisplay::Load() {
  running = true;
  lv_obj_t* container1 = lv_cont_create(lv_scr_act(), nullptr);

  lv_obj_set_style_local_bg_opa(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
  lv_obj_set_style_local_pad_all(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 10);
  lv_obj_set_style_local_pad_inner(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 5);
  lv_obj_set_style_local_border_width(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);

  lv_obj_set_pos(container1, 10, 60);
  lv_obj_set_width(container1, LV_HOR_RES - 20);
  lv_obj_set_height(container1, LV_VER_RES - 50);
  lv_cont_set_layout(container1, LV_LAYOUT_PRETTY_TOP);

  lv_obj_t* title = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(title, "Display timeout");
  lv_label_set_align(title, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(title, lv_scr_act(), LV_ALIGN_IN_TOP_MID, 10, 15);

  lv_obj_t* icon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(icon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
  lv_label_set_text_static(icon, Symbols::sun);
  lv_label_set_align(icon, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(icon, title, LV_ALIGN_OUT_LEFT_MID, -10, 0);

  char buffer[4];
  for (uint8_t i = 0; i < options.size(); i++) {
    cbOption[i] = lv_checkbox_create(container1, nullptr);
    snprintf(buffer, sizeof(buffer), "%2" PRIu16 "s", options[i] / 1000);
    lv_checkbox_set_text(cbOption[i], buffer);
    cbOption[i]->user_data = this;
    lv_obj_set_event_cb(cbOption[i], event_handlerGoal);
    SetRadioButtonStyle(cbOption[i]);

    if (System::SystemTask::displayApp->settingsController.GetScreenTimeOut() == options[i]) {
      lv_checkbox_set_checked(cbOption[i], true);
    }
  }
}

bool SettingDisplay::UnLoad() {
  if (running) {
    running = false;
    lv_obj_clean(lv_scr_act());
  }
  return false;
}

SettingDisplay::~SettingDisplay() {
  UnLoad();
  System::SystemTask::displayApp->settingsController.SaveSettings();
}

void SettingDisplay::updateSelected(lv_obj_t* object) {
  for (uint8_t i = 0; i < options.size(); i++) {
    if (object == cbOption[i]) {
      lv_checkbox_set_checked(cbOption[i], true);
      System::SystemTask::displayApp->settingsController.SetScreenTimeOut(options[i]);
      System::SystemTask::displayApp->StartApp(Apps::Clock, Screen::FullRefreshDirections::None);     
    } else {
      lv_checkbox_set_checked(cbOption[i], false);
    }
  }
}

void SettingDisplay::event_handlerGoal(lv_obj_t* obj, lv_event_t event) {
  if (event == LV_EVENT_CLICKED) {
    static_cast<SettingDisplay*>(obj->user_data)->updateSelected(obj);
  }
}
