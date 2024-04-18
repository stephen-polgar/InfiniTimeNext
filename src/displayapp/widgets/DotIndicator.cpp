#include "displayapp/widgets/DotIndicator.h"

using namespace Pinetime::Applications::Widgets;

DotIndicator::DotIndicator(uint8_t screens) : PageIndicator(screens) {
}

void DotIndicator::Load(uint8_t currentScreen) {
  lv_obj_t* dotIndicator[screens];
  static constexpr uint8_t dotSize = 12;

  container = lv_cont_create(lv_scr_act(), nullptr);
  lv_cont_set_layout(container, LV_LAYOUT_COLUMN_LEFT);
  lv_cont_set_fit(container, LV_FIT_TIGHT);
  lv_obj_set_style_local_pad_inner(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, dotSize);
  lv_obj_set_style_local_bg_opa(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);

  for (uint8_t i = 0; i < screens; i++) {
    dotIndicator[i] = lv_obj_create(container, nullptr);
    lv_obj_set_size(dotIndicator[i], dotSize, dotSize);
    lv_obj_set_style_local_bg_color(dotIndicator[i], LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
  }

  lv_obj_set_style_local_bg_color(dotIndicator[currentScreen], LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);

  lv_obj_align(container, nullptr, LV_ALIGN_IN_RIGHT_MID, 0, 0);
}

void DotIndicator::UnLoad() {
  lv_obj_del(container);
}