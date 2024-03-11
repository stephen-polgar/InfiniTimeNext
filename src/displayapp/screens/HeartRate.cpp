#include "HeartRate.h"
#include "systemtask/SystemTask.h"
#include "displayapp/InfiniTimeTheme.h"

using namespace Pinetime::Applications::Screens;
using namespace Pinetime::Controllers;

HeartRate::HeartRate() : Screen(Apps::HeartRate) {
}

void HeartRate::Load() {
  running = true;
  bool isHrRunning = System::SystemTask::displayApp->heartRateController.State() != HeartRateController::States::Stopped;
  label_hr = lv_label_create(lv_scr_act(), NULL);

  lv_obj_set_style_local_text_font(label_hr, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_76);

  if (isHrRunning) {
    lv_obj_set_style_local_text_color(label_hr, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::highlight);
  } else {
    lv_obj_set_style_local_text_color(label_hr, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::lightGray);
  }

  lv_label_set_text_static(label_hr, "000");
  lv_obj_align(label_hr, NULL, LV_ALIGN_CENTER, 0, -40);

  label_bpm = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text_static(label_bpm, "Heart rate BPM");
  lv_obj_align(label_bpm, label_hr, LV_ALIGN_OUT_TOP_MID, 0, -20);

  label_status = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(label_status, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
  lv_label_set_text_static(label_status, toString(HeartRateController::States::NotEnoughData));

  lv_obj_align(label_status, label_hr, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

  btn_startStop = lv_btn_create(lv_scr_act(), NULL);
  btn_startStop->user_data = this;
  lv_obj_set_height(btn_startStop, 50);
  lv_obj_set_event_cb(btn_startStop, btnStartStopEventHandler);
  lv_obj_align(btn_startStop, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);

  label_startStop = lv_label_create(btn_startStop, NULL);
  UpdateStartStopButton(isHrRunning);
  if (isHrRunning) {
    System::SystemTask::displayApp->systemTask->PushMessage(System::Messages::DisableSleeping);
  }
  taskRefresh = lv_task_create(RefreshTaskCallback, 100, LV_TASK_PRIO_MID, this);
}

bool HeartRate::UnLoad() {
  if (running) {
    lv_task_del(taskRefresh);
    lv_obj_clean(lv_scr_act());
    System::SystemTask::displayApp->systemTask->PushMessage(System::Messages::EnableSleeping);
    running = false;
  }
  return true;
}

HeartRate::~HeartRate() {
  UnLoad();
}


const char* HeartRate::toString(HeartRateController::States s) {
  switch (s) {
    case HeartRateController::States::NotEnoughData:
      return "Not enough data,\nplease wait...";
    case HeartRateController::States::NoTouch:
      return "No touch detected";
    case HeartRateController::States::Running:
      return "Measuring...";
    case HeartRateController::States::Stopped:
      return "Stopped";
  }
  return "";
}


void HeartRate::Refresh() {
  auto state = System::SystemTask::displayApp->heartRateController.State();
  switch (state) {
    case HeartRateController::States::NoTouch:
    case HeartRateController::States::NotEnoughData:
      // case HeartRateController::States::Stopped:
      lv_label_set_text_static(label_hr, "000");
      break;
    default:
      lv_label_set_text_fmt(label_hr, "%03d", System::SystemTask::displayApp->heartRateController.HeartRate());
  }

  lv_label_set_text_static(label_status, toString(state));
  lv_obj_align(label_status, label_hr, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
}

void HeartRate::onStartStopEvent() {
  auto* hr = &System::SystemTask::displayApp->heartRateController;
  if (hr->State() == HeartRateController::States::Stopped) {
    hr->Start();
    UpdateStartStopButton(hr->State() != HeartRateController::States::Stopped);
    System::SystemTask::displayApp->systemTask->PushMessage(System::Messages::DisableSleeping);
    lv_obj_set_style_local_text_color(label_hr, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::highlight);
  } else {
    hr->Stop();
    UpdateStartStopButton(hr->State() != HeartRateController::States::Stopped);
    System::SystemTask::displayApp->systemTask->PushMessage(System::Messages::EnableSleeping);
    lv_obj_set_style_local_text_color(label_hr, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::lightGray);
  }
}

void HeartRate::btnStartStopEventHandler(lv_obj_t* obj, lv_event_t event) {
  if (event == LV_EVENT_CLICKED)
    static_cast<HeartRate*>(obj->user_data)->onStartStopEvent();
}

void HeartRate::UpdateStartStopButton(bool isRunning) {
  if (isRunning) {
    lv_label_set_text_static(label_startStop, "Stop");
  } else {
    lv_label_set_text_static(label_startStop, "Start");
  }
}
