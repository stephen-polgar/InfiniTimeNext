/*
 * This file is part of the InfinitimeNext distribution (https://github.com/stephen-polgar/InfiniTimeNext).
 * Copyright (c) 2024 Istvan Polgar.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "WatchFaceScreen.h"
#include "WatchFaceDigital.h" //  default if not found or not working
#include "systemtask/SystemTask.h"
#include "components/settings/Settings.h"

// #define Log

#ifdef Log
  #include <nrf_log.h>
#endif

using namespace Pinetime::Applications::Screens;

WatchFaceScreen::WatchFaceScreen() : Screen(Apps::Clock) {
#ifdef Log
  NRF_LOG_INFO("WatchFaceScreen new=%d, %d", this, uint8_t(Id));
#endif
}

void WatchFaceScreen::Load() {
  Apps selected = Apps(System::SystemTask::displayApp->settingsController.GetWatchFace());
  if (!current || current->Id != selected) {
    if (current)
      delete current;
    current = System::SystemTask::displayApp->GetSelectedWatchFace();
    if (!current) {
      current = new WatchFaceDigital();
      System::SystemTask::displayApp->settingsController.SetWatchFace(WatchFace(current->Id));
    }
  }
  current->Load();
  running = current->IsRunning();
  if (running) {
    taskRefresh = lv_task_create(RefreshTaskCallback, LV_DISP_DEF_REFR_PERIOD, LV_TASK_PRIO_MID, this);
  } else {
    System::SystemTask::displayApp->settingsController.SetWatchFace(WatchFace::Digital);
  }
}

bool WatchFaceScreen::UnLoad() {
  if (running) {
    lv_task_del(taskRefresh);
    running = false;
    return current->UnLoad();
  }
  return true;
}

WatchFaceScreen::~WatchFaceScreen() {
  if (running) {
    lv_task_del(taskRefresh);
  }
  if (current) {
    delete current;
  }
#ifdef Log
  NRF_LOG_INFO("WatchFaceScreen delete=%d %d", this, uint8_t(Id));
#endif
}

void WatchFaceScreen::Refresh() {
  current->Refresh();
}

bool WatchFaceScreen::OnTouchEvent(uint16_t x, uint16_t y) {
  return running ? current->OnTouchEvent(x, y) : false;
}

bool WatchFaceScreen::OnButtonPushed() {
  System::SystemTask::displayApp->systemTask->PushMessage(System::Messages::GoToSleep);
  return true;
}

bool WatchFaceScreen::OnTouchEvent(TouchEvents event) {
  if (running && !current->OnTouchEvent(event)) {
    switch (event) {
      /*
      case TouchEvents::LongTap:
        System::SystemTask::displayApp->StartApp(Apps::SettingWatchFace);
        break;
        */
      case TouchEvents::SwipeUp:
        System::SystemTask::displayApp->StartApp(Apps::Launcher, Screen::FullRefreshDirections::Up);
        break;
      case TouchEvents::SwipeDown:
        if (System::SystemTask::displayApp->notificationManager.IsEmpty() &&
            System::SystemTask::displayApp->systemTask->nimbleController.weatherService.Current().has_value()) {
          System::SystemTask::displayApp->StartApp(Apps::Weather, Screen::FullRefreshDirections::Down);
        } else {
          System::SystemTask::displayApp->StartApp(Apps::Notifications, Screen::FullRefreshDirections::Down);
        }
        break;
      case TouchEvents::SwipeRight:
        System::SystemTask::displayApp->StartApp(Apps::QuickSettings, Screen::FullRefreshDirections::RightAnim);
        break;
      case TouchEvents::SwipeLeft:
        System::SystemTask::displayApp->StartApp(Apps::SettingWatchFace, Screen::FullRefreshDirections::LeftAnim);
        break;
      case TouchEvents::DoubleTap:
        System::SystemTask::displayApp->systemTask->PushMessage(System::Messages::GoToSleep);
        break;
      default:
        return false;
    }
    return true;
  }
  return false;
}
