#include "CheckboxList.h"
#include "Styles.h"

using namespace Pinetime::Applications::Screens;


CheckboxList::CheckboxList(const uint8_t screenID,
                           const uint8_t numScreens,
                           const char* optionsTitle,
                           const char* optionsSymbol,
                           uint32_t originalValue,
                           std::function<void(uint32_t)> OnValueChanged,
                           std::array<Item, MaxItems> options)
  : screenID {screenID},
    numScreens {numScreens},
    optionsTitle {optionsTitle},
    optionsSymbol {optionsSymbol},
    originalValue {originalValue},
    OnValueChanged {std::move(OnValueChanged)},
    options {options},
    value {originalValue},
    pageIndicator(screenID, numScreens) {
}

void CheckboxList::Load() {
  running = true;
  // Set the background to Black
  lv_obj_set_style_local_bg_color(lv_scr_act(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);

  if (numScreens > 1) {
    pageIndicator.Create();
  }

  lv_obj_t* container1 = lv_cont_create(lv_scr_act(), NULL);

  lv_obj_set_style_local_bg_opa(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
  lv_obj_set_style_local_pad_all(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 10);
  lv_obj_set_style_local_pad_inner(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 5);
  lv_obj_set_style_local_border_width(container1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);

  lv_obj_set_pos(container1, 10, 60);
  lv_obj_set_width(container1, LV_HOR_RES - 20);
  lv_obj_set_height(container1, LV_VER_RES - 50);
  lv_cont_set_layout(container1, LV_LAYOUT_COLUMN_LEFT);

  lv_obj_t* title = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text_static(title, optionsTitle);
  lv_label_set_align(title, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(title, lv_scr_act(), LV_ALIGN_IN_TOP_MID, 10, 15);

  lv_obj_t* icon = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(icon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
  lv_label_set_text_static(icon, optionsSymbol);
  lv_label_set_align(icon, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(icon, title, LV_ALIGN_OUT_LEFT_MID, -10, 0);

  for (unsigned int i = 0; i < options.size(); i++) {
    if (strcmp(options[i].name, "")) {
      cbOption[i] = lv_checkbox_create(container1, NULL);
      lv_checkbox_set_text(cbOption[i], options[i].name);
      if (!options[i].enabled) {
        lv_checkbox_set_disabled(cbOption[i]);
      }
      cbOption[i]->user_data = this;
      lv_obj_set_event_cb(cbOption[i], event_handler);
      SetRadioButtonStyle(cbOption[i]);

      if (static_cast<unsigned int>(originalValue - MaxItems * screenID) == i) {
        lv_checkbox_set_checked(cbOption[i], true);
      }
    }
  }
}

bool CheckboxList::UnLoad() {
  if (running) {
    running = false;
    lv_obj_clean(lv_scr_act());
    OnValueChanged(value);
  }
  return true;
}

CheckboxList::~CheckboxList() {
  UnLoad();
}

 void CheckboxList::event_handler(lv_obj_t* obj, lv_event_t event) {
    (static_cast<CheckboxList*>(obj->user_data))->updateSelected(obj, event);
  }

void CheckboxList::updateSelected(lv_obj_t* object, lv_event_t event) {
  if (event == LV_EVENT_VALUE_CHANGED) {
    for (unsigned int i = 0; i < options.size(); i++) {
      if (strcmp(options[i].name, "")) {
        if (object == cbOption[i]) {
          lv_checkbox_set_checked(cbOption[i], true);
          value = MaxItems * screenID + i;
        } else {
          lv_checkbox_set_checked(cbOption[i], false);
        }
        if (!options[i].enabled) {
          lv_checkbox_set_disabled(cbOption[i]);
        }
      }
    }
  }
}
