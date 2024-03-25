#include "SettingSetDate.h"
#include "systemtask/SystemTask.h"
#include "displayapp/screens/Symbols.h"

using namespace Pinetime::Applications::Screens;

namespace {
  constexpr int16_t POS_X_DAY = -72;
  constexpr int16_t POS_X_MONTH = 0;
  constexpr int16_t POS_X_YEAR = 72;
  constexpr int16_t POS_Y_TEXT = -6;

  int MaximumDayOfMonth(uint8_t month, uint16_t year) {
    switch (month) {
      case 2: {
        // TODO: When we start using C++20, use std::chrono::year::is_leap
        if ((((year % 4) == 0) && ((year % 100) != 0)) || ((year % 400) == 0)) {
          return 29;
        }
        return 28;
      }
      case 4:
      case 6:
      case 9:
      case 11:
        return 30;
      default:
        return 31;
    }
  }
}

SettingSetDate::SettingSetDate(Pinetime::Applications::Screens::SettingSetDateTime& settingSetDateTime)
  : settingSetDateTime {settingSetDateTime} {
}

void SettingSetDate::Load() {
  running = true;
  settingSetDateTime.dotIndicator.Create(0,2);
  lv_obj_t* title = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_text_static(title, "Set current date");
  lv_label_set_align(title, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(title, lv_scr_act(), LV_ALIGN_IN_TOP_MID, 15, 15);

  lv_obj_t* icon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(icon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_ORANGE);

  lv_label_set_text_static(icon, Symbols::clock);
  lv_label_set_align(icon, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(icon, title, LV_ALIGN_OUT_LEFT_MID, -10, 0);

  dayCounter.SetValueChangedEventCallback(this, ValueChangedHandler);
  dayCounter.Create();
  auto* dc = &System::SystemTask::displayApp->dateTimeController;
  dayCounter.SetValue(dc->Day());
  lv_obj_align(dayCounter.GetObject(), nullptr, LV_ALIGN_CENTER, POS_X_DAY, POS_Y_TEXT);

  monthCounter.EnableMonthMode();
  monthCounter.SetValueChangedEventCallback(this, ValueChangedHandler);
  monthCounter.Create();
  monthCounter.SetValue(static_cast<int>(dc->Month()));
  lv_obj_align(monthCounter.GetObject(), nullptr, LV_ALIGN_CENTER, POS_X_MONTH, POS_Y_TEXT);

  yearCounter.SetValueChangedEventCallback(this, ValueChangedHandler);
  yearCounter.Create();
  yearCounter.SetValue(dc->Year());
  lv_obj_align(yearCounter.GetObject(), nullptr, LV_ALIGN_CENTER, POS_X_YEAR, POS_Y_TEXT);

  btnSetTime = lv_btn_create(lv_scr_act(), nullptr);
  btnSetTime->user_data = this;
  lv_obj_set_size(btnSetTime, 120, 48);
  lv_obj_align(btnSetTime, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, 0);
  lv_obj_set_style_local_bg_color(btnSetTime, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x38, 0x38, 0x38));
  lblSetTime = lv_label_create(btnSetTime, nullptr);
  lv_label_set_text_static(lblSetTime, "Set");
  lv_obj_set_event_cb(btnSetTime, event_handler);
}

bool SettingSetDate::UnLoad() {
  if (running) {
    running = false;
    lv_obj_clean(lv_scr_act());
  }
  return true;
}

SettingSetDate::~SettingSetDate() {
  UnLoad();
}

void SettingSetDate::handleButtonPress() {
  const uint16_t yearValue = yearCounter.GetValue();
  const uint8_t monthValue = monthCounter.GetValue();
  const uint8_t dayValue = dayCounter.GetValue();
  // NRF_LOG_INFO("Setting date (manually) to %04d-%02d-%02d", yearValue, monthValue, dayValue);
  auto* dc = &System::SystemTask::displayApp->dateTimeController;
  dc->SetTime(yearValue, monthValue, dayValue, dc->Hours(), dc->Minutes(), dc->Seconds());
  settingSetDateTime.Advance();
}

void SettingSetDate::checkDay() {
  const int maxDay = MaximumDayOfMonth(monthCounter.GetValue(), yearCounter.GetValue());
  dayCounter.SetMax(maxDay);
}


  void SettingSetDate::event_handler(lv_obj_t* obj, lv_event_t event) {   
    if (event == LV_EVENT_CLICKED) {
      static_cast<SettingSetDate*>(obj->user_data)->handleButtonPress();
    }
  }

  void SettingSetDate::ValueChangedHandler(void* userData) {
    static_cast<SettingSetDate*>(userData)->checkDay();
  }
