#include "FirmwareValidation.h"
#include "Version.h"
#include "displayapp/InfiniTimeTheme.h"

using namespace Pinetime::Applications::Screens;

FirmwareValidation::FirmwareValidation(Controllers::FirmwareValidator& validator)
  : Screen(Apps::FirmwareValidation), validator {validator} {
}

void FirmwareValidation::Load() {
  running = true;
  lv_obj_t* labelVersion = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_fmt(labelVersion,
                        "Version : %lu.%lu.%lu\n"
                        "ShortRef : %s",
                        Version::Major(),
                        Version::Minor(),
                        Version::Patch(),
                        Version::GitCommitHash());
  lv_obj_align(labelVersion, nullptr, LV_ALIGN_IN_TOP_LEFT, 0, 0);

  lv_obj_t* labelIsValidated = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_align(labelIsValidated, labelVersion, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
  lv_label_set_recolor(labelIsValidated, true);
  lv_label_set_long_mode(labelIsValidated, LV_LABEL_LONG_BREAK);
  lv_obj_set_width(labelIsValidated, 240);

  if (validator.IsValidated()) {
    lv_label_set_text_static(labelIsValidated, "You have already\n#00ff00 validated# this firmware#");
  } else {
    lv_label_set_text_static(labelIsValidated,
                             "Please #00ff00 Validate# this version or\n#ff0000 Reset# to rollback to the previous version.");

    buttonValidate = lv_btn_create(lv_scr_act(), nullptr);
    buttonValidate->user_data = this;
    lv_obj_set_size(buttonValidate, 115, 50);
    lv_obj_align(buttonValidate, nullptr, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
    lv_obj_set_event_cb(buttonValidate, buttonEventHandler);
    lv_obj_set_style_local_bg_color(buttonValidate, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::highlight);

    lv_obj_t* labelButtonValidate = lv_label_create(buttonValidate, nullptr);
    lv_label_set_text_static(labelButtonValidate, "Validate");

    buttonReset = lv_btn_create(lv_scr_act(), nullptr);
    buttonReset->user_data = this;
    lv_obj_set_size(buttonReset, 115, 50);
    lv_obj_align(buttonReset, nullptr, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
    lv_obj_set_style_local_bg_color(buttonReset, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_RED);
    lv_obj_set_event_cb(buttonReset, buttonEventHandler);

    lv_obj_t* labelButtonReset = lv_label_create(buttonReset, nullptr);
    lv_label_set_text_static(labelButtonReset, "Reset");
  }
}

bool FirmwareValidation::UnLoad() {
  if (running) {   
    running = false;
    lv_obj_clean(lv_scr_act());
  }
  return true;
}

FirmwareValidation::~FirmwareValidation() {
  UnLoad();
}

void FirmwareValidation::onButtonEvent(lv_obj_t* object) {
  if (object == buttonValidate) {
    validator.Validate();   
  } else if (object == buttonReset) {
    validator.Reset();
  }
  UnLoad();
}

void FirmwareValidation::buttonEventHandler(lv_obj_t* obj, lv_event_t event) {
  if (event == LV_EVENT_CLICKED) static_cast<FirmwareValidation*>(obj->user_data)->onButtonEvent(obj);
}
