#include "WatchFaceDigital.h"
#include "systemtask/SystemTask.h"
#include "components/heartrate/HeartRateController.h"
#include "components/ble/NotificationManager.h"
#include "components/motion/MotionController.h"
#include "displayapp/InfiniTimeTheme.h"

#include "WeatherSymbols.h"
#include "Symbols.h"
#include "NotificationIcon.h"

using namespace Pinetime::Applications::Screens;

WatchFaceDigital::WatchFaceDigital() : Screen(WatchFace::Digital) {
}

void WatchFaceDigital::Load() {
  statusIcons.Create();

  notificationIcon = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(notificationIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_LIME);
  lv_obj_align(notificationIcon, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);

  weatherIcon = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(weatherIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_obj_set_style_local_text_font(weatherIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &fontawesome_weathericons);
  lv_obj_align(weatherIcon, NULL, LV_ALIGN_IN_TOP_MID, -20, 0);

  temperature = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(temperature, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_obj_align(temperature, NULL, LV_ALIGN_IN_TOP_MID, 20, 0);
  label_date = lv_label_create(lv_scr_act(), NULL);
  lv_obj_align(label_date, lv_scr_act(), LV_ALIGN_CENTER, 0, 60);
  lv_obj_set_style_local_text_color(label_date, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::orange);

  label_time = lv_label_create(lv_scr_act(), NULL);
  // lv_obj_set_style_local_text_color(label_time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::orange);
  lv_obj_set_style_local_text_font(label_time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_extrabold_compressed);

  lv_obj_align(label_time, lv_scr_act(), LV_ALIGN_IN_RIGHT_MID, 0, 0);

  label_time_ampm = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text_static(label_time_ampm, "");
  lv_obj_align(label_time_ampm, lv_scr_act(), LV_ALIGN_IN_RIGHT_MID, -30, -55);

  heartbeatIcon = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text_static(heartbeatIcon, Symbols::heartBeat);
  lv_obj_set_style_local_text_color(heartbeatIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xCE1B1B));
  lv_obj_align(heartbeatIcon, lv_scr_act(), LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);

  heartbeatValue = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(heartbeatValue, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xCE1B1B));
  lv_label_set_text_static(heartbeatValue, "");

  stepValue = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(stepValue, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x00FFE7));
  lv_obj_align(stepValue, lv_scr_act(), LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);

  stepIcon = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(stepIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x00FFE7));
  lv_label_set_text_static(stepIcon, Symbols::shoe);
  lv_obj_align(stepIcon, stepValue, LV_ALIGN_OUT_LEFT_MID, -5, 0);
  Refresh();
  lv_obj_align(heartbeatValue, heartbeatIcon, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
}

bool WatchFaceDigital::UnLoad() {
  if (running) {
    running = false;
    lv_obj_clean(lv_scr_act());
  }
  return true;
}

WatchFaceDigital::~WatchFaceDigital() {
  UnLoad();
}

