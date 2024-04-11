#include "Tile.h"
#include "systemtask/SystemTask.h"
#include "displayapp/InfiniTimeTheme.h"

using namespace Pinetime::Applications::Screens;

Tile::Tile(uint8_t screenID, Widgets::PageIndicator* pageIndicator) : screenID {screenID}, pageIndicator {pageIndicator} {
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
  for (uint8_t i = 0; i < size; i++) {
    if (i == 3) {
      btnmMap[btIndex++] = "\n";
    }
    btnmMap[btIndex++] = apps[i].icon;
  }
  btnmMap[btIndex] = "";

  lv_obj_t* btnm = lv_btnmatrix_create(lv_scr_act(), nullptr);
  lv_btnmatrix_set_map(btnm, btnmMap);
  lv_obj_set_size(btnm, LV_HOR_RES - 16, size < 4 ? (LV_VER_RES - 60)/2 : (LV_VER_RES - 60));
  lv_obj_align(btnm, nullptr, LV_ALIGN_CENTER, 0, 10);

  lv_obj_set_style_local_radius(btnm, LV_BTNMATRIX_PART_BTN, LV_STATE_DEFAULT, 20);
  lv_obj_set_style_local_bg_opa(btnm, LV_BTNMATRIX_PART_BTN, LV_STATE_DEFAULT, LV_OPA_50);
  lv_obj_set_style_local_bg_color(btnm, LV_BTNMATRIX_PART_BTN, LV_STATE_DEFAULT, LV_COLOR_AQUA);
  lv_obj_set_style_local_bg_opa(btnm, LV_BTNMATRIX_PART_BTN, LV_STATE_DISABLED, LV_OPA_50);
  lv_obj_set_style_local_bg_color(btnm, LV_BTNMATRIX_PART_BTN, LV_STATE_DISABLED, Colors::bgDark);
  lv_obj_set_style_local_pad_all(btnm, LV_BTNMATRIX_PART_BG, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_pad_inner(btnm, LV_BTNMATRIX_PART_BG, LV_STATE_DEFAULT, 10);

  for (uint8_t i = 0; i < size; i++) {
    lv_btnmatrix_set_btn_ctrl(btnm, i, LV_BTNMATRIX_CTRL_CLICK_TRIG);
  }

  btnm->user_data = this;
  lv_obj_set_event_cb(btnm, [](lv_obj_t* obj, lv_event_t event) {
    if (event == LV_EVENT_VALUE_CHANGED) {
      static_cast<Tile*>(obj->user_data)->onValueChangedEvent(*(uint32_t*) lv_event_get_data());
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
  UnLoad();
}

bool Tile::Add(Applications item) {
  apps[size++] = item;
  return size < MaxElements;
}

void Tile::Refresh() {
  lv_label_set_text(label_time, System::SystemTask::displayApp->dateTimeController.FormattedTime().c_str());
  statusIcons.Update();
}

void Tile::onValueChangedEvent(uint8_t index) {
  System::SystemTask::displayApp->StartApp(apps[index].app);
  running = false;
}
