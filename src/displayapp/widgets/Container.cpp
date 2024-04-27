#include "displayapp/widgets/Container.h"


using namespace Pinetime::Applications::Widgets;

void Container::Load(const char* title, const char* symbol, lv_layout_t layout) {
  lv_obj_set_style_local_bg_color(lv_scr_act(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_t* label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text_static(label, title);
  lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(label, lv_scr_act(), LV_ALIGN_IN_TOP_MID, 10, 15);
  lv_obj_t* icon = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(icon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
  lv_label_set_text_static(icon, symbol);
  lv_label_set_align(icon, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(icon, label, LV_ALIGN_OUT_LEFT_MID, -10, 0);

  Container = lv_cont_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_bg_opa(Container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
  lv_obj_set_style_local_pad_all(Container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 20);
  lv_obj_set_style_local_pad_inner(Container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 15);
  lv_obj_set_style_local_border_width(Container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_pos(Container, 0, 40);
  lv_obj_set_size(Container, LV_HOR_RES, LV_VER_RES - 40);
  lv_cont_set_layout(Container, layout);  
}

void Container::UpdateHeight(uint8_t pad_top) {
  lv_obj_set_style_local_pad_top(Container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, pad_top);
}