#include "WatchFaceTerminal.h"
#ifdef UseWatchFaceTerminal
  #include "systemtask/SystemTask.h"
  #include "components/ble/NotificationManager.h"
  #include "displayapp/screens/Symbols.h"
  #include "components/battery/BatteryController.h"
  #include "components/ble/BleController.h"
  #include "components/heartrate/HeartRateController.h"
  #include "components/motion/MotionController.h"

using namespace Pinetime::Applications::Screens;

WatchFaceTerminal::WatchFaceTerminal() : Screen(WatchFace::Terminal) {
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
  Refresh();
  running = true;
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
      lv_label_set_text_static(connectState, bleState.Get() ? "[STAT]#0082fc Connected#" : "[STAT]#0082fc Disconnected#");
    }
  }

  notificationState = app->notificationManager.AreNewNotificationsAvailable();
  if (!running || notificationState.IsUpdated()) {
    lv_label_set_text_static(notificationIcon, notificationState.Get() ? "You have note." : "");
  }

  currentDateTime = std::chrono::time_point_cast<std::chrono::seconds>(app->dateTimeController.CurrentDateTime());
  if (!running || currentDateTime.IsUpdated()) {
  #ifdef TimeFormat_24H
    lv_label_set_text_fmt(label_time,
                          "[TIME]#11cc55 %02d:%02d:%02d",
                          app->dateTimeController.Hours(),
                          app->dateTimeController.Minutes(),
                          app->dateTimeController.Seconds());
  #else
    uint8_t hour = app->dateTimeController.Hours();
    char ampmChar[3] = "AM";
    if (hour == 0) {
      hour = 12;
    } else if (hour == 12) {
      ampmChar[0] = 'P';
    } else if (hour > 12) {
      hour = hour - 12;
      ampmChar[0] = 'P';
    }
    lv_label_set_text_fmt(label_time,
                          "[TIME]#11cc55 %02d:%02d:%02d %s#",
                          hour,
                          app->dateTimeController.Minutes(),
                          app->dateTimeController.Seconds(),
                          ampmChar);
  #endif
    currentDate = std::chrono::time_point_cast<std::chrono::days>(currentDateTime.Get());
    if (!running || currentDate.IsUpdated()) {
  #ifdef DateFormat_YMD
      lv_label_set_text_fmt(label_date,
                            "[DATE]#007fff %04d-%02d-%02d#",
                            app->dateTimeController.Year(),
                            uint8_t(app->dateTimeController.Month()),
                            app->dateTimeController.Day());
  #elif defined(DateFormat_DMY)
      lv_label_set_text_fmt(label_date,
                            "[DATE]#007fff %02d-%02d-%04d#",
                            app->dateTimeController.Day(),
                            uint8_t(app->dateTimeController.Month()),
                            app->dateTimeController.Year());
  #else // MDY (Month, day, year)  i
      lv_label_set_text_fmt(label_date,
                            "[DATE]#007fff %02d-%02d-%04d#",
                            uint8_t(app->dateTimeController.Month()),
                            app->dateTimeController.Day(),
                            app->dateTimeController.Year());
  #endif
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
}
#endif