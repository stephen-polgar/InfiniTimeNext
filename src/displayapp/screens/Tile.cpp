#include "Tile.h"
#include "systemtask/SystemTask.h"
#include "displayapp/InfiniTimeTheme.h"

// #include <nrf_log.h>

using namespace Pinetime::Applications::Screens;

Tile::Tile(uint8_t screenID, std::array<Applications, MaxElements>& applications, Widgets::PageIndicator* pageIndicator)
  : screenID {screenID}, applications {std::move(applications)}, pageIndicator {pageIndicator} { 
#ifdef NRF_LOG_INFO
  NRF_LOG_INFO("Tile new=%d", this);
#endif
}

void Tile::Load() {
  running = true;
  statusIcons.Create();
  lv_obj_align(statusIcons.GetObject(), lv_scr_act(), LV_ALIGN_IN_TOP_RIGHT, -8, 0);

  label_time = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_align(label_time, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(label_time, nullptr, LV_ALIGN_IN_TOP_LEFT, 0, 0);

  pageIndicator->Create(screenID);

  uint8_t btIndex = 0;
  for (uint8_t i = 0; i < MaxElements; i++) {
    if (i == 3) {
      btnmMap[btIndex++] = "\n";
    }
    if (applications[i].application == Apps::None) {
      btnmMap[btIndex] = " ";
    } else {
      btnmMap[btIndex] = applications[i].icon;
    }
    btIndex++;
    apps[i] = applications[i].application;
  }
  btnmMap[btIndex] = "";

  btnm = lv_btnmatrix_create(lv_scr_act(), nullptr);
  lv_btnmatrix_set_map(btnm, btnmMap);
  lv_obj_set_size(btnm, LV_HOR_RES - 16, LV_VER_RES - 60);
  lv_obj_align(btnm, nullptr, LV_ALIGN_CENTER, 0, 10);

  lv_obj_set_style_local_radius(btnm, LV_BTNMATRIX_PART_BTN, LV_STATE_DEFAULT, 20);
  lv_obj_set_style_local_bg_opa(btnm, LV_BTNMATRIX_PART_BTN, LV_STATE_DEFAULT, LV_OPA_50);
  lv_obj_set_style_local_bg_color(btnm, LV_BTNMATRIX_PART_BTN, LV_STATE_DEFAULT, LV_COLOR_AQUA);
  lv_obj_set_style_local_bg_opa(btnm, LV_BTNMATRIX_PART_BTN, LV_STATE_DISABLED, LV_OPA_50);
  lv_obj_set_style_local_bg_color(btnm, LV_BTNMATRIX_PART_BTN, LV_STATE_DISABLED, Colors::bgDark);
  lv_obj_set_style_local_pad_all(btnm, LV_BTNMATRIX_PART_BG, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_pad_inner(btnm, LV_BTNMATRIX_PART_BG, LV_STATE_DEFAULT, 10);

  for (uint8_t i = 0; i < MaxElements; i++) {
    lv_btnmatrix_set_btn_ctrl(btnm, i, LV_BTNMATRIX_CTRL_CLICK_TRIG);
    if (applications[i].application == Apps::None || !applications[i].enabled) {
      lv_btnmatrix_set_btn_ctrl(btnm, i, LV_BTNMATRIX_CTRL_DISABLED);
    }
  }

  btnm->user_data = this;
  lv_obj_set_event_cb(btnm, [](lv_obj_t* obj, lv_event_t event) {
    if (event == LV_EVENT_VALUE_CHANGED) {
     uint32_t eventData = *(uint32_t*) lv_event_get_data();
     static_cast<Tile*>(obj->user_data)->onValueChangedEvent(eventData);
    }
  });

  taskUpdate = lv_task_create(RefreshTaskCallback, 5000, LV_TASK_PRIO_MID, this);

  Refresh();
}

bool Tile::UnLoad() {
  if (taskUpdate) {
    running = false;
    lv_task_del(taskUpdate);
    taskUpdate = NULL;
    lv_obj_clean(lv_scr_act());
  }
  return false;
}

Tile::~Tile() {
#ifdef NRF_LOG_INFO
  NRF_LOG_INFO("Tile del=%d", this);
#endif
  UnLoad();
}

void Tile::Refresh() {
  lv_label_set_text(label_time, System::SystemTask::displayApp->dateTimeController.FormattedTime().c_str());
  statusIcons.Update();
}

void Tile::onValueChangedEvent(uint8_t buttonId) {
   System::SystemTask::displayApp->StartApp(apps[buttonId]);
   running = false;  
}
