#include "displayapp/screens/Error.h"

using namespace Pinetime::Applications::Screens;

Error::Error(System::BootErrors error) : Screen(Apps::Error), error {error} {
}

void Error::Load() {
  running = true;
  lv_obj_t* warningLabel = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(warningLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
  lv_label_set_text_static(warningLabel, "Warning");
  lv_obj_align(warningLabel, nullptr, LV_ALIGN_IN_TOP_MID, 0, 0);

  lv_obj_t* causeLabel = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_long_mode(causeLabel, LV_LABEL_LONG_BREAK);
  lv_obj_set_width(causeLabel, LV_HOR_RES);
  lv_obj_align(causeLabel, warningLabel, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

  if (error == System::BootErrors::TouchController) {
    lv_label_set_text_static(causeLabel, "Touch controller error detected.");
  }

  lv_obj_t* tipLabel = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_long_mode(tipLabel, LV_LABEL_LONG_BREAK);
  lv_obj_set_width(tipLabel, LV_HOR_RES);
  lv_label_set_text_static(tipLabel, "If you encounter problems and your device is under warranty, contact the devices seller.");
  lv_obj_align(tipLabel, causeLabel, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

  lv_obj_t* btnOk = lv_btn_create(lv_scr_act(), nullptr);
  btnOk->user_data = this;
  lv_obj_set_event_cb(btnOk, buttonEventCallback);
  lv_obj_set_size(btnOk, LV_HOR_RES, 50);
  lv_obj_align(btnOk, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, 0);
  lv_obj_t* lblOk = lv_label_create(btnOk, nullptr);
  lv_label_set_text_static(lblOk, "Proceed");
  lv_obj_set_style_local_bg_color(btnOk, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
}

void Error::buttonEventCallback(lv_obj_t* obj, lv_event_t /*event*/) {
  static_cast<Error*>(obj->user_data)->UnLoad();
}
