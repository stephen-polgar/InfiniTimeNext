#include "displayapp/screens/WatchFaceInfineat.h"
#ifdef UseWatchFaceInfineat
  #include "systemtask/SystemTask.h"
  #include "displayapp/screens/Symbols.h"
  #include "displayapp/screens/BleIcon.h"
  #include "components/battery/BatteryController.h"
  #include "components/datetime/DateTimeController.h"
  #include "components/ble/BleController.h"
  #include "components/ble/NotificationManager.h"
  #include "components/motion/MotionController.h"

using namespace Pinetime::Applications::Screens;

WatchFaceInfineat::WatchFaceInfineat() : Screen(WatchFace::Infineat) {
  font_teko = lv_font_load("F:/fonts/teko.bin");
  font_bebas = lv_font_load("F:/fonts/bebas.bin");
  settingsController = &System::SystemTask::displayApp->settingsController;
}

void WatchFaceInfineat::Load() {
  if (!font_teko || !font_bebas) {
    return;
  }
  // Side Cover
  const std::array<lv_color_t, nLines>* colors = returnColor(static_cast<enum colors>(settingsController->GetInfineatColorIndex()));
  for (uint8_t i = 0; i < nLines; i++) {
    lines[i] = lv_line_create(lv_scr_act(), nullptr);
    lv_obj_set_style_local_line_width(lines[i], LV_LINE_PART_MAIN, LV_STATE_DEFAULT, lineWidths[i]);
    lv_obj_set_style_local_line_color(lines[i], LV_LINE_PART_MAIN, LV_STATE_DEFAULT, (*colors)[i]);
    lv_line_set_points(lines[i], linePoints[i], 2);
  }

  logoPine = lv_img_create(lv_scr_act(), nullptr);
  lv_img_set_src(logoPine, "F:/images/pine_small.bin");
  lv_obj_set_pos(logoPine, 15, 106);

  lineBattery = lv_line_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_line_width(lineBattery, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, 24);
  lv_obj_set_style_local_line_color(lineBattery, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, (*colors)[4]);
  lv_obj_set_style_local_line_opa(lineBattery, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, 190);
  lineBatteryPoints[0] = {27, 105};
  lineBatteryPoints[1] = {27, 106};
  lv_line_set_points(lineBattery, lineBatteryPoints, 2);
  lv_obj_move_foreground(lineBattery);

  notificationIcon = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_bg_color(notificationIcon, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, (*colors)[7]);
  lv_obj_set_style_local_radius(notificationIcon, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
  lv_obj_set_size(notificationIcon, 13, 13);
  lv_obj_set_hidden(notificationIcon, true);

  if (!settingsController->GetInfineatShowSideCover()) {
    toggleBatteryIndicatorColor(false);
    for (auto& line : lines) {
      lv_obj_set_hidden(line, true);
    }
  }

  timeContainer = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_bg_opa(timeContainer, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
  lv_obj_set_size(timeContainer, 185, 185);
  lv_obj_align(timeContainer, lv_scr_act(), LV_ALIGN_CENTER, 0, -10);

  labelHour = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(labelHour, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, font_bebas);

  labelMinutes = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(labelMinutes, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, font_bebas);
  #ifndef TimeFormat_24H
  labelTimeAmPm = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(labelTimeAmPm, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, font_teko);
  #endif
  dateContainer = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_bg_opa(dateContainer, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
  lv_obj_set_size(dateContainer, 60, 30);
  lv_obj_align(dateContainer, lv_scr_act(), LV_ALIGN_IN_RIGHT_MID, 0, 5);

  static constexpr lv_color_t grayColor = LV_COLOR_MAKE(0x99, 0x99, 0x99);
  labelDate = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(labelDate, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_obj_set_style_local_text_font(labelDate, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, font_teko);

  bleIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(bleIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLUE);
  lv_label_set_text_static(bleIcon, Symbols::bluetooth);

  stepValue = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(stepValue, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_obj_set_style_local_text_font(stepValue, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, font_teko);
  lv_obj_align(stepValue, lv_scr_act(), LV_ALIGN_IN_BOTTOM_RIGHT, 10, 0);

  stepIcon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(stepIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, grayColor);
  lv_label_set_text_static(stepIcon, Symbols::shoe);
  lv_obj_align(stepIcon, stepValue, LV_ALIGN_OUT_LEFT_MID, -5, 0);

  Refresh();

  lv_obj_align(labelHour, timeContainer, LV_ALIGN_IN_TOP_MID, 0, 0);
  lv_obj_align(labelMinutes, timeContainer, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
  #ifndef TimeFormat_24H
  lv_obj_align(labelTimeAmPm, timeContainer, LV_ALIGN_OUT_RIGHT_TOP, 0, 15);
  #endif
  lv_obj_align(labelDate, dateContainer, LV_ALIGN_IN_TOP_MID, 0, 0);
  lv_obj_align(bleIcon, dateContainer, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
  running = true;
}

void WatchFaceInfineat::showMenu() {
  btnClose = lv_btn_create(lv_scr_act(), nullptr);
  btnClose->user_data = this;
  lv_obj_set_size(btnClose, 60, 60);
  lv_obj_align(btnClose, lv_scr_act(), LV_ALIGN_CENTER, 0, -80);
  lv_obj_set_style_local_bg_opa(btnClose, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_70);
  lv_obj_t* lblClose = lv_label_create(btnClose, nullptr);
  lv_label_set_text_static(lblClose, "X");
  lv_obj_set_event_cb(btnClose, event_handler);

  btnNextColor = lv_btn_create(lv_scr_act(), btnClose);
  lv_obj_align(btnNextColor, lv_scr_act(), LV_ALIGN_IN_RIGHT_MID, -15, 0);
  lv_obj_t* lblNextColor = lv_label_create(btnNextColor, nullptr);
  lv_label_set_text_static(lblNextColor, ">");

  btnPrevColor = lv_btn_create(lv_scr_act(), btnClose);
  lv_obj_align(btnPrevColor, lv_scr_act(), LV_ALIGN_IN_LEFT_MID, 15, 0);
  lv_obj_t* lblPrevColor = lv_label_create(btnPrevColor, nullptr);
  lv_label_set_text_static(lblPrevColor, "<");

  btnToggleCover = lv_btn_create(lv_scr_act(), btnClose);
  lv_obj_align(btnToggleCover, lv_scr_act(), LV_ALIGN_CENTER, 0, 80);
  lblToggle = lv_label_create(btnToggleCover, nullptr);
  lv_label_set_text_static(lblToggle, settingsController->GetInfineatShowSideCover() ? "ON" : "OFF");

  savedTick = lv_tick_get();
}

bool WatchFaceInfineat::UnLoad() {
  if (running) {
    running = false;
    lv_obj_clean(lv_scr_act());
  }
  return (font_teko && font_bebas);
}

WatchFaceInfineat::~WatchFaceInfineat() {
  UnLoad();
  if (font_bebas)
    lv_font_free(font_bebas);
  if (font_teko)
    lv_font_free(font_teko);
}

void WatchFaceInfineat::closeMenu() {
  lv_obj_del(btnClose);
  lv_obj_del(btnNextColor);
  lv_obj_del(btnPrevColor);
  lv_obj_del(btnToggleCover);
  btnClose = NULL;
  settingsController->SaveSettings();
}

bool WatchFaceInfineat::OnTouchEvent(Pinetime::Applications::TouchEvents event) {
  if (running) {
    if (event == Applications::TouchEvents::LongTap && !btnClose) {
      showMenu();
      return true;
    }
    // Prevent screen from sleeping when double tapping with settings on
    if (event == Applications::TouchEvents::DoubleTap && btnClose) {
      return true;
    }
  }
  return false;
}

bool WatchFaceInfineat::OnButtonPushed() {
  if (running) {
    if (btnClose) {
      closeMenu();
      return true;
    }
  }
  return false;
}

void WatchFaceInfineat::updateColors(int8_t colorIndex) {
  const std::array<lv_color_t, nLines>* colors = returnColor(static_cast<enum colors>(colorIndex));
  for (uint8_t i = 0; i < nLines; i++) {
    lv_color_t color = (*colors)[i];
    lv_obj_set_style_local_line_color(lines[i], LV_LINE_PART_MAIN, LV_STATE_DEFAULT, color);
  }
  lv_obj_set_style_local_line_color(lineBattery, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, (*colors)[4]);
  lv_obj_set_style_local_bg_color(notificationIcon, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, (*colors)[7]);
  settingsController->SetInfineatColorIndex(colorIndex);
}

void WatchFaceInfineat::updateSelected(lv_obj_t* obj) {
  savedTick = lv_tick_get();
  if (obj == btnClose) {
    closeMenu();
  } else if (obj == btnToggleCover) {
    bool showSideCover = settingsController->GetInfineatShowSideCover();
    settingsController->SetInfineatShowSideCover(!showSideCover);
    toggleBatteryIndicatorColor(!showSideCover);
    for (auto& line : lines) {
      lv_obj_set_hidden(line, showSideCover);
    }
    lv_obj_set_hidden(btnNextColor, showSideCover);
    lv_obj_set_hidden(btnPrevColor, showSideCover);
    lv_label_set_text_static(lblToggle, showSideCover ? "OFF" : "ON");
  } else if (obj == btnNextColor) {
    int8_t colorIndex = settingsController->GetInfineatColorIndex();
    colorIndex = (colorIndex + 1) % nColors;
    updateColors(colorIndex);
  } else if (obj == btnPrevColor) {
    int8_t colorIndex = settingsController->GetInfineatColorIndex();
    if (--colorIndex < 0)
      colorIndex = nColors - 1;
    updateColors(colorIndex);
  }
}

void WatchFaceInfineat::event_handler(lv_obj_t* obj, lv_event_t event) {
  if (event == LV_EVENT_CLICKED)
    static_cast<WatchFaceInfineat*>(obj->user_data)->updateSelected(obj);
}

void WatchFaceInfineat::Refresh() {
  notificationState = System::SystemTask::displayApp->notificationManager.AreNewNotificationsAvailable();
  if (!running || notificationState.IsUpdated()) {
    lv_obj_set_hidden(notificationIcon, !notificationState.Get());
    lv_obj_align(notificationIcon, lv_scr_act(), LV_ALIGN_IN_TOP_RIGHT, 0, 0);
  }
  auto& dateTimeController = System::SystemTask::displayApp->dateTimeController;
  currentDateTime = std::chrono::time_point_cast<std::chrono::minutes>(dateTimeController.CurrentDateTime());
  if (!running || currentDateTime.IsUpdated()) {
  #ifdef TimeFormat_24H
    lv_label_set_text_fmt(labelHour, "%02d", dateTimeController.Hours());
    lv_label_set_text_fmt(labelMinutes, "%02d", dateTimeController.Minutes());
  #else
    uint8_t hour = dateTimeController.Hours();
    uint8_t minute = dateTimeController.Minutes();
    char ampmChar[3] = "AM";
    if (hour == 0) {
      hour = 12;
    } else if (hour == 12) {
      ampmChar[0] = 'P';
    } else if (hour > 12) {
      hour = hour - 12;
      ampmChar[0] = 'P';
    }
    lv_label_set_text(labelTimeAmPm, ampmChar);
    lv_label_set_text_fmt(labelHour, "%02d", hour);
    lv_label_set_text_fmt(labelMinutes, "%02d", minute);
    lv_obj_align(labelTimeAmPm, timeContainer, LV_ALIGN_OUT_RIGHT_TOP, 0, 10);
    lv_obj_align(labelHour, timeContainer, LV_ALIGN_IN_TOP_MID, 0, 5);
    lv_obj_align(labelMinutes, timeContainer, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
  #endif
    currentDate = std::chrono::time_point_cast<std::chrono::days>(currentDateTime.Get());
    if (!running || currentDate.IsUpdated()) {    
      Controllers::DateTime::Days dayOfWeek = dateTimeController.DayOfWeek();
      lv_label_set_text_fmt(labelDate, "%s %02d", dateTimeController.DayOfWeekShortToStringLow(dayOfWeek), dateTimeController.Day());
      lv_obj_realign(labelDate);
    }
  }

  auto& batteryController = System::SystemTask::displayApp->batteryController;
  batteryPercentRemaining = batteryController.PercentRemaining();
  isCharging = batteryController.IsCharging();
  if (!running || batteryController.IsCharging()) { // Charging battery animation
    chargingBatteryPercent += 1;
    if (chargingBatteryPercent > 100) {
      chargingBatteryPercent = batteryPercentRemaining.Get();
    }
    setBatteryLevel(chargingBatteryPercent);
  } else if (!running || isCharging.IsUpdated() || batteryPercentRemaining.IsUpdated()) {
    chargingBatteryPercent = batteryPercentRemaining.Get();
    setBatteryLevel(chargingBatteryPercent);
  }
  auto& bleController = System::SystemTask::displayApp->bleController;
  bleState = bleController.IsConnected();
  bleRadioEnabled = bleController.IsRadioEnabled();
  if (!running || bleState.IsUpdated()) {
    lv_label_set_text_static(bleIcon, BleIcon::GetIcon(bleState.Get()));
    lv_obj_align(bleIcon, dateContainer, LV_ALIGN_OUT_BOTTOM_MID, 0, 3);
  }

  stepCount = System::SystemTask::displayApp->motionController.NbSteps();
  if (!running || stepCount.IsUpdated()) {
    lv_label_set_text_fmt(stepValue, "%lu", stepCount.Get());
    lv_obj_align(stepValue, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 10, 0);
    lv_obj_align(stepIcon, stepValue, LV_ALIGN_OUT_LEFT_MID, -5, 0);
  }

  if (btnClose) {
    if (savedTick && (lv_tick_get() - savedTick) > 3000) {
      closeMenu();
      savedTick = 0;
    }
  }
}

void WatchFaceInfineat::setBatteryLevel(uint8_t batteryPercent) {
  // starting point (y) + Pine64 logo height * (100 - batteryPercent) / 100
  lineBatteryPoints[1] = {27, static_cast<lv_coord_t>(105 + 32 * (100 - batteryPercent) / 100)};
  lv_line_set_points(lineBattery, lineBatteryPoints, 2);
}

void WatchFaceInfineat::toggleBatteryIndicatorColor(bool showSideCover) {
  if (!showSideCover) { // make indicator and notification icon color white
    lv_obj_set_style_local_image_recolor_opa(logoPine, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_100);
    lv_obj_set_style_local_image_recolor(logoPine, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_line_color(lineBattery, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_obj_set_style_local_bg_color(notificationIcon, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  } else {
    lv_obj_set_style_local_image_recolor_opa(logoPine, LV_IMG_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_0);
    const std::array<lv_color_t, nLines>* colors =
      returnColor(static_cast<enum colors>(System::SystemTask::displayApp->settingsController.GetInfineatColorIndex()));
    lv_obj_set_style_local_line_color(lineBattery, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, (*colors)[4]);
    lv_obj_set_style_local_bg_color(notificationIcon, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, (*colors)[7]);
  }
}

bool WatchFaceInfineat::IsAvailable(Controllers::FS& filesystem) {
  lfs_file file;
  if (filesystem.FileOpen(&file, "/fonts/teko.bin", LFS_O_RDONLY) < 0) {
    return false;
  }
  filesystem.FileClose(&file);
  if (filesystem.FileOpen(&file, "/fonts/bebas.bin", LFS_O_RDONLY) < 0) {
    return false;
  }
  filesystem.FileClose(&file);
  if (filesystem.FileOpen(&file, "/images/pine_small.bin", LFS_O_RDONLY) < 0) {
    return false;
  }
  filesystem.FileClose(&file);
  return true;
}
#endif