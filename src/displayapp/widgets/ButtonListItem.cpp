#include "displayapp/widgets/ButtonListItem.h"
#include "displayapp/InfiniTimeTheme.h"


using namespace Pinetime::Applications::Widgets;

ButtonListItem::ButtonListItem(std::function<void(bool, void*)> on_checked,
                               std::function<void(void*)> on_open,
                               std::function<void(void*)> on_removed,
                               std::function<void()> on_addNew)
  : on_checked {on_checked}, on_open {on_open}, on_removed {on_removed}, on_addNew {on_addNew} {
}

void ButtonListItem::CreateNewItem(lv_obj_t* label, bool checked, void* data) {
  lv_obj_t* cont = lv_cont_create(lv_scr_act(), NULL);
  cont->user_data = data;

  lv_obj_set_style_local_bg_opa(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
  lv_obj_set_style_local_pad_all(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 3);
  lv_obj_set_style_local_pad_inner(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 8);
  lv_obj_set_style_local_border_width(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 1);

  lv_cont_set_fit(cont, LV_FIT_TIGHT);
  lv_cont_set_layout(cont, LV_LAYOUT_ROW_MID);
  const lv_coord_t size = 40;

  lv_obj_t* cb = lv_checkbox_create(cont, NULL);  
  lv_checkbox_set_text_static(cb, "");
  lv_obj_set_size(cb, size, size);
  lv_checkbox_set_checked(cb, checked);
  cb->user_data = this;
  lv_obj_set_event_cb(cb, [](lv_obj_t* obj, lv_event_t event) {
    if (event == LV_EVENT_VALUE_CHANGED) {
      static_cast<ButtonListItem*>(obj->user_data)->on_checked(lv_checkbox_is_checked(obj), obj->parent->user_data);
    }
  });

  lv_obj_t* btn = lv_btn_create(cont, NULL);
  // lv_obj_set_size(btn, 100, size);
  lv_obj_set_parent(label, btn);
  lv_obj_set_style_local_bg_color(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, Colors::bgAlt);

  btn->user_data = this;
  lv_obj_set_event_cb(btn, [](lv_obj_t* obj, lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
      static_cast<ButtonListItem*>(obj->user_data)->on_open(obj->parent->user_data);
    }
  });

  btn = lv_btn_create(cont, NULL);
  lv_obj_set_size(btn, size, size);
  label = lv_label_create(btn, NULL);
  lv_label_set_text_static(label, "X");
  // lv_obj_set_style_local_bg_color(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, Colors::bgAlt);
  lv_obj_set_style_local_border_width(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 4);
  lv_obj_set_style_local_border_color(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_set_style_local_pad_left(btn, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 8);
  btn->user_data = this;
  lv_obj_set_event_cb(btn, [](lv_obj_t* obj, lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
      static_cast<ButtonListItem*>(obj->user_data)->on_removed(obj->parent->user_data);
      lv_obj_del(obj->parent);
    }
  });

  // lv_obj_set_style_local_bg_color(label, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_NAVY);
  if (last)
    lv_obj_align(cont, last, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
  else
    lv_obj_align(cont, lv_scr_act(), LV_ALIGN_IN_TOP_MID, 0, 0);
  last = cont;
}

void ButtonListItem::CreateButtonNew() {
  btnNew = lv_btn_create(lv_scr_act(), NULL);
  lv_obj_align(btnNew, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, 0);
  lv_obj_t* label = lv_label_create(btnNew, NULL);
  btnNew->user_data = this;
  lv_obj_set_event_cb(btnNew, [](lv_obj_t* obj, lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
      static_cast<ButtonListItem*>(obj->user_data)->on_addNew();      
    }
  });
  lv_label_set_text_static(label, "+");
  // lv_obj_set_style_local_bg_color(btnMessage, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_NAVY);
}

void ButtonListItem::EnableAddButton(bool enable) {
  lv_btn_set_state(btnNew, enable ? LV_BTN_STATE_RELEASED : LV_BTN_STATE_DISABLED);
}

void ButtonListItem::Unload() {
  last = NULL;
}