void WatchFaceDigital::Refresh() {
  statusIcons.Update();

  auto* dateTimeController = &System::SystemTask::displayApp->dateTimeController;

  notificationState = System::SystemTask::displayApp->notificationManager.AreNewNotificationsAvailable();
  if (!running || notificationState.IsUpdated()) {
    lv_label_set_text_static(notificationIcon, NotificationIcon::GetIcon(notificationState.Get()));
  }

  currentDateTime = std::chrono::time_point_cast<std::chrono::minutes>(dateTimeController->CurrentDateTime());
  if (!running || currentDateTime.IsUpdated()) {
    uint8_t hour = dateTimeController->Hours();
    uint8_t minute = dateTimeController->Minutes();

    if (System::SystemTask::displayApp->settingsController.GetClockType() == Controllers::Settings::ClockType::H12) {
      char ampmChar[3] = "AM";
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
      lv_obj_align(label_time, lv_scr_act(), LV_ALIGN_IN_RIGHT_MID, 0, 0);
    } else {
      lv_label_set_text_fmt(label_time, "%02d:%02d", hour, minute);
      lv_obj_align(label_time, lv_scr_act(), LV_ALIGN_CENTER, 0, 0);
    }

    currentDate = std::chrono::time_point_cast<std::chrono::days>(currentDateTime.Get());
    if (!running || currentDate.IsUpdated()) {
      uint16_t year = dateTimeController->Year();
      uint8_t day = dateTimeController->Day();
      if (System::SystemTask::displayApp->settingsController.GetClockType() == Controllers::Settings::ClockType::H24) {
        lv_label_set_text_fmt(label_date,
                              "%d %s %d %s",
                              year,
                              dateTimeController->MonthShortToString(),
                              day,
                              dateTimeController->DayOfWeekShortToString());
      } else {
        lv_label_set_text_fmt(label_date,
                              "%s %s %d %d",
                              dateTimeController->DayOfWeekShortToString(),
                              dateTimeController->MonthShortToString(),
                              day,
                              year);
      }
      lv_obj_realign(label_date);
    }
  }

  heartbeat = System::SystemTask::displayApp->heartRateController.HeartRate();
  heartbeatRunning = System::SystemTask::displayApp->heartRateController.State() != Controllers::HeartRateController::States::Stopped;
  if (!running || heartbeat.IsUpdated() || heartbeatRunning.IsUpdated()) {
    if (heartbeatRunning.Get()) {
      lv_obj_set_style_local_text_color(heartbeatIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0xCE1B1B));
      lv_label_set_text_fmt(heartbeatValue, "%d", heartbeat.Get());
    } else {
      lv_obj_set_style_local_text_color(heartbeatIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, lv_color_hex(0x1B1B1B));
      lv_label_set_text_static(heartbeatValue, "");
    }

    lv_obj_realign(heartbeatIcon);
    lv_obj_realign(heartbeatValue);
  }

  stepCount = System::SystemTask::displayApp->motionController.NbSteps();
  if (!running || stepCount.IsUpdated()) {
    lv_label_set_text_fmt(stepValue, "%lu", stepCount.Get());
    lv_obj_realign(stepValue);
    lv_obj_realign(stepIcon);
  }

  currentWeather = System::SystemTask::displayApp->systemTask->nimbleController.weatherService.Current();
  if (!running || currentWeather.IsUpdated()) {
    auto optCurrentWeather = currentWeather.Get();
    if (optCurrentWeather) {
      int16_t temp = optCurrentWeather->temperature;
      char tempUnit = 'C';
      if (System::SystemTask::displayApp->settingsController.GetWeatherFormat() == Controllers::Settings::WeatherFormat::Imperial) {
        temp = Controllers::SimpleWeatherService::CelsiusToFahrenheit(temp);
        tempUnit = 'F';
      }
      temp = temp / 100 + (temp % 100 >= 50 ? 1 : 0);
      lv_label_set_text_fmt(temperature, "%d°%c", temp, tempUnit);
      lv_label_set_text(weatherIcon, Symbols::GetSymbol(optCurrentWeather->iconId));
    } else {
      lv_label_set_text_static(temperature, "");
      lv_label_set_text(weatherIcon, "");
    }
    lv_obj_realign(temperature);
    lv_obj_realign(weatherIcon);
  }
  running = true;
}

bool WatchFaceDigital::OnTouchEvent(uint16_t x, uint16_t y) {
  if (running) {
    if (x < 80 && y > LV_VER_RES - 60) { // heartbeatIcon
      System::SystemTask::displayApp->StartApp(Apps::HeartRate);
      return true;
    }
    uint16_t w = lv_obj_get_width(weatherIcon);
    if (w > 8) { // weather loaded
      uint16_t o = lv_obj_get_x(weatherIcon);
      if (x > o && x < o + 3 * w) {
        o = lv_obj_get_y(weatherIcon);
        if (y > o && y < o + w) {
          System::SystemTask::displayApp->StartApp(Apps::Weather);
          return true;
        }
      }
    }
    if (x > LV_HOR_RES - 80 && y > LV_VER_RES - 60) { // stepIcon
      System::SystemTask::displayApp->StartApp(Apps::Steps);
      return true;
    }
  }
  return false;
}