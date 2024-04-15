#include "CheckboxList.h"
#include "Styles.h"

using namespace Pinetime::Applications::Screens;

CheckboxList::CheckboxList(const uint8_t maxSize,
                           const char* optionsTitle,
                           const char* optionsSymbol,
                           std::function<uint8_t()> currentValue,
                           std::function<void(uint8_t)> OnValueChanged,
                           lv_layout_t layout,
                           Widgets::PageIndicator* pageIndicator)
  : ScreenTree(pageIndicator),
    maxSize {maxSize},
    optionsTitle {optionsTitle},
    optionsSymbol {optionsSymbol},
    currentValue {currentValue},
    OnValueChanged {OnValueChanged},
    layout {layout} {
  options.reserve(maxSize);
}

void CheckboxList::Load() {
  running = true;
  ScreenTree::Load();
  // Set the background to Black
  lv_obj_set_style_local_bg_color(lv_scr_act(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_t* container = lv_cont_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_bg_opa(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
  lv_obj_set_style_local_pad_all(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 20);
  lv_obj_set_style_local_pad_top(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, (layout == LV_LAYOUT_COLUMN_LEFT ? 80 : 160)/options.size());
  lv_obj_set_style_local_pad_inner(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 15);
  lv_obj_set_style_local_border_width(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);

  lv_obj_set_pos(container, 0, 40);
  lv_obj_set_width(container, LV_HOR_RES);
  lv_obj_set_height(container, LV_VER_RES - 40);
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
  for (auto& option : options) {
    option.cb = lv_checkbox_create(container, NULL);
    lv_checkbox_set_text(option.cb, option.name.c_str());
    if (!option.enabled) {
      lv_checkbox_set_disabled(option.cb);
    }   
    lv_obj_set_event_cb(option.cb, event_handler);
    SetRadioButtonStyle(option.cb);
    if (currentValue() == option.index) {
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

bool CheckboxList::Add(Item item) {
  options.push_back(item);
  return options.size() < maxSize;
}

void CheckboxList::event_handler(lv_obj_t* obj, lv_event_t event) {
  if (event == LV_EVENT_VALUE_CHANGED)
    static_cast<CheckboxList*>(obj->parent->user_data)->updateSelected(obj);
}

void CheckboxList::updateSelected(lv_obj_t* obj) {
  for (auto& option : options) {
    if (obj == option.cb) {
      lv_checkbox_set_checked(option.cb, true);
      OnValueChanged(option.index);
    } else {
      lv_checkbox_set_checked(option.cb, false);
    }
    if (!option.enabled) {
      lv_checkbox_set_disabled(option.cb);
    }
  }
}
