#include "displayapp/screens/Calendar.h"
#ifdef UseCalendar
#include "systemtask/SystemTask.h"
#include "components/datetime/DateTimeController.h"

using namespace Pinetime::Applications::Screens;

Calendar::Calendar() : ScreenRefresh(Apps::Calendar) {
}

void Calendar::Load() {
  running = true;
  statusIcons.Load();
  label_time = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_align(label_time, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(label_time, lv_scr_act(), LV_ALIGN_IN_TOP_LEFT, 0, 0);
  lv_obj_set_style_local_text_color(label_time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_YELLOW);
  // Create calendar object
  calendar = lv_calendar_create(lv_scr_act(), NULL);
  // Set size
  lv_obj_set_size(calendar, 230, 200);
  // Set alignment
  lv_obj_align(calendar, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -5);
  // Disable clicks
  lv_obj_set_click(calendar, false);

  // Set background of today's date
  //lv_obj_set_style_local_bg_opa(calendar, LV_CALENDAR_PART_DATE, LV_STATE_FOCUSED, LV_OPA_COVER);
  lv_obj_set_style_local_bg_color(calendar, LV_CALENDAR_PART_DATE, LV_STATE_FOCUSED, LV_COLOR_WHITE);
  lv_obj_set_style_local_radius(calendar, LV_CALENDAR_PART_DATE, LV_STATE_FOCUSED, 3);

  // Set style of today's date
  lv_obj_set_style_local_text_color(calendar, LV_CALENDAR_PART_DATE, LV_STATE_FOCUSED, LV_COLOR_CYAN);

  // Set style of inactive month's days
  lv_obj_set_style_local_text_color(calendar, LV_CALENDAR_PART_DATE, LV_STATE_DISABLED, lv_color_hex(0x505050));
  
   //lv_calendar_get_pressed_date
   // lv_calendar_set_highlighted_dates
  auto& dateTimeController = System::SystemTask::displayApp->dateTimeController;
  // Get today's date
  today.year = static_cast<uint16_t>(dateTimeController.Year());
  today.month = static_cast<uint8_t>(dateTimeController.Month());
  today.day = static_cast<uint8_t>(dateTimeController.Day());

  // Set today's date
  lv_calendar_set_today_date(calendar, &today);
  lv_calendar_set_showed_date(calendar, &today);

  // Use today's date as a reference for which month to show if moved
  current = today;
  createRefreshTask(5000, LV_TASK_PRIO_MID);
  Refresh();
}

void Calendar::Refresh() {
  lv_label_set_text(label_time, System::SystemTask::displayApp->dateTimeController.FormattedTime().c_str());
  statusIcons.Refresh();
}


bool Calendar::OnTouchEvent(Applications::TouchEvents event) {
  switch (event) {
    case TouchEvents::SwipeLeft: {
      if (current.month == 12) {
        current.month = 1;
        current.year++;
      } else {
        current.month++;
      }
      lv_calendar_set_showed_date(calendar, &current);
      return true;
    }
    case TouchEvents::SwipeRight: {
      if (current.month == 1) {
        current.month = 12;
        current.year--;
      } else {
        current.month--;
      }
      lv_calendar_set_showed_date(calendar, &current);
      return true;
    }
    default: {
      return false;
    }
  }
}
#endif