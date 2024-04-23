#include "Steps.h"
#include "components/motion/MotionController.h"
#include "systemtask/SystemTask.h"
#include "displayapp/InfiniTimeTheme.h"

using namespace Pinetime::Applications::Screens;

Steps::Steps() : Screen(Apps::Steps) {
}

void Steps::Load() {
  stepsArc = lv_arc_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_bg_opa(stepsArc, LV_ARC_PART_BG, LV_STATE_DEFAULT, LV_OPA_0);
  lv_obj_set_style_local_line_color(stepsArc, LV_ARC_PART_BG, LV_STATE_DEFAULT, Colors::bgAlt);
  lv_obj_set_style_local_border_width(stepsArc, LV_ARC_PART_BG, LV_STATE_DEFAULT, 2);
  lv_obj_set_style_local_radius(stepsArc, LV_ARC_PART_BG, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_line_color(stepsArc, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, Colors::blue);
  lv_arc_set_end_angle(stepsArc, 200);
  lv_obj_set_size(stepsArc, 240, 240);
  lv_arc_set_range(stepsArc, 0, 500);
  lv_obj_align(stepsArc, NULL, LV_ALIGN_CENTER, 0, 0);

  lSteps = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(lSteps, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_LIME);
  lv_obj_set_style_local_text_font(lSteps, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_42);

  lv_obj_t* lstepsL = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(lstepsL, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::lightGray);
  lv_label_set_text_static(lstepsL, "Steps");

  lv_obj_t* lstepsGoal = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(lstepsGoal, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_CYAN);
  lv_label_set_text_fmt(lstepsGoal, "Goal: %5lu", System::SystemTask::displayApp->settingsController.GetStepsGoal());
  lv_label_set_align(lstepsGoal, LV_LABEL_ALIGN_CENTER);

  resetBtn = lv_btn_create(lv_scr_act(), NULL);
  resetBtn->user_data = this;
  lv_obj_set_event_cb(resetBtn, lap_event_handler);
  lv_obj_set_size(resetBtn, 120, 50);
  lv_obj_set_style_local_radius(resetBtn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
  lv_obj_set_style_local_bg_color(resetBtn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, Colors::bgAlt);
  lv_obj_align(resetBtn, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, 0);
  resetButtonLabel = lv_label_create(resetBtn, NULL);
  lv_label_set_text_static(resetButtonLabel, "Reset");

  tripLabel = lv_label_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_text_color(tripLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_YELLOW);

  Refresh();

  lv_obj_align(lSteps, NULL, LV_ALIGN_CENTER, 0, -40);
  lv_obj_align(lstepsL, lSteps, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
  lv_obj_align(lstepsGoal, lSteps, LV_ALIGN_OUT_BOTTOM_MID, 0, 40);
#ifdef UnitFormat_Metric
  lv_obj_align(tripLabel, lstepsGoal, LV_ALIGN_IN_LEFT_MID, -14, 20);
#else
  lv_obj_align(tripLabel, lstepsGoal, LV_ALIGN_IN_LEFT_MID, -20, 20);
#endif
  taskRefresh = lv_task_create(RefreshTaskCallback, 100, LV_TASK_PRIO_LOW, this);
  running = true;
}

bool Steps::UnLoad() {
  if (running) {
    lv_task_del(taskRefresh);
    lv_obj_clean(lv_scr_act());
    running = false;
  }
  return true;
}

Steps::~Steps() {
  UnLoad();
}

void Steps::Refresh() {
  uint32_t stepsCount = System::SystemTask::displayApp->motionController.NbSteps();
  lv_label_set_text_fmt(lSteps, "%li", stepsCount);
  lv_obj_align(lSteps, NULL, LV_ALIGN_CENTER, 0, -40);
  lv_label_set_text_fmt(tripLabel,
#ifdef UnitFormat_Metric
                        "Trip: %5li m",
                        System::SystemTask::displayApp->motionController.GetTripSteps() *
                          System::SystemTask::displayApp->settingsController.GetStepLength() / 100);
#else
                        "Trip: %5li ft",
                        System::SystemTask::displayApp->motionController.GetTripSteps() *
                          System::SystemTask::displayApp->settingsController.GetStepLength() / 12);
#endif
  lv_arc_set_value(stepsArc, int16_t(500 * stepsCount / System::SystemTask::displayApp->settingsController.GetStepsGoal()));
}

void Steps::lapBtnClicked() {
  System::SystemTask::displayApp->motionController.ResetTrip();
  Refresh();
}

void Steps::lap_event_handler(lv_obj_t* obj, lv_event_t event) {
  if (event == LV_EVENT_CLICKED) {
    static_cast<Steps*>(obj->user_data)->lapBtnClicked();
  }
}

bool Steps::OnTouchEvent(TouchEvents event) {
  if (event == TouchEvents::LongTap || event == TouchEvents::SwipeUp) {
    System::SystemTask::displayApp->StartApp(Apps::SettingSteps);
    return true;
  }
  return false;
}
