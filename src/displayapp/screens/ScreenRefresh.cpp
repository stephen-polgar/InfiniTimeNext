#include "displayapp/screens/ScreenRefresh.h"

using namespace Pinetime::Applications::Screens;

ScreenRefresh::ScreenRefresh(Apps Id) : Screen(Id) {
}


void ScreenRefresh::refreshTaskCallback(lv_task_t* task) {
  static_cast<Screen*>(task->user_data)->Refresh();
}

void ScreenRefresh::createRefreshTask(uint32_t period, lv_task_prio_t prio) {
  taskRefresh = lv_task_create(refreshTaskCallback, period, prio, this);
}

bool ScreenRefresh::UnLoad() {
  if (taskRefresh) {
    lv_task_del(taskRefresh);
    taskRefresh = NULL;
  }
  return Screen::UnLoad();
}

ScreenRefresh::~ScreenRefresh() {
  UnLoad();
}