/*
 * This file is part of the Infinitime distribution (https://github.com/InfiniTimeOrg/Infinitime).
 * Copyright (c) 2021 Kieran Cawthray.
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
 *
 * PineTimeStyle watch face for Infinitime created by Kieran Cawthray
 * Based on WatchFaceDigital
 * Style/layout copied from TimeStyle for Pebble by Dan Tilden (github.com/tilden)
 */

#include "displayapp/screens/WatchFacePineTimeStyle.h"
#ifdef UseWatchFacePineTimeStyle
  #include "systemtask/SystemTask.h"
  #include "displayapp/screens/BleIcon.h"
  #include "displayapp/screens/NotificationIcon.h"
  #include "displayapp/screens/Symbols.h"
  #include "displayapp/screens/WeatherSymbols.h"
  #include "components/battery/BatteryController.h"
  #include "components/ble/BleController.h"
  #include "components/ble/NotificationManager.h"
  #include "components/motion/MotionController.h"

// #include <nrf_log.h>

using namespace Pinetime::Applications::Screens;

WatchFacePineTimeStyle::WatchFacePineTimeStyle() : Screen(WatchFace::PineTimeStyle) {
  settingsController = &System::SystemTask::displayApp->settingsController;
}

void WatchFacePineTimeStyle::Load() {
  displayedHour = displayedMinute = displayedSecond = -1;
  currentDay = 0;
  currentMonth = Controllers::DateTime::Months::Unknown;
  currentDayOfWeek = Controllers::DateTime::Days::Unknown;

  // Create a 200px wide background rectangle
  timebar = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_bg_color(timebar, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, Convert(settingsController->GetPTSColorBG()));
  lv_obj_set_style_local_radius(timebar, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_size(timebar, 200, 240);
  lv_obj_align(timebar, lv_scr_act(), LV_ALIGN_IN_TOP_LEFT, 0, 0);

  // Display the time
  timeDD1 = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(timeDD1, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &open_sans_light);
  lv_obj_set_style_local_text_color(timeDD1, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Convert(settingsController->GetPTSColorTime()));

  timeDD2 = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(timeDD2, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &open_sans_light);
  lv_obj_set_style_local_text_color(timeDD2, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Convert(settingsController->GetPTSColorTime()));

  timeAMPM = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(timeAMPM, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Convert(settingsController->GetPTSColorTime()));
  lv_obj_set_style_local_text_line_space(timeAMPM, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, -3);
  lv_label_set_text_static(timeAMPM, "");

  // Create a 40px wide bar down the right side of the screen
  sidebar = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_bg_color(sidebar, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, Convert(settingsController->GetPTSColorBar()));
  lv_obj_set_style_local_radius(sidebar, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_size(sidebar, 40, 240);
  lv_obj_align(sidebar, lv_scr_act(), LV_ALIGN_IN_TOP_RIGHT, 0, 0);

  // Display icons
  batteryIcon.Create(sidebar);
  batteryIcon.SetColor(LV_COLOR_BLACK);
  lv_obj_align(batteryIcon.GetObject(), nullptr, LV_ALIGN_IN_TOP_MID, 10, 2);

  plugIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(plugIcon, Symbols::plug);
  lv_obj_set_style_local_text_color(plugIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_align(plugIcon, sidebar, LV_ALIGN_IN_TOP_MID, 10, 2);

  bleIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(bleIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_align(bleIcon, sidebar, LV_ALIGN_IN_TOP_MID, -10, 2);

  notificationIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(notificationIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Convert(settingsController->GetPTSColorTime()));

  // Calendar icon
  calendarOuter = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_bg_color(calendarOuter, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_set_style_local_radius(calendarOuter, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_size(calendarOuter, 34, 34);
  lv_obj_align(calendarOuter,
               sidebar,
               LV_ALIGN_CENTER,
               0,
               settingsController->GetPTSWeather() == Controllers::Settings::PTSWeather::On ? 20 : 0);
  calendarInner = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_bg_color(calendarInner, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_obj_set_style_local_radius(calendarInner, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_size(calendarInner, 27, 27);
  lv_obj_align(calendarInner, calendarOuter, LV_ALIGN_CENTER, 0, 0);

  calendarBar1 = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_bg_color(calendarBar1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_set_style_local_radius(calendarBar1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_size(calendarBar1, 3, 12);
  lv_obj_align(calendarBar1, calendarOuter, LV_ALIGN_IN_TOP_MID, -6, -3);

  calendarBar2 = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_bg_color(calendarBar2, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_set_style_local_radius(calendarBar2, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_size(calendarBar2, 3, 12);
  lv_obj_align(calendarBar2, calendarOuter, LV_ALIGN_IN_TOP_MID, 6, -3);

  calendarCrossBar1 = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_bg_color(calendarCrossBar1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_set_style_local_radius(calendarCrossBar1, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_size(calendarCrossBar1, 8, 3);
  lv_obj_align(calendarCrossBar1, calendarBar1, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

  calendarCrossBar2 = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_bg_color(calendarCrossBar2, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_set_style_local_radius(calendarCrossBar2, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_size(calendarCrossBar2, 8, 3);
  lv_obj_align(calendarCrossBar2, calendarBar2, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

  // Display date
  dateDayOfWeek = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(dateDayOfWeek, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);

  dateDay = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(dateDay, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);

  dateMonth = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(dateMonth, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);

  // Step count gauge
  needle_colors[0] = settingsController->GetPTSColorBar() == Controllers::Settings::Colors::White ? LV_COLOR_BLACK : LV_COLOR_WHITE;
  stepGauge = lv_gauge_create(lv_scr_act(), nullptr);
  lv_gauge_set_needle_count(stepGauge, 1, needle_colors);
  lv_gauge_set_range(stepGauge, 0, 100);
  lv_gauge_set_value(stepGauge, 0, 0);
  if (settingsController->GetPTSGaugeStyle() == Controllers::Settings::PTSGaugeStyle::Full) {
    lv_obj_set_size(stepGauge, 40, 40);
    lv_obj_align(stepGauge, sidebar, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    lv_gauge_set_scale(stepGauge, 360, 11, 0);
    lv_gauge_set_angle_offset(stepGauge, 180);
    lv_gauge_set_critical_value(stepGauge, 100);
  } else if (settingsController->GetPTSGaugeStyle() == Controllers::Settings::PTSGaugeStyle::Half) {
    lv_obj_set_size(stepGauge, 37, 37);
    lv_obj_align(stepGauge, sidebar, LV_ALIGN_IN_BOTTOM_MID, 0, -10);
    lv_gauge_set_scale(stepGauge, 180, 5, 0);
    lv_gauge_set_angle_offset(stepGauge, 0);
    lv_gauge_set_critical_value(stepGauge, 120);
  } else if (settingsController->GetPTSGaugeStyle() == Controllers::Settings::PTSGaugeStyle::Numeric) {
    lv_obj_set_hidden(stepGauge, true);
  }

  lv_obj_set_style_local_pad_right(stepGauge, LV_GAUGE_PART_MAIN, LV_STATE_DEFAULT, 3);
  lv_obj_set_style_local_pad_left(stepGauge, LV_GAUGE_PART_MAIN, LV_STATE_DEFAULT, 3);
  lv_obj_set_style_local_pad_bottom(stepGauge, LV_GAUGE_PART_MAIN, LV_STATE_DEFAULT, 3);
  lv_obj_set_style_local_line_opa(stepGauge, LV_GAUGE_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_COVER);
  lv_obj_set_style_local_scale_width(stepGauge, LV_GAUGE_PART_MAIN, LV_STATE_DEFAULT, 4);
  lv_obj_set_style_local_line_width(stepGauge, LV_GAUGE_PART_MAIN, LV_STATE_DEFAULT, 4);
  lv_obj_set_style_local_line_color(stepGauge, LV_GAUGE_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_set_style_local_line_opa(stepGauge, LV_GAUGE_PART_NEEDLE, LV_STATE_DEFAULT, LV_OPA_COVER);
  lv_obj_set_style_local_line_width(stepGauge, LV_GAUGE_PART_NEEDLE, LV_STATE_DEFAULT, 3);
  lv_obj_set_style_local_pad_inner(stepGauge, LV_GAUGE_PART_NEEDLE, LV_STATE_DEFAULT, 4);

  stepValue = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(stepValue, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_set_hidden(stepValue, settingsController->GetPTSGaugeStyle() != Controllers::Settings::PTSGaugeStyle::Numeric);

  stepIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(stepIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_label_set_text_static(stepIcon, Symbols::shoe);
  lv_obj_align(stepIcon, stepValue, LV_ALIGN_OUT_TOP_MID, 0, 0);
  lv_obj_set_hidden(stepIcon, settingsController->GetPTSGaugeStyle() != Controllers::Settings::PTSGaugeStyle::Numeric);

  // Display seconds
  timeDD3 = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(timeDD3, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_set_hidden(timeDD3, settingsController->GetPTSGaugeStyle() != Controllers::Settings::PTSGaugeStyle::Half);

  if (settingsController->GetPTSWeather() == Controllers::Settings::PTSWeather::On)
    showWeather();
  else
    temperature = NULL;

  Refresh();

  lv_obj_align(timeDD1, timebar, LV_ALIGN_IN_TOP_MID, 5, 5);
  lv_obj_align(timeDD2, timebar, LV_ALIGN_IN_BOTTOM_MID, 5, -5);
  lv_obj_align(timeAMPM, timebar, LV_ALIGN_IN_BOTTOM_LEFT, 2, -20);
  lv_obj_align(notificationIcon, timebar, LV_ALIGN_IN_TOP_LEFT, 5, 5);
  lv_obj_align(dateDayOfWeek, calendarOuter, LV_ALIGN_CENTER, 0, -32);
  lv_obj_align(dateDay, calendarOuter, LV_ALIGN_CENTER, 0, 3);
  lv_obj_align(dateMonth, calendarOuter, LV_ALIGN_CENTER, 0, 32);
  lv_obj_align(stepValue, sidebar, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
  lv_obj_align(timeDD3, sidebar, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
}

void WatchFacePineTimeStyle::showMenu() {
  btnClose = lv_btn_create(lv_scr_act(), nullptr);
  btnClose->user_data = this;
  lv_obj_set_size(btnClose, 60, 60);
  lv_obj_align(btnClose, lv_scr_act(), LV_ALIGN_CENTER, 0, -80);
  lv_obj_set_style_local_bg_opa(btnClose, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_50);
  lv_obj_t* lblClose = lv_label_create(btnClose, nullptr);
  lv_label_set_text_static(lblClose, "X");
  lv_obj_set_event_cb(btnClose, event_handler);
  lv_obj_set_hidden(btnClose, true);
  //
  btnSetColor = lv_btn_create(lv_scr_act(), btnClose);
  lv_obj_set_size(btnSetColor, 150, 60);
  lv_obj_align(btnSetColor, lv_scr_act(), LV_ALIGN_CENTER, 0, -40);
  lv_obj_set_style_local_radius(btnSetColor, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 20);
  lv_obj_t* lblSetColor = lv_label_create(btnSetColor, nullptr);
  lv_obj_set_style_local_text_font(lblSetColor, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_sys_48);
  lv_label_set_text_static(lblSetColor, Symbols::paintbrushLg);

  btnSetOpts = lv_btn_create(lv_scr_act(), btnSetColor);
  lv_obj_align(btnSetOpts, lv_scr_act(), LV_ALIGN_CENTER, 0, 40);
  lv_obj_set_event_cb(btnSetOpts, event_handler);
  lv_obj_t* lblSetOpts = lv_label_create(btnSetOpts, lblSetColor);
  lv_label_set_text_static(lblSetOpts, Symbols::settings);
  lv_obj_set_hidden(btnSetOpts, false);
  ///
  btnNextTime = lv_btn_create(lv_scr_act(), btnClose);
  lv_obj_align(btnNextTime, lv_scr_act(), LV_ALIGN_IN_RIGHT_MID, -15, -80);
  lv_obj_t* lblNextTime = lv_label_create(btnNextTime, nullptr);
  lv_label_set_text_static(lblNextTime, ">");

  btnPrevTime = lv_btn_create(lv_scr_act(), btnClose);
  lv_obj_align(btnPrevTime, lv_scr_act(), LV_ALIGN_IN_LEFT_MID, 15, -80);
  lv_obj_t* lblPrevTime = lv_label_create(btnPrevTime, nullptr);
  lv_label_set_text_static(lblPrevTime, "<");

  btnNextBar = lv_btn_create(lv_scr_act(), btnClose);
  lv_obj_align(btnNextBar, lv_scr_act(), LV_ALIGN_IN_RIGHT_MID, -15, 0);
  lv_obj_t* lblNextBar = lv_label_create(btnNextBar, nullptr);
  lv_label_set_text_static(lblNextBar, ">");

  btnPrevBar = lv_btn_create(lv_scr_act(), btnClose);
  lv_obj_align(btnPrevBar, lv_scr_act(), LV_ALIGN_IN_LEFT_MID, 15, 0);
  lv_obj_t* lblPrevBar = lv_label_create(btnPrevBar, nullptr);
  lv_label_set_text_static(lblPrevBar, "<");

  btnNextBG = lv_btn_create(lv_scr_act(), btnClose);
  lv_obj_align(btnNextBG, lv_scr_act(), LV_ALIGN_IN_RIGHT_MID, -15, 80);
  lv_obj_t* lblNextBG = lv_label_create(btnNextBG, nullptr);
  lv_label_set_text_static(lblNextBG, ">");

  btnPrevBG = lv_btn_create(lv_scr_act(), btnClose);
  lv_obj_align(btnPrevBG, lv_scr_act(), LV_ALIGN_IN_LEFT_MID, 15, 80);
  lv_obj_t* lblPrevBG = lv_label_create(btnPrevBG, nullptr);
  lv_label_set_text_static(lblPrevBG, "<");

  btnReset = lv_btn_create(lv_scr_act(), btnClose);
  lv_obj_align(btnReset, lv_scr_act(), LV_ALIGN_CENTER, 0, 80);
  lv_obj_t* lblReset = lv_label_create(btnReset, nullptr);
  lv_label_set_text_static(lblReset, "Rst");

  btnRandom = lv_btn_create(lv_scr_act(), btnClose);
  lv_obj_align(btnRandom, lv_scr_act(), LV_ALIGN_CENTER, 0, 0);
  lv_obj_t* lblRandom = lv_label_create(btnRandom, nullptr);
  lv_label_set_text_static(lblRandom, "Rnd");

  btnSteps = lv_btn_create(lv_scr_act(), btnSetColor);
  lv_obj_align(btnSteps, lv_scr_act(), LV_ALIGN_CENTER, 0, -10);
  lv_obj_t* lblSteps = lv_label_create(btnSteps, nullptr);
  lv_label_set_text_static(lblSteps, "Steps style");
    
  btnWeather = lv_btn_create(lv_scr_act(), btnSteps);
  lv_obj_align(btnWeather, lv_scr_act(), LV_ALIGN_CENTER, 0, 60);
  lv_obj_t* lblWeather = lv_label_create(btnWeather, nullptr);
  lv_label_set_text_static(lblWeather, "Weather");

  lv_obj_set_hidden(btnSetColor, false);

  savedTick = lv_tick_get();
}

void WatchFacePineTimeStyle::showWeather() {
  temperature = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(temperature, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  weatherIcon = lv_label_create(lv_scr_act(), temperature);
  lv_obj_set_style_local_text_font(weatherIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &fontawesome_weathericons);
  lv_obj_align(weatherIcon, sidebar, LV_ALIGN_IN_TOP_MID, 0, 35);
}

bool WatchFacePineTimeStyle::UnLoad() {
  if (running) {
    running = false;
    lv_obj_clean(lv_scr_act());
  }
  return true;
}

WatchFacePineTimeStyle::~WatchFacePineTimeStyle() {
  UnLoad();
}

void WatchFacePineTimeStyle::closeMenu() {
  lv_obj_del(btnNextTime);
  lv_obj_del(btnPrevTime);
  lv_obj_del(btnNextBar);
  lv_obj_del(btnPrevBar);
  lv_obj_del(btnNextBG);
  lv_obj_del(btnPrevBG);
  lv_obj_del(btnReset);
  lv_obj_del(btnRandom);
  lv_obj_del(btnClose);
  lv_obj_del(btnSteps);
  lv_obj_del(btnWeather);
  lv_obj_del(btnSetColor);
  lv_obj_del(btnSetOpts);
  btnClose = NULL;
  settingsController->SaveSettings();
}

bool WatchFacePineTimeStyle::OnTouchEvent(Applications::TouchEvents event) {
  if (running) {
    if (event == Applications::TouchEvents::LongTap && !btnClose) {
      showMenu();
      return true;
    }
    if (event == Applications::TouchEvents::DoubleTap && btnClose) {
      return true;
    }
  }
  return false;
}

bool WatchFacePineTimeStyle::OnButtonPushed() {
  if (running) {
    if (btnClose) {
      closeMenu();
      return true;
    }
  }
  return false;
}

void WatchFacePineTimeStyle::setBatteryIcon() {
  auto batteryPercent = batteryPercentRemaining.Get();
  batteryIcon.SetBatteryPercentage(batteryPercent);
}

void WatchFacePineTimeStyle::updateWeather(bool force) {
  currentWeather = System::SystemTask::displayApp->systemTask->nimbleController.weatherService.Current();
  if (force || currentWeather.IsUpdated()) {
    auto optCurrentWeather = currentWeather.Get();
    if (optCurrentWeather) {
      int16_t temp = optCurrentWeather->temperature;
      if (settingsController->GetWeatherFormat() == Controllers::Settings::WeatherFormat::Imperial) {
        temp = Controllers::SimpleWeatherService::CelsiusToFahrenheit(temp);
      }
      temp = temp / 100 + (temp % 100 >= 50 ? 1 : 0);
      lv_label_set_text_fmt(temperature, "%d°", temp);
      lv_label_set_text(weatherIcon, Symbols::GetSymbol(optCurrentWeather->iconId));
    } else {
      lv_label_set_text(temperature, "--");
      lv_label_set_text(weatherIcon, Symbols::ban);
    }
    lv_obj_align(temperature, sidebar, LV_ALIGN_IN_TOP_MID, 0, 65);
    lv_obj_realign(temperature);
    lv_obj_realign(weatherIcon);
  }
}

void WatchFacePineTimeStyle::Refresh() {
  auto& batteryController = System::SystemTask::displayApp->batteryController;
  isCharging = batteryController.IsCharging();
  if (!running || isCharging.IsUpdated()) {
    if (isCharging.Get()) {
      lv_obj_set_hidden(batteryIcon.GetObject(), true);
      lv_obj_set_hidden(plugIcon, false);
    } else {
      lv_obj_set_hidden(batteryIcon.GetObject(), false);
      lv_obj_set_hidden(plugIcon, true);
      setBatteryIcon();
    }
  }
  if (!isCharging.Get()) {
    batteryPercentRemaining = batteryController.PercentRemaining();
    if (!running || batteryPercentRemaining.IsUpdated()) {
      setBatteryIcon();
    }
  }
  auto& bleController = System::SystemTask::displayApp->bleController;
  bleState = bleController.IsConnected();
  bleRadioEnabled = bleController.IsRadioEnabled();
  if (!running || bleState.IsUpdated() || bleRadioEnabled.IsUpdated()) {
    lv_label_set_text_static(bleIcon, BleIcon::GetIcon(bleState.Get()));
    lv_obj_realign(bleIcon);
  }

  notificationState = System::SystemTask::displayApp->notificationManager.AreNewNotificationsAvailable();
  if (!running || notificationState.IsUpdated()) {
    lv_label_set_text_static(notificationIcon, NotificationIcon::GetIcon(notificationState.Get()));
  }
  auto& dateTimeController = System::SystemTask::displayApp->dateTimeController;
  currentDateTime = dateTimeController.CurrentDateTime();
  if (!running || currentDateTime.IsUpdated()) {
    auto hour = dateTimeController.Hours();
    auto minute = dateTimeController.Minutes();
    auto second = dateTimeController.Seconds();
    auto year = dateTimeController.Year();
    auto month = dateTimeController.Month();
    auto dayOfWeek = dateTimeController.DayOfWeek();
    auto day = dateTimeController.Day();

    if (displayedHour != hour || displayedMinute != minute) {
      displayedHour = hour;
      displayedMinute = minute;

      if (settingsController->GetClockType() == Controllers::Settings::ClockType::H12) {
        char ampmChar[4] = "A\nM";
        if (hour == 0) {
          hour = 12;
        } else if (hour == 12) {
          ampmChar[0] = 'P';
        } else if (hour > 12) {
          hour = hour - 12;
          ampmChar[0] = 'P';
        }
        lv_label_set_text(timeAMPM, ampmChar);
        // Should be padded with blank spaces, but the space character doesn't exist in the font
        lv_label_set_text_fmt(timeDD1, "%02d", hour);
        lv_label_set_text_fmt(timeDD2, "%02d", minute);
      } else {
        lv_label_set_text_fmt(timeDD1, "%02d", hour);
        lv_label_set_text_fmt(timeDD2, "%02d", minute);
      }
    }

    if (displayedSecond != second) {
      displayedSecond = second;
      lv_label_set_text_fmt(timeDD3, ":%02d", second);
    }

    if ((year != currentYear) || (month != currentMonth) || (dayOfWeek != currentDayOfWeek) || (day != currentDay)) {
      lv_label_set_text_static(dateDayOfWeek, dateTimeController.DayOfWeekShortToString());
      lv_label_set_text_fmt(dateDay, "%d", day);
      lv_obj_realign(dateDay);
      lv_label_set_text_static(dateMonth, dateTimeController.MonthShortToString());

      currentYear = year;
      currentMonth = month;
      currentDayOfWeek = dayOfWeek;
      currentDay = day;
    }
  }

  stepCount = System::SystemTask::displayApp->motionController.NbSteps();
  if (!running || stepCount.IsUpdated()) {
    lv_gauge_set_value(stepGauge, 0, (stepCount.Get() / (settingsController->GetStepsGoal() / 100)) % 100);
    lv_obj_realign(stepGauge);
    lv_label_set_text_fmt(stepValue, "%luK", (stepCount.Get() / 1000));
    lv_obj_realign(stepValue);
    if (stepCount.Get() > settingsController->GetStepsGoal()) {
      lv_obj_set_style_local_line_color(stepGauge, LV_GAUGE_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
      lv_obj_set_style_local_scale_grad_color(stepGauge, LV_GAUGE_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    }
  }

  if (temperature) {
    updateWeather(!running);
  }
  if (btnClose) {
    if (savedTick && (lv_tick_get() - savedTick) > 3000) {
      closeMenu();
    }
  }
  running = true;
}

void WatchFacePineTimeStyle::updateSelected(lv_obj_t* obj) {
  savedTick = lv_tick_get();
  auto valueTime = settingsController->GetPTSColorTime();
  auto valueBar = settingsController->GetPTSColorBar();
  auto valueBG = settingsController->GetPTSColorBG();

  if (obj == btnNextTime) {
    valueTime = GetNext(valueTime);
    if (valueTime == valueBG) {
      valueTime = GetNext(valueTime);
    }
    settingsController->SetPTSColorTime(valueTime);
    lv_obj_set_style_local_text_color(timeDD1, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Convert(valueTime));
    lv_obj_set_style_local_text_color(timeDD2, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Convert(valueTime));
    lv_obj_set_style_local_text_color(timeAMPM, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Convert(valueTime));
  } else if (obj == btnPrevTime) {
    valueTime = GetPrevious(valueTime);
    if (valueTime == valueBG) {
      valueTime = GetPrevious(valueTime);
    }
    settingsController->SetPTSColorTime(valueTime);
    lv_obj_set_style_local_text_color(timeDD1, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Convert(valueTime));
    lv_obj_set_style_local_text_color(timeDD2, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Convert(valueTime));
    lv_obj_set_style_local_text_color(timeAMPM, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Convert(valueTime));
  } else if (obj == btnNextBar) {
    valueBar = GetNext(valueBar);
    if (valueBar == Controllers::Settings::Colors::Black) {
      valueBar = GetNext(valueBar);
    }
    needle_colors[0] = valueBar == Controllers::Settings::Colors::White ? LV_COLOR_BLACK : LV_COLOR_WHITE;
    settingsController->SetPTSColorBar(valueBar);
    lv_obj_set_style_local_bg_color(sidebar, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, Convert(valueBar));
  } else if (obj == btnPrevBar) {
    valueBar = GetPrevious(valueBar);
    if (valueBar == Controllers::Settings::Colors::Black) {
      valueBar = GetPrevious(valueBar);
    }
    needle_colors[0] = valueBar == Controllers::Settings::Colors::White ? LV_COLOR_BLACK : LV_COLOR_WHITE;
    settingsController->SetPTSColorBar(valueBar);
    lv_obj_set_style_local_bg_color(sidebar, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, Convert(valueBar));
  } else if (obj == btnNextBG) {
    valueBG = GetNext(valueBG);
    if (valueBG == valueTime) {
      valueBG = GetNext(valueBG);
    }
    settingsController->SetPTSColorBG(valueBG);
    lv_obj_set_style_local_bg_color(timebar, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, Convert(valueBG));
  } else if (obj == btnPrevBG) {
    valueBG = GetPrevious(valueBG);
    if (valueBG == valueTime) {
      valueBG = GetPrevious(valueBG);
    }
    settingsController->SetPTSColorBG(valueBG);
    lv_obj_set_style_local_bg_color(timebar, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, Convert(valueBG));
  } else if (obj == btnReset) {
    needle_colors[0] = LV_COLOR_WHITE;
    settingsController->SetPTSColorTime(Controllers::Settings::Colors::Teal);
    lv_obj_set_style_local_text_color(timeDD1, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Convert(Controllers::Settings::Colors::Teal));
    lv_obj_set_style_local_text_color(timeDD2, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Convert(Controllers::Settings::Colors::Teal));
    lv_obj_set_style_local_text_color(timeAMPM, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Convert(Controllers::Settings::Colors::Teal));
    settingsController->SetPTSColorBar(Controllers::Settings::Colors::Teal);
    lv_obj_set_style_local_bg_color(sidebar, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, Convert(Controllers::Settings::Colors::Teal));
    settingsController->SetPTSColorBG(Controllers::Settings::Colors::Black);
    lv_obj_set_style_local_bg_color(timebar, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, Convert(Controllers::Settings::Colors::Black));
  } else if (obj == btnRandom) {
    valueTime = static_cast<Controllers::Settings::Colors>(rand() % 17);
    valueBar = static_cast<Controllers::Settings::Colors>(rand() % 17);
    valueBG = static_cast<Controllers::Settings::Colors>(rand() % 17);
    if (valueTime == valueBG) {
      valueBG = GetNext(valueBG);
    }
    if (valueBar == Controllers::Settings::Colors::Black) {
      valueBar = GetPrevious(valueBar);
    }
    needle_colors[0] = valueBar == Controllers::Settings::Colors::White ? LV_COLOR_BLACK : LV_COLOR_WHITE;
    settingsController->SetPTSColorTime(static_cast<Controllers::Settings::Colors>(valueTime));
    lv_obj_set_style_local_text_color(timeDD1, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Convert(valueTime));
    lv_obj_set_style_local_text_color(timeDD2, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Convert(valueTime));
    lv_obj_set_style_local_text_color(timeAMPM, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Convert(valueTime));
    settingsController->SetPTSColorBar(static_cast<Controllers::Settings::Colors>(valueBar));
    lv_obj_set_style_local_bg_color(sidebar, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, Convert(valueBar));
    settingsController->SetPTSColorBG(static_cast<Controllers::Settings::Colors>(valueBG));
    lv_obj_set_style_local_bg_color(timebar, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, Convert(valueBG));
  } else if (obj == btnClose) {
    closeMenu();
  } else if (obj == btnSteps) {
    if (!lv_obj_get_hidden(stepGauge) && (lv_obj_get_hidden(timeDD3))) {
      // show half gauge & seconds
      lv_obj_set_hidden(timeDD3, false);
      lv_obj_set_size(stepGauge, 37, 37);
      lv_obj_align(stepGauge, sidebar, LV_ALIGN_IN_BOTTOM_MID, 0, -10);
      lv_gauge_set_scale(stepGauge, 180, 5, 0);
      lv_gauge_set_angle_offset(stepGauge, 0);
      lv_gauge_set_critical_value(stepGauge, 120);
      settingsController->SetPTSGaugeStyle(Controllers::Settings::PTSGaugeStyle::Half);
    } else if (!lv_obj_get_hidden(timeDD3) && (lv_obj_get_hidden(stepValue))) {
      // show step count & icon
      lv_obj_set_hidden(timeDD3, true);
      lv_obj_set_hidden(stepGauge, true);
      lv_obj_set_hidden(stepValue, false);
      lv_obj_set_hidden(stepIcon, false);
      settingsController->SetPTSGaugeStyle(Controllers::Settings::PTSGaugeStyle::Numeric);
    } else {
      // show full gauge
      lv_obj_set_hidden(stepGauge, false);
      lv_obj_set_hidden(stepValue, true);
      lv_obj_set_hidden(stepIcon, true);
      lv_obj_set_size(stepGauge, 40, 40);
      lv_obj_align(stepGauge, sidebar, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
      lv_gauge_set_scale(stepGauge, 360, 11, 0);
      lv_gauge_set_angle_offset(stepGauge, 180);
      lv_gauge_set_critical_value(stepGauge, 100);
      settingsController->SetPTSGaugeStyle(Controllers::Settings::PTSGaugeStyle::Full);
    }
  } else if (obj == btnWeather) {
    if (temperature) {
      // hide weather
      lv_obj_del(temperature);
      lv_obj_del(weatherIcon);
      temperature = NULL;
      lv_obj_align(calendarOuter, sidebar, LV_ALIGN_CENTER, 0, 0);
      lv_obj_realign(calendarInner);
      lv_obj_realign(calendarBar1);
      lv_obj_realign(calendarBar2);
      lv_obj_realign(calendarCrossBar1);
      lv_obj_realign(calendarCrossBar2);
      lv_obj_realign(dateDayOfWeek);
      lv_obj_realign(dateDay);
      lv_obj_realign(dateMonth);
      settingsController->SetPTSWeather(Controllers::Settings::PTSWeather::Off);
    } else {
      // show weather icon and temperature
      showWeather();
      updateWeather(true);
      lv_obj_align(calendarOuter, sidebar, LV_ALIGN_CENTER, 0, 20);
      lv_obj_realign(calendarInner);
      lv_obj_realign(calendarBar1);
      lv_obj_realign(calendarBar2);
      lv_obj_realign(calendarCrossBar1);
      lv_obj_realign(calendarCrossBar2);
      lv_obj_realign(dateDayOfWeek);
      lv_obj_realign(dateDay);
      lv_obj_realign(dateMonth);
      settingsController->SetPTSWeather(Controllers::Settings::PTSWeather::On);
    }
  } else if (obj == btnSetColor) {
    lv_obj_set_hidden(btnSetColor, true);
    lv_obj_set_hidden(btnSetOpts, true);
    lv_obj_set_hidden(btnNextTime, false);
    lv_obj_set_hidden(btnPrevTime, false);
    lv_obj_set_hidden(btnNextBar, false);
    lv_obj_set_hidden(btnPrevBar, false);
    lv_obj_set_hidden(btnNextBG, false);
    lv_obj_set_hidden(btnPrevBG, false);
    lv_obj_set_hidden(btnReset, false);
    lv_obj_set_hidden(btnRandom, false);
    lv_obj_set_hidden(btnClose, false);
  } else if (obj == btnSetOpts) {
    lv_obj_set_hidden(btnSetColor, true);
    lv_obj_set_hidden(btnSetOpts, true);
    lv_obj_set_hidden(btnSteps, false);
    lv_obj_set_hidden(btnWeather, false);
    lv_obj_set_hidden(btnClose, false);
  }
}

void WatchFacePineTimeStyle::event_handler(lv_obj_t* obj, lv_event_t event) {
  if (event == LV_EVENT_CLICKED)
    static_cast<WatchFacePineTimeStyle*>(obj->user_data)->updateSelected(obj);
}

Pinetime::Controllers::Settings::Colors WatchFacePineTimeStyle::GetNext(Controllers::Settings::Colors color) {
  auto colorAsInt = static_cast<uint8_t>(color);
  Controllers::Settings::Colors nextColor;
  if (colorAsInt < 17) {
    nextColor = static_cast<Controllers::Settings::Colors>(colorAsInt + 1);
  } else {
    nextColor = static_cast<Controllers::Settings::Colors>(0);
  }
  return nextColor;
}

Pinetime::Controllers::Settings::Colors WatchFacePineTimeStyle::GetPrevious(Controllers::Settings::Colors color) {
  auto colorAsInt = static_cast<uint8_t>(color);
  Controllers::Settings::Colors prevColor;

  if (colorAsInt > 0) {
    prevColor = static_cast<Controllers::Settings::Colors>(colorAsInt - 1);
  } else {
    prevColor = static_cast<Controllers::Settings::Colors>(17);
  }
  return prevColor;
}
#endif