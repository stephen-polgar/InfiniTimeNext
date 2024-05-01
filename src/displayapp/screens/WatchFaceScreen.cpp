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

using namespace Pinetime::Applications::Screens;

#ifdef UseFileManager
std::string WatchFaceScreen::bgImage;
lv_obj_t* WatchFaceScreen::image = NULL;
#endif

WatchFaceScreen::WatchFaceScreen() : ScreenRefresh(Apps::Clock) {
}

void WatchFaceScreen::Load() {
  Apps selected = Apps(System::SystemTask::displayApp->settingsController.GetWatchFace());
  if (!current || current->Id != selected) {
    if (current) {
      delete current;
#ifdef UseFileManager
      bgImage.clear();
#endif
    }
    current = System::SystemTask::displayApp->GetSelectedWatchFace();
    if (!current) {
      current = new WatchFaceDigital();
      System::SystemTask::displayApp->settingsController.SetWatchFace(WatchFace(current->Id));
    }
  }
#ifdef UseFileManager
  if (!bgImage.empty()) {
    image = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(image, bgImage.c_str());
    lv_obj_align(image, lv_scr_act(), LV_ALIGN_CENTER, 0, 0);
    lv_obj_move_background(image);
  }
#endif
  current->Load();
  running = current->IsRunning();
  if (running) {
    createRefreshTask(LV_DISP_DEF_REFR_PERIOD, LV_TASK_PRIO_MID);
  } else {
    System::SystemTask::displayApp->settingsController.SetWatchFace(WatchFace::Digital);
  }
}

bool WatchFaceScreen::UnLoad() {
#ifdef UseFileManager
  if (image) {
    lv_obj_del(image);
    image = NULL;
  }
#endif
  if (running) {
    lv_task_del(taskRefresh);
    running = false;
    return current->UnLoad();
  }
  return true;
}

WatchFaceScreen::~WatchFaceScreen() {
#ifdef UseFileManager
  if (image) {
    lv_obj_del(image);
    image = NULL;
  }
#endif
  if (running) {
    lv_task_del(taskRefresh);
  }
  if (current) {
    delete current;
  }
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
