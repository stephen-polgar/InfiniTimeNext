
#include "WatchFaceScreen.h"
#include "WatchFaceDigital.h"  //  default if not found
#include "systemtask/SystemTask.h"
#include "components/settings/Settings.h"

//#define Log

#ifdef Log
  #include <nrf_log.h>
#endif

using namespace Pinetime::Applications::Screens;

WatchFaceScreen::WatchFaceScreen() : Screen(Apps::Clock) {
}

void WatchFaceScreen::Load() {
  running = true;
  Apps selected = Apps(System::SystemTask::displayApp->settingsController.GetWatchFace());
  if (!current || current->Id != selected) {
#ifdef Log
    NRF_LOG_INFO("WatchFaceScreen::Load current=%d sel=%d", current, selected);
#endif
    if (current)
      delete current;
    current = System::SystemTask::displayApp->GetSelectedWatchFace();
    if (!current) {
      current = new WatchFaceDigital();
      System::SystemTask::displayApp->settingsController.SetWatchFace(WatchFace(current->Id));
    }
  }
  current->Load();
  taskRefresh = lv_task_create(RefreshTaskCallback, LV_DISP_DEF_REFR_PERIOD, LV_TASK_PRIO_MID, this);
  Refresh();
}

bool WatchFaceScreen::UnLoad() {
  if (running) {
    lv_task_del(taskRefresh);
    running = false;
    current->UnLoad();
  }
  return true;
}

WatchFaceScreen::~WatchFaceScreen() {
  UnLoad();
#ifdef Log
  NRF_LOG_INFO("WatchFaceScreen delete=%d", this);
#endif
}

void WatchFaceScreen::Refresh() {
  current->Refresh();
}

bool WatchFaceScreen::OnTouchEvent(uint16_t x, uint16_t y) {
  return running ? current->OnTouchEvent(x, y) : false;
}

bool WatchFaceScreen::OnTouchEvent(TouchEvents event) {
  if (running) {
    if (current->OnTouchEvent(event))
      return true;
    if (event == TouchEvents::LongTap) {
      System::SystemTask::displayApp->StartApp(Apps::SettingWatchFace);
      return true;
    }
  }
  return false;
}
