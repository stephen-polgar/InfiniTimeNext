#include "Settings.h"
#include "systemtask/SystemTask.h"
#include "displayapp/InfiniTimeTheme.h"

using namespace Pinetime::Applications::Screens;

Settings::Settings()
  : Screen(Apps::Settings),
    arrayTouchHandler {apps.size(),
                       maxItems,
                       [this](uint8_t indexBegin, uint8_t indexEnd, Screen::FullRefreshDirections direction) {
                         load(indexBegin, indexEnd, direction);
                       }},
    pageIndicator {apps.size() / maxItems + 1} {
}

void Settings::Load() {
  // Set the background to Black
  lv_obj_set_style_local_bg_color(lv_scr_act(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0, 0, 0));
  container = lv_cont_create(lv_scr_act(), nullptr);
  container->user_data = this;
  lv_obj_set_style_local_bg_opa(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
  lv_obj_set_style_local_pad_inner(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, innerPad);
  lv_obj_set_style_local_border_width(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
  // lv_obj_set_pos(container, 0, 0);
  lv_obj_set_width(container, LV_HOR_RES - 8);
  lv_obj_set_height(container, LV_VER_RES);
  lv_cont_set_layout(container, LV_LAYOUT_COLUMN_LEFT);
  arrayTouchHandler.LoadCurrentPos();
  loaded = running = true;
}

void Settings::load(uint8_t indexBegin, uint8_t indexEnd, Screen::FullRefreshDirections direction) {
  if (running) {
    unload();
    System::SystemTask::displayApp->SetFullRefresh(direction);
  }
  static constexpr uint16_t btnHeight = (LV_HOR_RES_MAX - ((maxItems - 1) * innerPad)) / maxItems;
  pageIndicator.Load(indexBegin >= maxItems ? indexBegin / maxItems : 0);
  for (uint8_t i = indexBegin; i < indexEnd; i++) {
    lv_obj_t* btn = lv_btn_create(container, nullptr);
    lv_obj_set_style_local_radius(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, btnHeight / 3);
    lv_obj_set_style_local_bg_color(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, Colors::bgAlt);
    lv_obj_set_width(btn, LV_HOR_RES - 8);
    lv_obj_set_height(btn, btnHeight);
    lv_btn_set_layout(btn, LV_LAYOUT_OFF);
    btn->user_data = (void*) &apps[i];
    lv_obj_set_event_cb(btn, buttonEventHandler);
    lv_obj_set_style_local_clip_corner(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, true);

    lv_obj_t* icon = lv_label_create(btn, nullptr);
    lv_obj_set_style_local_text_color(icon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_YELLOW);
    lv_label_set_text_static(icon, apps[i].icon);
    lv_label_set_long_mode(icon, LV_LABEL_LONG_CROP);
    lv_label_set_align(icon, LV_LABEL_ALIGN_CENTER);
    lv_obj_set_width(icon, btnHeight);
    lv_obj_align(icon, nullptr, LV_ALIGN_IN_LEFT_MID, 0, 0);

    lv_obj_t* text = lv_label_create(btn, nullptr);
    lv_label_set_text_static(text, apps[i].name);
    lv_obj_align(text, icon, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
  }
}

bool Settings::UnLoad() {
  if (loaded) {
    loaded = running = false;
    lv_obj_clean(lv_scr_act());
  }
  return true;
}

void Settings::unload() {
  pageIndicator.UnLoad();
  lv_obj_t* btn = NULL;
  while ((btn = lv_obj_get_child(container, btn))) {
    lv_obj_del(btn);
  }
}

Settings::~Settings() {
  UnLoad();
}

bool Settings::OnTouchEvent(Applications::TouchEvents event) {
  return arrayTouchHandler.OnTouchEvent(event);
}

void Settings::onButtonEvent(lv_obj_t* obj) {
  System::SystemTask::displayApp->StartApp(static_cast<AppList*>(obj->user_data)->app);
  running = false;
}

void Settings::buttonEventHandler(lv_obj_t* obj, lv_event_t event) {
  if (event == LV_EVENT_CLICKED)
    static_cast<Settings*>(obj->parent->user_data)->onButtonEvent(obj);
}
