#include "Tile.h"
#include "systemtask/SystemTask.h"
#include "displayapp/InfiniTimeTheme.h"


using namespace Pinetime::Applications::Screens;

Tile::Tile(uint8_t screenID, uint8_t numScreens, std::array<Applications, 6>& applications, Widgets::PageIndicator& pageIndicator)
  : screenID {screenID}, applications {std::move(applications)},  numScreens {numScreens}, pageIndicator {pageIndicator} {
  taskUpdate = NULL;
}

void Tile::Load() {
  running = true;
  System::SystemTask::displayApp->settingsController.SetAppMenu(screenID);
  statusIcons.Create();
  lv_obj_align(statusIcons.GetObject(), lv_scr_act(), LV_ALIGN_IN_TOP_RIGHT, -8, 0);

  label_time = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_align(label_time, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(label_time, nullptr, LV_ALIGN_IN_TOP_LEFT, 0, 0);

  pageIndicator.Create(screenID, numScreens);

  uint8_t btIndex = 0;
  for (uint8_t i = 0; i < 6; i++) {
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

  btnm1 = lv_btnmatrix_create(lv_scr_act(), nullptr);
  lv_btnmatrix_set_map(btnm1, btnmMap);
  lv_obj_set_size(btnm1, LV_HOR_RES - 16, LV_VER_RES - 60);
  lv_obj_align(btnm1, nullptr, LV_ALIGN_CENTER, 0, 10);

  lv_obj_set_style_local_radius(btnm1, LV_BTNMATRIX_PART_BTN, LV_STATE_DEFAULT, 20);
  lv_obj_set_style_local_bg_opa(btnm1, LV_BTNMATRIX_PART_BTN, LV_STATE_DEFAULT, LV_OPA_50);
  lv_obj_set_style_local_bg_color(btnm1, LV_BTNMATRIX_PART_BTN, LV_STATE_DEFAULT, LV_COLOR_AQUA);
  lv_obj_set_style_local_bg_opa(btnm1, LV_BTNMATRIX_PART_BTN, LV_STATE_DISABLED, LV_OPA_50);
  lv_obj_set_style_local_bg_color(btnm1, LV_BTNMATRIX_PART_BTN, LV_STATE_DISABLED, Colors::bgDark);
  lv_obj_set_style_local_pad_all(btnm1, LV_BTNMATRIX_PART_BG, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_pad_inner(btnm1, LV_BTNMATRIX_PART_BG, LV_STATE_DEFAULT, 10);

  for (uint8_t i = 0; i < 6; i++) {
    lv_btnmatrix_set_btn_ctrl(btnm1, i, LV_BTNMATRIX_CTRL_CLICK_TRIG);
    if (applications[i].application == Apps::None || !applications[i].enabled) {
      lv_btnmatrix_set_btn_ctrl(btnm1, i, LV_BTNMATRIX_CTRL_DISABLED);
    }
  }

  btnm1->user_data = this;
  lv_obj_set_event_cb(btnm1, [](lv_obj_t* obj, lv_event_t event) {
    if (event == LV_EVENT_VALUE_CHANGED) {
      Tile* screen = static_cast<Tile*>(obj->user_data);
      auto* eventDataPtr = (uint32_t*) lv_event_get_data();
      uint32_t eventData = *eventDataPtr;
      screen->onValueChangedEvent(obj, eventData);
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
  return true;
}

Tile::~Tile() {
  UnLoad();
}

void Tile::Refresh() {
  lv_label_set_text(label_time, System::SystemTask::displayApp->dateTimeController.FormattedTime().c_str());
  statusIcons.Update();
}

void Tile::onValueChangedEvent(lv_obj_t* obj, uint8_t buttonId) {
  if (obj == btnm1) {
    System::SystemTask::displayApp->StartApp(apps[buttonId]);
    running = false;
  }
}
