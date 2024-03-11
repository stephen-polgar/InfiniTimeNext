#include "SettingSteps.h"
#include "systemtask/SystemTask.h"
#include "displayapp/screens/Symbols.h"
#include "displayapp/InfiniTimeTheme.h"

using namespace Pinetime::Applications::Screens;

SettingSteps::SettingSteps() : Screen(Apps::SettingSteps) {
}

void SettingSteps::Load() {
  running = true;
  lv_obj_t* container1 = lv_cont_create(lv_scr_act(), nullptr);

  lv_obj_set_style_local_bg_opa(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
  lv_obj_set_style_local_pad_all(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 10);
  lv_obj_set_style_local_pad_inner(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 5);
  lv_obj_set_style_local_border_width(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_pos(container1, 30, 60);
  lv_obj_set_width(container1, LV_HOR_RES - 50);
  lv_obj_set_height(container1, LV_VER_RES - 60);
  lv_cont_set_layout(container1, LV_LAYOUT_COLUMN_LEFT);

  lv_obj_t* title = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(title, "Daily steps goal");
  lv_label_set_align(title, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(title, lv_scr_act(), LV_ALIGN_IN_TOP_MID, 15, 15);

  lv_obj_t* icon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(icon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_ORANGE);

  lv_label_set_text_static(icon, Symbols::shoe);
  lv_label_set_align(icon, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(icon, title, LV_ALIGN_OUT_LEFT_MID, -10, 0);

  stepGoal = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(stepGoal, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_42);
  lv_label_set_text_fmt(stepGoal, "%lu", System::SystemTask::displayApp->settingsController.GetStepsGoal());
  lv_label_set_align(stepGoal, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(stepGoal, lv_scr_act(), LV_ALIGN_IN_TOP_MID, 0, 50);

  static constexpr uint8_t btnSize = 40;

  btnMinus = lv_btn_create(lv_scr_act(), nullptr);
  btnMinus->user_data = this;
  lv_obj_set_size(btnMinus, btnSize, btnSize);
  lv_obj_set_event_cb(btnMinus, event_handlerGoal);
  lv_obj_align(btnMinus, stepGoal, LV_ALIGN_OUT_LEFT_MID, -10, 0);
  lv_obj_set_style_local_bg_color(btnMinus, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::bgAlt);
  lv_obj_t* lblMinus = lv_label_create(btnMinus, nullptr);
  lv_obj_set_style_local_text_font(lblMinus, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_bold_20);
  lv_label_set_text_static(lblMinus, "<");

  btnPlus = lv_btn_create(lv_scr_act(), btnMinus);
  lv_obj_align(btnPlus, stepGoal, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
  lv_obj_t* lblPlus = lv_label_create(btnPlus, lblMinus);
  lv_label_set_text_static(lblPlus, ">");

  title = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(title, "Step length [cm]");
  lv_label_set_align(title, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(title, stepGoal, LV_ALIGN_OUT_BOTTOM_MID, 0, 40);

  stepLengt = lv_label_create(lv_scr_act(), stepGoal);
  lv_label_set_text_fmt(stepLengt, "%lu", System::SystemTask::displayApp->settingsController.GetStepLength());
  lv_obj_align(stepLengt, title, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

  btnM = lv_btn_create(lv_scr_act(), btnMinus);
  lblMinus = lv_label_create(btnM, lblMinus);
  lv_obj_set_event_cb(btnM, event_handlerLength);
  lv_obj_align(btnM, stepLengt, LV_ALIGN_OUT_LEFT_MID, -10, 0);
  btnP = lv_btn_create(lv_scr_act(), btnM);
  lv_obj_align(btnP, stepLengt, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
  lv_label_create(btnP, lblPlus);
}

bool SettingSteps::UnLoad() {
  if (running) {
    running = false;
    lv_obj_clean(lv_scr_act());
  }
  return true;
}

SettingSteps::~SettingSteps() {
  UnLoad();
  System::SystemTask::displayApp->settingsController.SaveSettings();
}

void SettingSteps::updateGoal(lv_obj_t* object, lv_event_t event) {
  int valueChange = 0;
  if (event == LV_EVENT_SHORT_CLICKED) {
    valueChange = 500;
  } else if (event == LV_EVENT_LONG_PRESSED || event == LV_EVENT_LONG_PRESSED_REPEAT) {
    valueChange = 1000;
  } else {
    return;
  }

  uint32_t value = System::SystemTask::displayApp->settingsController.GetStepsGoal();
  if (value < 1000 || value > 500000)
    value = 10000;

  if (object == btnPlus) {
    value += valueChange;
  } else if (object == btnMinus) {
    value -= valueChange;
  }

  System::SystemTask::displayApp->settingsController.SetStepsGoal(value);
  lv_label_set_text_fmt(stepGoal, "%lu", value);
  lv_obj_realign(stepGoal);
}

void SettingSteps::updateLength(lv_obj_t* object, lv_event_t event) {

  int valueChange = 0;
  if (event == LV_EVENT_SHORT_CLICKED) {
    valueChange = 1;
  } else if (event == LV_EVENT_LONG_PRESSED || event == LV_EVENT_LONG_PRESSED_REPEAT) {
    valueChange = 10;
  } else {
    return;
  }

  uint32_t value = System::SystemTask::displayApp->settingsController.GetStepLength();
  if (value > 100 || value < 10)
    value = 40;

  if (object == btnP) {
    value += valueChange;
  } else if (object == btnM) {
    value -= valueChange;
  }

  System::SystemTask::displayApp->settingsController.SetStepLength(value);
  lv_label_set_text_fmt(stepLengt, "%lu", value);
  lv_obj_realign(stepLengt);
}

void SettingSteps::event_handlerGoal(lv_obj_t* obj, lv_event_t event) {
  (static_cast<SettingSteps*>(obj->user_data))->updateGoal(obj, event);
}

void SettingSteps::event_handlerLength(lv_obj_t* obj, lv_event_t event) {
  (static_cast<SettingSteps*>(obj->user_data))->updateLength(obj, event);
}
