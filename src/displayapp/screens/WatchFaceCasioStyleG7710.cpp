#include "WatchFaceCasioStyleG7710.h"
#include "systemtask/SystemTask.h"
#include "BleIcon.h"
#include "NotificationIcon.h"
#include "Symbols.h"
#include "components/ble/NotificationManager.h"
#include "components/motion/MotionController.h"
#include "components/battery/BatteryController.h"
#include "components/ble/BleController.h"
#include "components/heartrate/HeartRateController.h"

using namespace Pinetime::Applications::Screens;

WatchFaceCasioStyleG7710::WatchFaceCasioStyleG7710() : batteryIcon(true), Screen(WatchFace::CasioStyleG7710) {
  font_dot40 = lv_font_load("F:/fonts/lv_font_dots_40.bin");
  font_segment40 = lv_font_load("F:/fonts/7segments_40.bin");
  font_segment115 = lv_font_load("F:/fonts/7segments_115.bin");
}

void WatchFaceCasioStyleG7710::Load() {
  if (!font_dot40 || !font_segment40 || !font_segment115) {
      return;
  };
  label_battery_value = lv_label_create(lv_scr_act(), NULL);
  lv_obj_align(label_battery_value, lv_scr_act(), LV_ALIGN_IN_TOP_RIGHT, 0, 0);
  lv_obj_set_style_local_text_color(label_battery_value, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, color_text);
  //lv_label_set_text_static(label_battery_value, "00%");

  batteryIcon.Create(lv_scr_act());
  batteryIcon.SetColor(color_text);
  lv_obj_align(batteryIcon.GetObject(), label_battery_value, LV_ALIGN_OUT_LEFT_MID, -5, 0);

  batteryPlug = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(batteryPlug, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, color_text);
  lv_label_set_text_static(batteryPlug, Symbols::plug);
  lv_obj_align(batteryPlug, batteryIcon.GetObject(), LV_ALIGN_OUT_LEFT_MID, -5, 0);

  bleIcon = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(bleIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, color_text);
  lv_label_set_text_static(bleIcon, Symbols::bluetooth);
  lv_obj_align(bleIcon, batteryPlug, LV_ALIGN_OUT_LEFT_MID, -5, 0);

  notificationIcon = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(notificationIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, color_text);
  lv_label_set_text_static(notificationIcon, NotificationIcon::GetIcon(false));
  lv_obj_align(notificationIcon, bleIcon, LV_ALIGN_OUT_LEFT_MID, -5, 0);

  label_day_of_week = lv_label_create(lv_scr_act(), NULL);
  lv_obj_align(label_day_of_week, lv_scr_act(), LV_ALIGN_IN_TOP_LEFT, 10, 64);
  lv_obj_set_style_local_text_color(label_day_of_week, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, color_text);
  lv_obj_set_style_local_text_font(label_day_of_week, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, font_dot40);
  //lv_label_set_text_static(label_day_of_week, "SUN");

  label_week_number = lv_label_create(lv_scr_act(), NULL);
  lv_obj_align(label_week_number, lv_scr_act(), LV_ALIGN_IN_TOP_LEFT, 5, 22);
  lv_obj_set_style_local_text_color(label_week_number, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, color_text);
  lv_obj_set_style_local_text_font(label_week_number, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, font_dot40);
  //lv_label_set_text_static(label_week_number, "WK26");

  label_day_of_year = lv_label_create(lv_scr_act(), NULL);
  lv_obj_align(label_day_of_year, lv_scr_act(), LV_ALIGN_IN_TOP_LEFT, 100, 30);
  lv_obj_set_style_local_text_color(label_day_of_year, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, color_text);
  lv_obj_set_style_local_text_font(label_day_of_year, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, font_segment40);
  //lv_label_set_text_static(label_day_of_year, "181-184");

  lv_style_init(&style_line);
  lv_style_set_line_width(&style_line, LV_STATE_DEFAULT, 2);
  lv_style_set_line_color(&style_line, LV_STATE_DEFAULT, color_line);
  lv_style_set_line_rounded(&style_line, LV_STATE_DEFAULT, true);

  lv_style_init(&style_border);
  lv_style_set_line_width(&style_border, LV_STATE_DEFAULT, 6);
  lv_style_set_line_color(&style_border, LV_STATE_DEFAULT, color_line);
  lv_style_set_line_rounded(&style_border, LV_STATE_DEFAULT, true);

  lv_obj_t* line_icons = lv_line_create(lv_scr_act(), NULL);
  static const lv_point_t line_icons_points[3] {{0, 5}, {117, 5}, {122, 0}};
  lv_line_set_points(line_icons, line_icons_points, 3);
  lv_obj_add_style(line_icons, LV_LINE_PART_MAIN, &style_line);
  lv_obj_align(line_icons, NULL, LV_ALIGN_IN_TOP_RIGHT, -10, 18);

  line_day_of_week_number = lv_line_create(lv_scr_act(), NULL);
  static const lv_point_t line_day_of_week_number_points[4] {{0, 0}, {100, 0}, {95, 95}, {0, 95}};
  lv_line_set_points(line_day_of_week_number, line_day_of_week_number_points, 4);
  lv_obj_add_style(line_day_of_week_number, LV_LINE_PART_MAIN, &style_border);
  lv_obj_align(line_day_of_week_number, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 8);

  line_day_of_year = lv_line_create(lv_scr_act(), NULL);
  static const lv_point_t line_day_of_year_points[3] {{0, 5}, {130, 5}, {135, 0}};
  lv_line_set_points(line_day_of_year, line_day_of_year_points, 3);
  lv_obj_add_style(line_day_of_year, LV_LINE_PART_MAIN, &style_line);
  lv_obj_align(line_day_of_year, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 60);

  label_date = lv_label_create(lv_scr_act(), NULL);
  lv_obj_align(label_date, lv_scr_act(), LV_ALIGN_IN_TOP_LEFT, 100, 70);
  lv_obj_set_style_local_text_color(label_date, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, color_text);
  lv_obj_set_style_local_text_font(label_date, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, font_segment40);
  //lv_label_set_text_static(label_date, "6-30");

  lv_obj_t* line_date = lv_line_create(lv_scr_act(), NULL);
  static const lv_point_t line_date_points[3] {{0, 5}, {135, 5}, {140, 0}};
  lv_line_set_points(line_date, line_date_points, 3);
  lv_obj_add_style(line_date, LV_LINE_PART_MAIN, &style_line);
  lv_obj_align(line_date, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 100);

  label_time = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(label_time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_obj_set_style_local_text_font(label_time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, font_segment115);
  lv_obj_align(label_time, lv_scr_act(), LV_ALIGN_CENTER, 0, 40);

  lv_obj_t* line_time = lv_line_create(lv_scr_act(), NULL);
  static const lv_point_t line_time_points[3] {{0, 0}, {230, 0}, {235, 5}};
  lv_line_set_points(line_time, line_time_points, 3);
  lv_obj_add_style(line_time, LV_LINE_PART_MAIN, &style_line);
  lv_obj_align(line_time, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, 0, -25);

  label_time_ampm = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(label_time_ampm, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, color_text);
  lv_label_set_text_static(label_time_ampm, "");
  lv_obj_align(label_time_ampm, lv_scr_act(), LV_ALIGN_IN_LEFT_MID, 5, -5);

  lv_obj_t* backgroundLabel = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_click(backgroundLabel, true);
  lv_label_set_long_mode(backgroundLabel, LV_LABEL_LONG_CROP);
  lv_obj_set_size(backgroundLabel, 240, 240);
  lv_obj_set_pos(backgroundLabel, 0, 0);
  lv_label_set_text_static(backgroundLabel, "");

  heartbeatIcon = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text_static(heartbeatIcon, Symbols::heartBeat);
  lv_obj_set_style_local_text_color(heartbeatIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, color_text);
  lv_obj_align(heartbeatIcon, lv_scr_act(), LV_ALIGN_IN_BOTTOM_LEFT, 5, -2);

  heartbeatValue = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(heartbeatValue, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, color_text);
  lv_label_set_text_static(heartbeatValue, "");
  lv_obj_align(heartbeatValue, heartbeatIcon, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

  stepValue = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(stepValue, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, color_text);
  lv_label_set_text_static(stepValue, "0");
  lv_obj_align(stepValue, lv_scr_act(), LV_ALIGN_IN_BOTTOM_RIGHT, -5, -2);

  stepIcon = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(stepIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, color_text);
  lv_label_set_text_static(stepIcon, Symbols::shoe);
  lv_obj_align(stepIcon, stepValue, LV_ALIGN_OUT_LEFT_MID, -5, 0);
  Refresh();
}

bool WatchFaceCasioStyleG7710::UnLoad() {
  if (running) {      
    running = false;
    lv_obj_clean(lv_scr_act());
    lv_style_reset(&style_line);
    lv_style_reset(&style_border);
  }
  return (font_dot40 && font_segment40 && font_segment115);
}

WatchFaceCasioStyleG7710::~WatchFaceCasioStyleG7710() {
  UnLoad();
  if (font_dot40)
    lv_font_free(font_dot40);
  if (font_segment40)
    lv_font_free(font_segment40);
  if (font_segment115)
    lv_font_free(font_segment115);
}

void WatchFaceCasioStyleG7710::Refresh() {
  auto* app = System::SystemTask::displayApp;
  powerPresent = app->batteryController.IsPowerPresent();
  if (!running || powerPresent.IsUpdated()) {
    lv_label_set_text_static(batteryPlug, BatteryIcon::GetPlugIcon(powerPresent.Get()));
  }

  batteryPercentRemaining = app->batteryController.PercentRemaining();
  if (!running || batteryPercentRemaining.IsUpdated()) {
    auto batteryPercent = batteryPercentRemaining.Get();
    batteryIcon.SetBatteryPercentage(batteryPercent);
    lv_label_set_text_fmt(label_battery_value, "%d%%", batteryPercent);
  }

  bleState = app->bleController.IsConnected();
  bleRadioEnabled = app->bleController.IsRadioEnabled();
  if (!running || bleState.IsUpdated() || bleRadioEnabled.IsUpdated()) {
    lv_label_set_text_static(bleIcon, BleIcon::GetIcon(bleState.Get()));
  }
  lv_obj_realign(label_battery_value);
  lv_obj_realign(batteryIcon.GetObject());
  lv_obj_realign(batteryPlug);
  lv_obj_realign(bleIcon);
  lv_obj_realign(notificationIcon);

  notificationState = app->notificationManager.AreNewNotificationsAvailable();
  if (!running || notificationState.IsUpdated()) {
    lv_label_set_text_static(notificationIcon, NotificationIcon::GetIcon(notificationState.Get()));
  }

  currentDateTime = std::chrono::time_point_cast<std::chrono::minutes>(app->dateTimeController.CurrentDateTime());
  if (!running || currentDateTime.IsUpdated()) {
    uint8_t hour = app->dateTimeController.Hours();
    uint8_t minute = app->dateTimeController.Minutes();
    if (app->settingsController.GetClockType() == Controllers::Settings::ClockType::H12) {
      char ampmChar[2] = "A";
      if (hour == 0) {
        hour = 12;
      } else if (hour == 12) {
        ampmChar[0] = 'P';
      } else if (hour > 12) {
        hour = hour - 12;
        ampmChar[0] = 'P';
      }
      lv_label_set_text(label_time_ampm, ampmChar);
      lv_label_set_text_fmt(label_time, "%2d:%02d", hour, minute);
    } else {
      lv_label_set_text_fmt(label_time, "%02d:%02d", hour, minute);
    }
    lv_obj_realign(label_time);

    currentDate = std::chrono::time_point_cast<std::chrono::days>(currentDateTime.Get());
    if (!running || currentDate.IsUpdated()) {
      const char* weekNumberFormat = "%V";

      uint16_t year = app->dateTimeController.Year();
      Controllers::DateTime::Months month = app->dateTimeController.Month();
      uint8_t day = app->dateTimeController.Day();
      int dayOfYear = app->dateTimeController.DayOfYear();
      if (app->settingsController.GetClockType() == Controllers::Settings::ClockType::H24) {
        // 24h mode: ddmmyyyy, first DOW=Monday;
        lv_label_set_text_fmt(label_date, " %02d-%02d", month, day);
        weekNumberFormat = "%V"; // Replaced by the week number of the year (Monday as the first day of the week) as a decimal number
                                 // [01,53]. If the week containing 1 January has four or more days in the new year, then it is considered
                                 // week 1. Otherwise, it is the last week of the previous year, and the next week is week 1. Both January
                                 // 4th and the first Thursday of January are always in week 1. [ tm_year, tm_wday, tm_yday]
      } else {
        // 12h mode: mmddyyyy, first DOW=Sunday;
        lv_label_set_text_fmt(label_date, " %02d-%02d", month, day);
        weekNumberFormat = "%U"; // Replaced by the week number of the year as a decimal number [00,53]. The first Sunday of January is the
                                 // first day of week 1; days in the new year before this are in week 0. [ tm_year, tm_wday, tm_yday]
      }

      time_t ttTime =
        std::chrono::system_clock::to_time_t(std::chrono::time_point_cast<std::chrono::system_clock::duration>(currentDateTime.Get()));
      tm* tmTime = std::localtime(&ttTime);

      // TODO: When we start using C++20, use std::chrono::year::is_leap
      int daysInCurrentYear = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0 ? 366 : 365;
      uint16_t daysTillEndOfYearNumber = daysInCurrentYear - dayOfYear;

      char buffer[8];
      strftime(buffer, 8, weekNumberFormat, tmTime);
      uint8_t weekNumber = atoi(buffer);

      lv_label_set_text_fmt(label_day_of_week, "%s", app->dateTimeController.DayOfWeekShortToString());
      lv_label_set_text_fmt(label_day_of_year, "%3d-%3d", dayOfYear, daysTillEndOfYearNumber);
      lv_label_set_text_fmt(label_week_number, "WK%02d", weekNumber);

      lv_obj_realign(label_day_of_week);
      lv_obj_realign(label_day_of_year);
      lv_obj_realign(label_week_number);
      lv_obj_realign(label_date);
    }
  }

  heartbeat = app->heartRateController.HeartRate();
  heartbeatRunning = app->heartRateController.State() != Controllers::HeartRateController::States::Stopped;
  if (!running || heartbeat.IsUpdated() || heartbeatRunning.IsUpdated()) {
    if (heartbeatRunning.Get()) {
      lv_obj_set_style_local_text_color(heartbeatIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, color_text);
      lv_label_set_text_fmt(heartbeatValue, "%d", heartbeat.Get());
    } else {
      lv_obj_set_style_local_text_color(heartbeatIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x1B1B1B));
      lv_label_set_text_static(heartbeatValue, "");
    }

    lv_obj_realign(heartbeatIcon);
    lv_obj_realign(heartbeatValue);
  }

  stepCount = app->motionController.NbSteps();
  if (!running || stepCount.IsUpdated()) {
    lv_label_set_text_fmt(stepValue, "%lu", stepCount.Get());
    lv_obj_realign(stepValue);
    lv_obj_realign(stepIcon);
  }
  running = true;
}

bool WatchFaceCasioStyleG7710::IsAvailable(Controllers::FS& filesystem) {
  lfs_file file;
  if (filesystem.FileOpen(&file, "/fonts/lv_font_dots_40.bin", LFS_O_RDONLY) < 0) {
    return false;
  }
  filesystem.FileClose(&file);
  if (filesystem.FileOpen(&file, "/fonts/7segments_40.bin", LFS_O_RDONLY) < 0) {
    return false;
  }
  filesystem.FileClose(&file);
  if (filesystem.FileOpen(&file, "/fonts/7segments_115.bin", LFS_O_RDONLY) < 0) {
    return false;
  }
  filesystem.FileClose(&file);
  return true;
}
