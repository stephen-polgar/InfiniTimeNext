
#include <FreeRTOS.h>
#include "StopWatch.h"
#include "systemtask/SystemTask.h"
#include "displayapp/InfiniTimeTheme.h"

using namespace Pinetime::Applications::Screens;

States StopWatch::currentState = States::Init;
TickType_t StopWatch::oldTimeElapsed;
TickType_t StopWatch::blinkTime;
int StopWatch::lapsDone;
TickType_t StopWatch::startTime;
TickType_t StopWatch::laps[maxLapCount + 1];
TimeSeparated_t StopWatch::currentTimeSeparated;

constexpr TickType_t blinkInterval = pdMS_TO_TICKS(1000);

StopWatch::StopWatch() : ScreenRefresh(Apps::StopWatch) {
}

void StopWatch::Load() {
  if (currentState == States::Init) {
    oldTimeElapsed = 0;
    blinkTime = 0;
    lapsDone = 0;
  }
  isHoursLabelUpdated = false;

  static constexpr uint8_t btnWidth = 115;
  static constexpr uint8_t btnHeight = 80;
  btnPlayPause = lv_btn_create(lv_scr_act(), NULL);
  btnPlayPause->user_data = this;
  lv_obj_set_event_cb(btnPlayPause, play_pause_event_handler);
  lv_obj_set_size(btnPlayPause, btnWidth, btnHeight);
  lv_obj_align(btnPlayPause, lv_scr_act(), LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
  txtPlayPause = lv_label_create(btnPlayPause, NULL);

  btnStopLap = lv_btn_create(lv_scr_act(), NULL);
  btnStopLap->user_data = this;
  lv_obj_set_event_cb(btnStopLap, stop_lap_event_handler);
  lv_obj_set_size(btnStopLap, btnWidth, btnHeight);
  lv_obj_align(btnStopLap, lv_scr_act(), LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
  txtStopLap = lv_label_create(btnStopLap, NULL);
  if (currentState != States::Halted)
    lv_obj_set_state(btnStopLap, LV_STATE_DISABLED);
  lv_obj_set_state(txtStopLap, LV_STATE_DISABLED);

  lapText = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(lapText, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::lightGray);
  lv_label_set_text_static(lapText, "\n");
  lv_label_set_long_mode(lapText, LV_LABEL_LONG_BREAK);
  lv_label_set_align(lapText, LV_LABEL_ALIGN_CENTER);
  lv_obj_set_width(lapText, LV_HOR_RES_MAX);
  lv_obj_align(lapText, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, -btnHeight);

  msecTime = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(msecTime, LV_LABEL_PART_MAIN, LV_STATE_DISABLED, Colors::lightGray);
  lv_obj_align(msecTime, lapText, LV_ALIGN_OUT_TOP_MID, 0, 0);

  time = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_font(time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_76);
  lv_label_set_text_static(time, "00:00");
  lv_obj_set_style_local_text_color(time, LV_LABEL_PART_MAIN, LV_STATE_DISABLED, Colors::lightGray);
  lv_obj_align(time, msecTime, LV_ALIGN_OUT_TOP_MID, 0, 0);
  if (currentState == States::Running) {
    SetInterfaceRunning();
    Refresh();
  } else if (currentState == States::Init)
    SetInterfaceStopped();
  else {
    SetInterfacePaused();
    refresh(currentTimeSeparated);
  }
  createRefreshTask(LV_DISP_DEF_REFR_PERIOD, LV_TASK_PRIO_MID);
  running = true;
}

bool StopWatch::UnLoad() {
  System::SystemTask::displayApp->systemTask->PushMessage(System::Messages::EnableSleeping);
  return ScreenRefresh::UnLoad();
}

StopWatch::~StopWatch() {
  UnLoad();
}

void StopWatch::SetInterfacePaused() {
  lv_obj_set_style_local_bg_color(btnStopLap, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_RED);
  lv_obj_set_style_local_bg_color(btnPlayPause, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, Colors::blue);
  lv_label_set_text_static(txtPlayPause, Symbols::play);
  lv_label_set_text_static(txtStopLap, Symbols::stop);
}

void StopWatch::SetInterfaceRunning() {
  lv_obj_set_state(time, LV_STATE_DEFAULT);
  lv_obj_set_state(msecTime, LV_STATE_DEFAULT);
  lv_obj_set_style_local_bg_color(btnPlayPause, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, Colors::bgAlt);
  lv_obj_set_style_local_bg_color(btnStopLap, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, Colors::bgAlt);

  lv_label_set_text_static(txtPlayPause, Symbols::pause);
  lv_label_set_text_static(txtStopLap, Symbols::lapsFlag);

  lv_obj_set_state(btnStopLap, LV_STATE_DEFAULT);
  lv_obj_set_state(txtStopLap, LV_STATE_DEFAULT);
}

void StopWatch::SetInterfaceStopped() {
  lv_obj_set_state(time, LV_STATE_DISABLED);
  lv_obj_set_state(msecTime, LV_STATE_DISABLED);
  lv_obj_set_style_local_bg_color(btnPlayPause, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, Colors::blue);

  lv_label_set_text_static(time, "00:00");
  lv_label_set_text_static(msecTime, "00");

  if (isHoursLabelUpdated) {
    lv_obj_set_style_local_text_font(time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_76);
    lv_obj_realign(time);
    isHoursLabelUpdated = false;
  }

  lv_label_set_text_static(lapText, "");
  lv_label_set_text_static(txtPlayPause, Symbols::play);
  lv_label_set_text_static(txtStopLap, Symbols::lapsFlag);
  lv_obj_set_state(btnStopLap, LV_STATE_DISABLED);
  lv_obj_set_state(txtStopLap, LV_STATE_DISABLED);
}

void StopWatch::Reset() {
  SetInterfaceStopped();
  currentState = States::Init;
  oldTimeElapsed = 0;
  lapsDone = 0;
}

void StopWatch::Start() {
  SetInterfaceRunning();
  startTime = xTaskGetTickCount();
  currentState = States::Running;
  System::SystemTask::displayApp->systemTask->PushMessage(System::Messages::DisableSleeping);
}

void StopWatch::Pause() {
  SetInterfacePaused();
  startTime = 0;
  // Store the current time elapsed in cache
  oldTimeElapsed = laps[lapsDone];
  blinkTime = xTaskGetTickCount() + blinkInterval;
  currentState = States::Halted;
  System::SystemTask::displayApp->systemTask->PushMessage(System::Messages::EnableSleeping);
}

void StopWatch::refresh(TimeSeparated_t& currentTimeSeparated) {
  if (currentTimeSeparated.hours == 0) {
    lv_label_set_text_fmt(time, "%02d:%02d", currentTimeSeparated.mins, currentTimeSeparated.secs);
  } else {
    lv_label_set_text_fmt(time, "%02d:%02d:%02d", currentTimeSeparated.hours, currentTimeSeparated.mins, currentTimeSeparated.secs);
    if (!isHoursLabelUpdated) {
      lv_obj_set_style_local_text_font(time, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_42);
      lv_obj_realign(time);
      isHoursLabelUpdated = true;
    }
  }
  lv_label_set_text_fmt(msecTime, "%02d", currentTimeSeparated.hundredths);
}

void StopWatch::Refresh() {
  if (currentState == States::Running) {
    laps[lapsDone] = oldTimeElapsed + xTaskGetTickCount() - startTime;
    currentTimeSeparated = convertTicksToTimeSegments(laps[lapsDone]);
    refresh(currentTimeSeparated);
  } else if (currentState == States::Halted) {
    const TickType_t currentTime = xTaskGetTickCount();
    if (currentTime > blinkTime) {
      blinkTime = currentTime + blinkInterval;
      if (lv_obj_get_state(time, LV_LABEL_PART_MAIN) == LV_STATE_DEFAULT) {
        lv_obj_set_state(time, LV_STATE_DISABLED);
        lv_obj_set_state(msecTime, LV_STATE_DISABLED);
      } else {
        lv_obj_set_state(time, LV_STATE_DEFAULT);
        lv_obj_set_state(msecTime, LV_STATE_DEFAULT);
      }
    }
  }
}

void StopWatch::playPauseBtnEventHandler() {
  if (currentState == States::Init || currentState == States::Halted) {
    Start();
  } else if (currentState == States::Running) {
    Pause();
  }
}

void StopWatch::stopLapBtnEventHandler() {
  // If running, then this button is used to save laps
  if (currentState == States::Running) {
    lv_label_set_text(lapText, "");
    lapsDone = std::min(lapsDone + 1, maxLapCount);
    for (int i = lapsDone - displayedLaps; i < lapsDone; i++) {
      if (i < 0) {
        lv_label_ins_text(lapText, LV_LABEL_POS_LAST, "\n");
        continue;
      }
      TimeSeparated_t times = convertTicksToTimeSegments(laps[i]);
      char buffer[17];
      if (times.hours == 0) {
        snprintf(buffer, sizeof(buffer), "#%2d    %2d:%02d.%02d\n", i + 1, times.mins, times.secs, times.hundredths);
      } else {
        snprintf(buffer, sizeof(buffer), "#%2d %2d:%02d:%02d.%02d\n", i + 1, times.hours, times.mins, times.secs, times.hundredths);
      }
      lv_label_ins_text(lapText, LV_LABEL_POS_LAST, buffer);
    }
  } else if (currentState == States::Halted) {
    Reset();
  }
}

TimeSeparated_t StopWatch::convertTicksToTimeSegments(const TickType_t timeElapsed) {
  // Centiseconds
  const int timeElapsedCentis = timeElapsed * 100 / configTICK_RATE_HZ;

  const int hundredths = (timeElapsedCentis % 100);
  const int secs = (timeElapsedCentis / 100) % 60;
  const int mins = ((timeElapsedCentis / 100) / 60) % 60;
  const int hours = ((timeElapsedCentis / 100) / 60) / 60;
  return TimeSeparated_t {hours, mins, secs, hundredths};
}

void StopWatch::play_pause_event_handler(lv_obj_t* obj, lv_event_t event) {
  if (event == LV_EVENT_CLICKED) {
    static_cast<StopWatch*>(obj->user_data)->playPauseBtnEventHandler();
  }
}

void StopWatch::stop_lap_event_handler(lv_obj_t* obj, lv_event_t event) {
  if (event == LV_EVENT_CLICKED) {
    static_cast<StopWatch*>(obj->user_data)->stopLapBtnEventHandler();
  }
}

bool StopWatch::OnButtonPushed() {
  if (currentState == States::Running) {
    Pause();
    return true;
  }
  return false;
}
