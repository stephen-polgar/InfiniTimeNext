#include "Timer.h"
#include "TimerSet.h"
#include "systemtask/SystemTask.h"
#include "displayapp/InfiniTimeTheme.h"

// #define Log

#ifdef Log
  #include <libraries/log/nrf_log.h>
#endif

using namespace Pinetime::Applications::Screens;

bool Timer::changed = false;

Timer::Timer() : Screen(Apps::Timer) {
#ifdef Log
  NRF_LOG_INFO("Timer::Timer=%d", this);
#endif
}

void Timer::Load() {
#ifdef Log
  NRF_LOG_INFO("Timer::Load=%d", this);
#endif
  running = true;
  for (auto* timerController : Controllers::TimerController::timers) {
    createElem(timerController);
  }

  btnNew = lv_btn_create(lv_scr_act(), NULL);
  lv_obj_align(btnNew, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, 0);
  lv_obj_t* label = lv_label_create(btnNew, NULL);
  btnNew->user_data = this;
  lv_obj_set_event_cb(btnNew, [](lv_obj_t* obj, lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
      Controllers::TimerController* timerController = new Controllers::TimerController();
      System::SystemTask::displayApp->StartApp(new TimerSet(timerController));
      (static_cast<Timer*>(obj->user_data))->createElem(timerController);
    }
  });
  lv_label_set_text_static(label, "+");
  // lv_obj_set_style_local_bg_color(btnMessage, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_NAVY);
  updateAddButton();
}

bool Timer::UnLoad() {
#ifdef Log
  NRF_LOG_INFO("Timer::UnLoad()=%d", this);
#endif
  if (running) {
    lv_obj_clean(lv_scr_act());
    last = NULL;
    running = false;
  }
  return true;
}

Timer::~Timer() {
  UnLoad();
  if (Timer::changed) {
    Timer::changed = false;
    Controllers::TimerController::Save();
  }
}

void Timer::updateAddButton() {
  lv_btn_set_state(btnNew,
                   Controllers::TimerController::timers.size() >= Controllers::TimerController::MaxElements ? LV_BTN_STATE_DISABLED
                                                                                                            : LV_BTN_STATE_RELEASED);
}

void Timer::createElem(Controllers::TimerController* timerController) {
  lv_obj_t* cont = lv_cont_create(lv_scr_act(), NULL);
  cont->user_data = timerController;

  lv_obj_set_style_local_bg_opa(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
  lv_obj_set_style_local_pad_all(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 3);
  lv_obj_set_style_local_pad_inner(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 8);
  lv_obj_set_style_local_border_width(cont, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 1);

  lv_cont_set_fit(cont, LV_FIT_TIGHT);
  lv_cont_set_layout(cont, LV_LAYOUT_ROW_MID);
  const lv_coord_t size = 40;

  lv_obj_t* cb = lv_checkbox_create(cont, NULL);
  // SetRadioButtonStyle(cb);
  lv_checkbox_set_text_static(cb, "");
  lv_obj_set_size(cb, size, size);

  lv_checkbox_set_checked(cb, timerController->IsRunning());
  lv_obj_set_event_cb(cb, [](lv_obj_t* cb, lv_event_t event) {
    if (event == LV_EVENT_VALUE_CHANGED) {
      Controllers::TimerController* timerController = static_cast<Controllers::TimerController*>(cb->parent->user_data);
      if (lv_checkbox_is_checked(cb))
        timerController->StartTimer(timerController->duration);
      else
        timerController->StopTimer();
    }
  });

  lv_obj_t* btn = lv_btn_create(cont, NULL);
  // lv_obj_set_size(btn, 100, size);
  lv_obj_t* label = lv_label_create(btn, NULL);
  lv_obj_set_style_local_bg_color(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, Colors::bgAlt);
  auto secondsRemaining = std::chrono::duration_cast<std::chrono::seconds>(timerController->duration);
  lv_label_set_text_fmt(label, "%.2d:%.2d", secondsRemaining.count() / 60, secondsRemaining.count() % 60);
  lv_obj_set_event_cb(btn, [](lv_obj_t* obj, lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {      
      System::SystemTask::displayApp->StartApp(new TimerSet(static_cast<Controllers::TimerController*>(obj->parent->user_data)));
    }
  });

  btn = lv_btn_create(cont, NULL);
  lv_obj_set_size(btn, size, size);
  label = lv_label_create(btn, NULL);
  lv_label_set_text_static(label, "X");
  // lv_obj_set_style_local_bg_color(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, Colors::bgAlt);
  lv_obj_set_style_local_border_width(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, 4);
  lv_obj_set_style_local_border_color(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_set_style_local_pad_left(btn, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 8);
  btn->user_data = this;
  lv_obj_set_event_cb(btn, [](lv_obj_t* obj, lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
      delete static_cast<Controllers::TimerController*>(obj->parent->user_data);
      lv_obj_clean(obj->parent);
      (static_cast<Timer*>(obj->user_data))->updateAddButton();
      Timer::changed = true;
    }
  });

  // lv_obj_set_style_local_bg_color(label, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_NAVY);
  if (last)
    lv_obj_align(cont, last, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
  else
    lv_obj_align(cont, lv_scr_act(), LV_ALIGN_IN_TOP_MID, 0, 0);
  last = cont;
}
