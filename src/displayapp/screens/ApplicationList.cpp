#include "ApplicationList.h"
#include "systemtask/SystemTask.h"
#include "displayapp/InfiniTimeTheme.h"

using namespace Pinetime::Applications::Screens;

ApplicationList::ApplicationList(std::array<AppList, UserAppTypes::Count>& apps)
  : Screen(Apps::Launcher),
    apps {apps},
    arrayTouchHandler {maxItems,
                       [this](uint8_t indexBegin, uint8_t indexEnd, Screen::FullRefreshDirections direction) {
                         load(indexBegin, indexEnd, direction);
                       },
                       UserAppTypes::Count},
    pageIndicator {UserAppTypes::Count / maxItems + 1} {
}

void ApplicationList::Load() {
  btnm = lv_btnmatrix_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_radius(btnm, LV_BTNMATRIX_PART_BTN, LV_STATE_DEFAULT, 20);
  lv_obj_set_style_local_bg_opa(btnm, LV_BTNMATRIX_PART_BTN, LV_STATE_DEFAULT, LV_OPA_50);
  lv_obj_set_style_local_bg_color(btnm, LV_BTNMATRIX_PART_BTN, LV_STATE_DEFAULT, LV_COLOR_AQUA);
  lv_obj_set_style_local_bg_opa(btnm, LV_BTNMATRIX_PART_BTN, LV_STATE_DISABLED, LV_OPA_50);
  lv_obj_set_style_local_bg_color(btnm, LV_BTNMATRIX_PART_BTN, LV_STATE_DISABLED, Colors::bgDark);
  lv_obj_set_style_local_pad_all(btnm, LV_BTNMATRIX_PART_BG, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_pad_inner(btnm, LV_BTNMATRIX_PART_BG, LV_STATE_DEFAULT, 10);

  btnm->user_data = this;
  lv_obj_set_event_cb(btnm, [](lv_obj_t* obj, lv_event_t event) {
    if (event == LV_EVENT_VALUE_CHANGED) {
      static_cast<ApplicationList*>(obj->user_data)->onValueChangedEvent(*(uint32_t*) lv_event_get_data());
    }
  });
  arrayTouchHandler.LoadCurrentPos();
  running = true;
}

void ApplicationList::load(uint8_t indexBegin, uint8_t indexEnd, Screen::FullRefreshDirections direction) {
  this->indexBegin = indexBegin;
  if (running) {
    System::SystemTask::displayApp->SetFullRefresh(direction);
  }
  pageIndicator.Load(indexBegin >= maxItems ? indexBegin / maxItems : 0);
  const uint8_t size = indexEnd - indexBegin;
  uint8_t i, btIndex = 0;
  for (i = indexBegin; i < indexEnd; i++) {
    if (i - indexBegin == 3) {
      btnmMap[btIndex++] = "\n";
    }
    btnmMap[btIndex++] = apps[i].icon;
  }
  btnmMap[btIndex] = "";
  lv_btnmatrix_set_map(btnm, btnmMap);
  for (i = 0; i < size; i++) {
    lv_btnmatrix_set_btn_ctrl(btnm, i, LV_BTNMATRIX_CTRL_CLICK_TRIG);
  }
  lv_obj_set_size(btnm, LV_HOR_RES - 16, size < 4 ? (LV_VER_RES - 60) / 2 : (LV_VER_RES - 60));
  lv_obj_align(btnm, NULL, LV_ALIGN_CENTER, 0, 0);
}

bool ApplicationList::UnLoad() {
  if (btnm) {
    btnm = NULL;
    running = false;
    pageIndicator.UnLoad();
    lv_obj_clean(lv_scr_act());
  }
  return true;
}

ApplicationList::~ApplicationList() {
  UnLoad();
}

bool ApplicationList::OnTouchEvent(Applications::TouchEvents event) {
  enableEvent = event == TouchEvents::Tap;
  switch (event) {
    case TouchEvents::SwipeRight:
      System::SystemTask::displayApp->StartApp(Apps::QuickSettings, Screen::FullRefreshDirections::RightAnim);
      break;
    case TouchEvents::SwipeLeft:
      System::SystemTask::displayApp->StartApp(Apps::SettingWatchFace, Screen::FullRefreshDirections::LeftAnim);
      break;
    default:
      return arrayTouchHandler.OnTouchEvent(event);
  }
  return false;
}

void ApplicationList::onValueChangedEvent(uint8_t index) {
  if (enableEvent) {
    System::SystemTask::displayApp->StartApp(apps[indexBegin + index].app);
    running = false;
  }
}
