#include "TimerSet.h"
#include "Symbols.h"
#include "displayapp/InfiniTimeTheme.h"

// #define Log

#ifdef Log
  #include <libraries/log/nrf_log.h>
#endif

using namespace Pinetime::Applications::Screens;

TimerSet::TimerSet(Controllers::TimerController* t) : timer {t}, ScreenRefresh(Apps::TimerSet) {
  if (!t)
    timer = new Controllers::TimerController();
}

TimerSet::TimerSet() : ScreenRefresh(Apps::TimerSet) {
  TimerDone();
}

void TimerSet::setTimer(Controllers::TimerController* timer) {
  if (this->timer) {
    this->timer = timer;
    reset();
  } else
    this->timer = timer;
}

void TimerSet::TimerDone() {
  for (auto* timerController : Controllers::TimerController::timers) {
    if (timerController->done) {
      timerController->done = false;
      setTimer(timerController);
      break;
    }
  }
}

void TimerSet::Load() {
#ifdef Log
  NRF_LOG_INFO("TimerSet::Load=%d", this);
#endif
  running = true;
  lv_obj_t* colonLabel = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_font(colonLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_76);
  lv_obj_set_style_local_text_color(colonLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_label_set_text_static(colonLabel, ":");
  lv_obj_align(colonLabel, lv_scr_act(), LV_ALIGN_CENTER, 0, -29);

  minuteCounter.Create();
  secondCounter.Create();
  lv_obj_align(minuteCounter.GetObject(), NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
  lv_obj_align(secondCounter.GetObject(), NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);

  highlightObjectMask = lv_objmask_create(lv_scr_act(), NULL);
  lv_obj_set_size(highlightObjectMask, 240, 50);
  lv_obj_align(highlightObjectMask, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, 0);

  lv_draw_mask_line_param_t tmpMaskLine;

  lv_draw_mask_line_points_init(&tmpMaskLine, 0, 0, 0, 240, LV_DRAW_MASK_LINE_SIDE_LEFT);
  highlightMask = lv_objmask_add_mask(highlightObjectMask, &tmpMaskLine);

  lv_obj_t* btnHighlight = lv_obj_create(highlightObjectMask, NULL);
  lv_obj_set_style_local_radius(btnHighlight, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
  lv_obj_set_style_local_bg_color(btnHighlight, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
  lv_obj_set_size(btnHighlight, LV_HOR_RES, 50);
  lv_obj_align(btnHighlight, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, 0);

  btnObjectMask = lv_objmask_create(lv_scr_act(), NULL);
  lv_obj_set_size(btnObjectMask, 240, 50);
  lv_obj_align(btnObjectMask, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, 0);

  lv_draw_mask_line_points_init(&tmpMaskLine, 0, 0, 0, 240, LV_DRAW_MASK_LINE_SIDE_RIGHT);
  btnMask = lv_objmask_add_mask(btnObjectMask, &tmpMaskLine);

  btnPlayPause = lv_btn_create(btnObjectMask, NULL);
  btnPlayPause->user_data = this;
  lv_obj_set_style_local_radius(btnPlayPause, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
  lv_obj_set_style_local_bg_color(btnPlayPause, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, Colors::bgAlt);
  lv_obj_set_event_cb(btnPlayPause, btnEventHandler);
  lv_obj_set_size(btnPlayPause, LV_HOR_RES, 50);

  txtPlayPause = lv_label_create(lv_scr_act(), NULL);
  lv_obj_align(txtPlayPause, btnPlayPause, LV_ALIGN_CENTER, 0, 0);

  if (timer->IsRunning()) {
    setTimerRunning();
  } else {
    setTimerStopped();
    updateTime(timer->duration);
  }
  createRefreshTask(LV_DISP_DEF_REFR_PERIOD, LV_TASK_PRIO_MID);
}

void TimerSet::buttonPressed() {
  pressTime = xTaskGetTickCount();
  buttonPressing = true;
}

void TimerSet::maskReset() {
  buttonPressing = false;
  // A click event is processed before a release event,
  // so the release event would override the "Pause" text without this check
  if (!timer->IsRunning()) {
    lv_label_set_text_static(txtPlayPause, "Start");
  }
  maskPosition = 0;
  updateMask();
}

void TimerSet::updateMask() {
  lv_draw_mask_line_param_t maskLine;

  lv_draw_mask_line_points_init(&maskLine, maskPosition, 0, maskPosition, 240, LV_DRAW_MASK_LINE_SIDE_LEFT);
  lv_objmask_update_mask(highlightObjectMask, highlightMask, &maskLine);

  lv_draw_mask_line_points_init(&maskLine, maskPosition, 0, maskPosition, 240, LV_DRAW_MASK_LINE_SIDE_RIGHT);
  lv_objmask_update_mask(btnObjectMask, btnMask, &maskLine);
}


 void TimerSet::btnEventHandler(lv_obj_t* obj, lv_event_t event) {
  auto* screen = static_cast<TimerSet*>(obj->user_data);
  if (event == LV_EVENT_PRESSED) {
    screen->buttonPressed();
  } else if (event == LV_EVENT_RELEASED || event == LV_EVENT_PRESS_LOST) {
    screen->maskReset();
  } else if (event == LV_EVENT_SHORT_CLICKED) {
    screen->toggleRunning();
  }
}


void TimerSet::updateTime(std::chrono::milliseconds time) {
  auto seconds = std::chrono::duration_cast<std::chrono::seconds>(time);
  minuteCounter.SetValue(seconds.count() / 60);
  secondCounter.SetValue(seconds.count() % 60);
}

void TimerSet::Refresh() {
  if (timer->IsRunning())
    updateTime(timer->GetTimeRemaining());
  else if (buttonPressing && xTaskGetTickCount() > pressTime + pdMS_TO_TICKS(150)) {
    lv_label_set_text_static(txtPlayPause, "Reset");
    maskPosition += 15;
    if (maskPosition > 240) {
      maskReset();
      reset();
    } else {
      updateMask();
    }
  }
}

void TimerSet::setTimerRunning() {
  minuteCounter.HideControls();
  secondCounter.HideControls();
  lv_label_set_text_static(txtPlayPause, "Pause");
}

void TimerSet::setTimerStopped() {
  minuteCounter.ShowControls();
  secondCounter.ShowControls();
  lv_label_set_text_static(txtPlayPause, "Start");
}

void TimerSet::toggleRunning() {
  if (timer->IsRunning()) {
    updateTime(timer->GetTimeRemaining());
    timer->StopTimer();
    setTimerStopped();
  } else if (secondCounter.GetValue() + minuteCounter.GetValue() > 0) {
    if (timer->duration == (std::chrono::milliseconds) 0) {
      timer->duration = std::chrono::minutes(minuteCounter.GetValue()) + std::chrono::seconds(secondCounter.GetValue());
      Controllers::TimerController::Save();
      timer->StartTimer(timer->duration);
    } else
      timer->StartTimer(std::chrono::minutes(minuteCounter.GetValue()) + std::chrono::seconds(secondCounter.GetValue()));
    Refresh();
    setTimerRunning();
  }
}

void TimerSet::reset() {
  updateTime(timer->duration);
  setTimerStopped();
}
