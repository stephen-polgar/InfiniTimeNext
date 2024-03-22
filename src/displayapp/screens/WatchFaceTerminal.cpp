#include "WatchFaceTerminal.h"
#include "systemtask/SystemTask.h"
#include "components/ble/NotificationManager.h"
#include "displayapp/screens/Symbols.h"
#include "components/battery/BatteryController.h"
#include "components/ble/BleController.h"
#include "components/heartrate/HeartRateController.h"
#include "components/motion/MotionController.h"

using namespace Pinetime::Applications::Screens;

WatchFaceTerminal::WatchFaceTerminal() : currentDateTime {{}}, Screen(Apps::Clock) {
}

void WatchFaceTerminal::Load() {   
  batteryValue = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_recolor(batteryValue, true);
  lv_obj_align(batteryValue, lv_scr_act(), LV_ALIGN_IN_LEFT_MID, 0, -20);

  connectState = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_recolor(connectState, true);
  lv_obj_align(connectState, lv_scr_act(), LV_ALIGN_IN_LEFT_MID, 0, 40);

  notificationIcon = lv_label_create(lv_scr_act(), NULL);
  lv_obj_align(notificationIcon, NULL, LV_ALIGN_IN_LEFT_MID, 0, -100);

  label_date = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_recolor(label_date, true);
  lv_obj_align(label_date, lv_scr_act(), LV_ALIGN_IN_LEFT_MID, 0, -40);

  label_prompt_1 = lv_label_create(lv_scr_act(), NULL);
  lv_obj_align(label_prompt_1, lv_scr_act(), LV_ALIGN_IN_LEFT_MID, 0, -80);
  lv_label_set_text_static(label_prompt_1, "user@watch:~ $ now");

  label_prompt_2 = lv_label_create(lv_scr_act(), NULL);
  lv_obj_align(label_prompt_2, lv_scr_act(), LV_ALIGN_IN_LEFT_MID, 0, 60);
  lv_label_set_text_static(label_prompt_2, "user@watch:~ $");

  label_time = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_recolor(label_time, true);
  lv_obj_align(label_time, lv_scr_act(), LV_ALIGN_IN_LEFT_MID, 0, -60);

  heartbeatValue = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_recolor(heartbeatValue, true);
  lv_obj_align(heartbeatValue, lv_scr_act(), LV_ALIGN_IN_LEFT_MID, 0, 20);

  stepValue = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_recolor(stepValue, true);
  lv_obj_align(stepValue, lv_scr_act(), LV_ALIGN_IN_LEFT_MID, 0, 0);

  taskRefresh = lv_task_create(RefreshTaskCallback, LV_DISP_DEF_REFR_PERIOD, LV_TASK_PRIO_MID, this);
  Refresh();
}

bool WatchFaceTerminal::UnLoad() {
  if (running) {
    lv_task_del(taskRefresh);
    running = false;
    lv_obj_clean(lv_scr_act());
  }
  return true;
}

WatchFaceTerminal::~WatchFaceTerminal() {
  UnLoad();
}

void WatchFaceTerminal::Refresh() {
  auto* app = System::SystemTask::displayApp;
  
  powerPresent = app->batteryController.IsPowerPresent();
  batteryPercentRemaining = app->batteryController.PercentRemaining();
  if (!running || batteryPercentRemaining.IsUpdated() || powerPresent.IsUpdated()) {
    lv_label_set_text_fmt(batteryValue, "[BATT]#387b54 %d%%", batteryPercentRemaining.Get());
    if (app->batteryController.IsPowerPresent()) {
      lv_label_ins_text(batteryValue, LV_LABEL_POS_LAST, " Charging");
    }
  }

  bleState = app->bleController.IsConnected();
  bleRadioEnabled = app->bleController.IsRadioEnabled();
  if (!running || bleState.IsUpdated() || bleRadioEnabled.IsUpdated()) {
    if (!bleRadioEnabled.Get()) {
      lv_label_set_text_static(connectState, "[STAT]#0082fc Disabled#");
    } else {
      if (bleState.Get()) {
        lv_label_set_text_static(connectState, "[STAT]#0082fc Connected#");
      } else {
        lv_label_set_text_static(connectState, "[STAT]#0082fc Disconnected#");
      }
    }
  }

  notificationState = app->notificationManager.AreNewNotificationsAvailable();
  if (!running || notificationState.IsUpdated()) {
    if (notificationState.Get()) {
      lv_label_set_text_static(notificationIcon, "You have mail.");
    } else {
      lv_label_set_text_static(notificationIcon, "");
    }
  }

  currentDateTime = std::chrono::time_point_cast<std::chrono::seconds>(app->dateTimeController.CurrentDateTime());
  if (!running || currentDateTime.IsUpdated()) {
    uint8_t hour = app->dateTimeController.Hours();
    uint8_t minute = app->dateTimeController.Minutes();
    uint8_t second = app->dateTimeController.Seconds();

    if (app->settingsController.GetClockType() == Controllers::Settings::ClockType::H12) {
      char ampmChar[3] = "AM";
      if (hour == 0) {
        hour = 12;
      } else if (hour == 12) {
        ampmChar[0] = 'P';
      } else if (hour > 12) {
        hour = hour - 12;
        ampmChar[0] = 'P';
      }
      lv_label_set_text_fmt(label_time, "[TIME]#11cc55 %02d:%02d:%02d %s#", hour, minute, second, ampmChar);
    } else {
      lv_label_set_text_fmt(label_time, "[TIME]#11cc55 %02d:%02d:%02d", hour, minute, second);
    }

    currentDate = std::chrono::time_point_cast<std::chrono::days>(currentDateTime.Get());
    if (!running || currentDate.IsUpdated()) {
      uint16_t year = app->dateTimeController.Year();
      Controllers::DateTime::Months month = app->dateTimeController.Month();
      uint8_t day = app->dateTimeController.Day();
      lv_label_set_text_fmt(label_date, "[DATE]#007fff %04d-%02d-%02d#", short(year), char(month), char(day));
    }
  }

  heartbeat = app->heartRateController.HeartRate();
  heartbeatRunning = app->heartRateController.State() != Controllers::HeartRateController::States::Stopped;
  if (!running || heartbeat.IsUpdated() || heartbeatRunning.IsUpdated()) {
    if (heartbeatRunning.Get()) {
      lv_label_set_text_fmt(heartbeatValue, "[L_HR]#ee3311 %d bpm#", heartbeat.Get());
    } else {
      lv_label_set_text_static(heartbeatValue, "[L_HR]#ee3311 ---#");
    }
  }

  stepCount = app->motionController.NbSteps();
  if (!running || stepCount.IsUpdated()) {
    lv_label_set_text_fmt(stepValue, "[STEP]#ee3377 %lu steps#", stepCount.Get());
  }
  running = true;
}
