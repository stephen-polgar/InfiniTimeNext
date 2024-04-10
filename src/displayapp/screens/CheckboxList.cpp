#include "CheckboxList.h"
#include "Styles.h"

using namespace Pinetime::Applications::Screens;

CheckboxList::CheckboxList(const uint8_t screenID,
                           Widgets::PageIndicator* pageIndicator,
                           const char* optionsTitle,
                           const char* optionsSymbol,
                           std::function<uint8_t(uint8_t screenId)> currentValue,
                           std::function<void(uint8_t screenId, uint8_t index)> OnValueChanged,
                           lv_layout_t layout)
  : screenID {screenID},
    pageIndicator {pageIndicator},
    optionsTitle {optionsTitle},
    optionsSymbol {optionsSymbol},
    currentValue {currentValue},
    OnValueChanged {OnValueChanged},
    layout {layout} {
}

CheckboxList::CheckboxList(const char* optionsTitle,
                           const char* optionsSymbol,
                           std::function<uint8_t(uint8_t screenId)> currentValue,
                           std::function<void(uint8_t screenId, uint8_t index)> OnValueChanged,
                           lv_layout_t layout)
  : optionsTitle {optionsTitle},
    optionsSymbol {optionsSymbol},
    currentValue {currentValue},
    OnValueChanged {OnValueChanged},
    pageIndicator {NULL},
    screenID {0},
    layout {layout} {
}

void CheckboxList::Load() {
  running = true;
  // Set the background to Black
  lv_obj_set_style_local_bg_color(lv_scr_act(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);

  if (pageIndicator) {
    pageIndicator->Create(screenID);
  }

  lv_obj_t* container = lv_cont_create(lv_scr_act(), NULL);

  lv_obj_set_style_local_bg_opa(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
  lv_obj_set_style_local_pad_all(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 10);
  lv_obj_set_style_local_pad_inner(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 5);
  lv_obj_set_style_local_border_width(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);

  lv_obj_set_pos(container, 10, 60);
  lv_obj_set_width(container, LV_HOR_RES - 20);
  lv_obj_set_height(container, LV_VER_RES - 50);
  lv_cont_set_layout(container, layout);

  lv_obj_t* title = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text_static(title, optionsTitle);
  lv_label_set_align(title, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(title, lv_scr_act(), LV_ALIGN_IN_TOP_MID, 10, 15);

  lv_obj_t* icon = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(icon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
  lv_label_set_text_static(icon, optionsSymbol);
  lv_label_set_align(icon, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(icon, title, LV_ALIGN_OUT_LEFT_MID, -10, 0);
  container->user_data = this;
  uint8_t i = 0;
  for (auto& option : options) {
    option.cb = lv_checkbox_create(container, NULL);
    lv_checkbox_set_text(option.cb, option.name.c_str());
    if (!option.enabled) {
      lv_checkbox_set_disabled(option.cb);
    }
    lv_obj_set_event_cb(option.cb, event_handler);
    SetRadioButtonStyle(option.cb);
    if (currentValue(screenID) == i++) {
      lv_checkbox_set_checked(option.cb, true);
    }
  }
}

bool CheckboxList::UnLoad() {
  if (running) {
    running = false;
    lv_obj_clean(lv_scr_act());
  }
  return true;
}

CheckboxList::~CheckboxList() {
  UnLoad();
}

void CheckboxList::Reserve(uint8_t size) {
  options.reserve(size);
}

void CheckboxList::Add(Item item) {
  options.push_back(item);
}

void CheckboxList::event_handler(lv_obj_t* obj, lv_event_t event) {
  if (event == LV_EVENT_VALUE_CHANGED)
    static_cast<CheckboxList*>(obj->parent->user_data)->updateSelected(obj);
}

void CheckboxList::updateSelected(lv_obj_t* object) {
  uint8_t i = 0;
  for (auto& option : options) {
    if (object == option.cb) {
      lv_checkbox_set_checked(option.cb, true);
      OnValueChanged(screenID, i);
    } else {
      lv_checkbox_set_checked(option.cb, false);
    }
    if (!options[i].enabled) {
      lv_checkbox_set_disabled(option.cb);
    }
    i++;
  }
}
