#include "AlarmSet.h"
#include "Symbols.h"
#include "systemtask/SystemTask.h"
#include "displayapp/InfiniTimeTheme.h"

// #define Log

#ifdef Log
  #include <nrf_log.h>
#endif

using namespace Pinetime::Applications::Screens;

AlarmSet::AlarmSet(bool showAlarm) : Screen(Apps::AlarmSet) {
#ifdef Log
  NRF_LOG_INFO("new AlarmSet=%d", this);
#endif
  if (showAlarm) {
    for (auto* alarmC : AlarmController::alarmControllers) {
      if (alarmC->State() == AlarmController::AlarmState::Alerting) {
        alarmController = alarmC;
        return;
      }
    }
  }
  alarmController = new AlarmController();
}

AlarmSet::AlarmSet(AlarmController* alarmController) : Screen(Apps::AlarmSet), alarmController {alarmController} {
#ifdef Log
  NRF_LOG_INFO("new AlarmSet=%d", this);
#endif
  if (!alarmController)
    this->alarmController = new AlarmController();
}

void AlarmSet::Load() {
#ifdef Log
  //  NRF_LOG_INFO("AlarmSet.Load=%d", this);
#endif
  running = true;
  hourCounter.Create();
  lv_obj_align(hourCounter.GetObject(), NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
#ifndef TimeFormat_24H
  lblampm = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_font(lblampm, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_bold_20);
  lv_label_set_text_static(lblampm, "AM");
  lv_label_set_align(lblampm, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(lblampm, lv_scr_act(), LV_ALIGN_CENTER, 0, 30);
#endif
  hourCounter.SetValue(alarmController->Hours());
  hourCounter.SetValueChangedEventCallback(this, valueChangedHandler);

  minuteCounter.Create();
  lv_obj_align(minuteCounter.GetObject(), NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
  minuteCounter.SetValue(alarmController->Minutes());
  minuteCounter.SetValueChangedEventCallback(this, valueChangedHandler);

  lv_obj_t* colonLabel = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_font(colonLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_76);
  lv_label_set_text_static(colonLabel, ":");
  lv_obj_align(colonLabel, lv_scr_act(), LV_ALIGN_CENTER, 0, -29);

  btnStop = lv_btn_create(lv_scr_act(), NULL);
  btnStop->user_data = this;
  lv_obj_set_event_cb(btnStop, [](lv_obj_t* obj, lv_event_t event) {
    if (event == LV_EVENT_CLICKED)
      (static_cast<AlarmSet*>(obj->user_data))->StopAlerting();
  });
  lv_obj_set_size(btnStop, 240, 60);
  lv_obj_align(btnStop, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, 0);
  lv_obj_set_style_local_bg_color(btnStop, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_RED);
  lv_obj_t* txtStop = lv_label_create(btnStop, NULL);
  lv_label_set_text_static(txtStop, Symbols::stop);
  lv_obj_set_hidden(btnStop, true);

  static constexpr lv_color_t bgColor = Colors::bgAlt;

  btnRecur = lv_btn_create(lv_scr_act(), NULL);
  btnRecur->user_data = this;
  lv_obj_set_event_cb(btnRecur, [](lv_obj_t* obj, lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
      (static_cast<AlarmSet*>(obj->user_data))->showRecurences();
    }
  });
  lv_obj_set_size(btnRecur, 240, 56);
  lv_obj_align(btnRecur, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, 0);
  txtRecur = lv_label_create(btnRecur, NULL);
  lv_label_set_recolor(txtRecur, true);
  lv_obj_set_style_local_bg_color(btnRecur, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, bgColor);
  updateRecurencesButton();
  UpdateAlarmTime();

  if (alarmController->State() == AlarmController::AlarmState::Alerting) {
    SetAlerting();
  }
}

bool AlarmSet::UnLoad() {
  if (running) {
    running = false;
    if (alarmController->State() != AlarmController::AlarmState::Not_Set)
      alarmController->ScheduleAlarm();
    lv_obj_clean(lv_scr_act());
  }
  return true;
}

AlarmSet::~AlarmSet() {
#ifdef Log
  NRF_LOG_INFO("delete AlarmSet=%d", this);
#endif
  if (alarmController->State() == AlarmController::AlarmState::Alerting) {
    StopAlerting();
  }
  UnLoad();
  if (changed)
    AlarmController::Save();
}

bool AlarmSet::OnButtonPushed() {
  if (alarmController->State() == AlarmController::AlarmState::Alerting) {
    StopAlerting();
    return true;
  }
  return false;
}

bool AlarmSet::OnTouchEvent(Pinetime::Applications::TouchEvents event) {
  // Don't allow closing the screen by swiping while the alarm is alerting
  return alarmController->State() == AlarmController::AlarmState::Alerting && event == TouchEvents::SwipeDown;
}

void AlarmSet::valueChangedHandler(void* userData) {
  (static_cast<AlarmSet*>(userData))->onValueChanged();
}

void AlarmSet::onValueChanged() {
  // DisableAlarm();
  UpdateAlarmTime();
  changed = true;
}

void AlarmSet::UpdateAlarmTime() {
#ifndef TimeFormat_24H
  lv_label_set_text_static(lblampm, hourCounter.GetValue() >= 12 ? "PM" : "AM");
#endif
  alarmController->SetAlarmTime(hourCounter.GetValue(), minuteCounter.GetValue());
}

void AlarmSet::SetAlerting() {
  lv_obj_set_hidden(btnRecur, true);
  lv_obj_set_hidden(btnStop, false);
  taskStopAlarm = lv_task_create(
    [](lv_task_t* task) {
      (static_cast<AlarmSet*>(task->user_data))->StopAlerting();
    },
    pdMS_TO_TICKS(60 * 1000),
    LV_TASK_PRIO_MID,
    this);
}

void AlarmSet::StopAlerting() {
  alarmController->StopAlerting();
  System::SystemTask::displayApp->motorController.StopRinging();
  if (taskStopAlarm != NULL) {
    lv_task_del(taskStopAlarm);
    taskStopAlarm = NULL;
  }
  System::SystemTask::displayApp->systemTask->PushMessage(System::Messages::EnableSleeping);
  lv_obj_set_hidden(btnRecur, false);
  lv_obj_set_hidden(btnStop, true);
}

void AlarmSet::showRecurences() {
  cont1 = lv_cont_create(lv_scr_act(), NULL);
  lv_obj_set_pos(cont1, 0, 0);
  lv_obj_set_size(cont1, LV_HOR_RES / 2 - 20, LV_VER_RES);
  lv_obj_set_style_local_bg_color(cont1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_set_style_local_pad_all(cont1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 1);
  lv_obj_set_style_local_pad_inner(cont1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 1);
  lv_obj_set_style_local_border_width(cont1, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 1);

  // lv_cont_set_fit(cont, LV_FIT_TIGHT);
  lv_cont_set_layout(cont1, LV_LAYOUT_COLUMN_LEFT);
  // lv_cont_set_layout(cont, LV_LAYOUT_GRID);
  uint16_t i;
  auto rec = alarmController->Recurrence();
  for (i = 0; i < 7; i++) {
    lv_obj_t* cb = lv_checkbox_create(cont1, NULL);
    switch (i) {
#ifdef WeekStartsMonday
      case 0:
        lv_checkbox_set_checked(cb, rec.Mon);
        break;
      case 1:
        lv_checkbox_set_checked(cb, rec.Tue);
        break;
      case 2:
        lv_checkbox_set_checked(cb, rec.Wed);
        break;
      case 3:
        lv_checkbox_set_checked(cb, rec.Thu);
        break;
      case 4:
        lv_checkbox_set_checked(cb, rec.Fri);
        break;
      case 5:
        lv_obj_set_style_local_text_color(cb, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_RED);
        lv_checkbox_set_checked(cb, rec.Sat);
        break;
      case 6:
        lv_obj_set_style_local_text_color(cb, LV_CHECKBOX_PART_BG, LV_STATE_DEFAULT, LV_COLOR_RED);
        lv_checkbox_set_checked(cb, rec.Sun);
#else
      case 0:
        lv_obj_set_style_local_text_color(cb, LV_CHECKBOX_PART_BG, LV_STATE_DEFAULT, LV_COLOR_RED);
        lv_checkbox_set_checked(cb, rec.Sun);
        break;
      case 1:
        lv_checkbox_set_checked(cb, rec.Mon);
        break;
      case 2:
        lv_checkbox_set_checked(cb, rec.Tue);
        break;
      case 3:
        lv_checkbox_set_checked(cb, rec.Wed);
        break;
      case 4:
        lv_checkbox_set_checked(cb, rec.Thu);
        break;
      case 5:
        lv_checkbox_set_checked(cb, rec.Fri);
        break;
      case 6:
        lv_obj_set_style_local_text_color(cb, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_RED);
        lv_checkbox_set_checked(cb, rec.Sat);
#endif
        break;
      default:
        break;
    }
    lv_checkbox_set_text_static(cb, options[i]);
  }
  cont2 = lv_cont_create(lv_scr_act(), NULL);
  lv_obj_set_pos(cont2, LV_HOR_RES / 2 - 20, 0);
  lv_obj_set_size(cont2, LV_HOR_RES / 2 + 20, LV_VER_RES);
  lv_obj_set_style_local_bg_color(cont2, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_set_style_local_pad_all(cont2, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 4);
  lv_obj_set_style_local_pad_inner(cont2, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 10);
  lv_obj_set_style_local_border_width(cont2, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 1);
  lv_cont_set_layout(cont2, LV_LAYOUT_COLUMN_MID);
  cont2->user_data = this;

  cbOnce = lv_checkbox_create(cont2, NULL);
  lv_checkbox_set_text_static(cbOnce, options[i]);
  lv_checkbox_set_checked(cbOnce, rec.Once);

  lv_obj_t* btn = lv_btn_create(cont2, NULL);
  lv_obj_t* o = lv_label_create(btn, NULL);
  // lv_obj_set_style_local_bg_color(btnDaily, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_NAVY);
  lv_label_set_text_static(o, "Invert");
  lv_obj_set_event_cb(btn, [](lv_obj_t* obj, lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
      (static_cast<AlarmSet*>(obj->parent->user_data))->invertSeledtedDays();
    }
  });

  btn = lv_btn_create(cont2, NULL);
  lv_obj_set_event_cb(btn, [](lv_obj_t* obj, lv_event_t event) {
    if (event == LV_EVENT_CLICKED)
      (static_cast<AlarmSet*>(obj->parent->user_data))->saveRecurences();
  });
  lv_obj_set_size(btn, 80, 40);
  o = lv_label_create(btn, NULL);
  lv_obj_set_style_local_bg_color(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GREEN);
  lv_label_set_text_static(o, "OK");
}

void AlarmSet::invertSeledtedDays() {
  lv_obj_t* cb = NULL;
  while ((cb = lv_obj_get_child(cont1, cb))) {
    lv_checkbox_set_checked(cb, !lv_checkbox_is_checked(cb));
  }
}

void AlarmSet::saveRecurences() {
  AlarmController::RecurType rec;
  uint8_t i = 0;
  lv_obj_t* cb = NULL;
  while ((cb = lv_obj_get_child(cont1, cb))) {
    bool checked = lv_checkbox_is_checked(cb);
    switch (i++) {
#ifdef WeekStartsMonday
      case 6:
        rec.Mon = checked;
        break;
      case 5:
        rec.Tue = checked;
        break;
      case 4:
        rec.Wed = checked;
        break;
      case 3:
        rec.Thu = checked;
        break;
      case 2:
        rec.Fri = checked;
        break;
      case 1:
        rec.Sat = checked;
        break;
      case 0:
        rec.Sun = checked;
#else
      case 6:
        rec.Sun = checked;
        break;
      case 5:
        rec.Mon = checked;
        break;
      case 4:
        rec.Tue = checked;
        break;
      case 3:
        rec.Wed = checked;
        break;
      case 2:
        rec.Thu = checked;
        break;
      case 1:
        rec.Fri = checked;
        break;
      case 0:
        rec.Sat = checked;
#endif
        break;
      default:
        break;
    }
  }
  rec.Once = lv_checkbox_is_checked(cbOnce);
  lv_obj_del(cont1);
  lv_obj_del(cont2);
  alarmController->SetRecurrence(rec);
  updateRecurencesButton();
}

void AlarmSet::updateRecurencesButton() {
  std::string str;
  auto rec = alarmController->Recurrence();
  if (rec.Mon)
    str.append("Mo ");
  if (rec.Tue)
    str.append("Tu ");
  if (rec.Wed)
    str.append("We ");
  if (rec.Thu)
    str.append("Th ");
  if (rec.Fri)
    str.append("Fr ");
  if (rec.Sat)
    str.append("#FFFF00 Sa# ");
  if (rec.Sun)
    str.append("#FFFF00 Su");
  lv_label_set_text(txtRecur, str.c_str());
}

bool AlarmSet::OnAlertingState() {
  return alarmController->State() == AlarmController::AlarmState::Alerting;
}

/*
void AlarmSet::DisableAlarm() {
  if (alarmController->State() == AlarmController::AlarmState::Set) {
    alarmController->DisableAlarm();
  }
}
*/