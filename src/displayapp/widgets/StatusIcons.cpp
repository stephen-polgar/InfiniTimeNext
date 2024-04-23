#include "displayapp/widgets/StatusIcons.h"
#include "displayapp/screens/Symbols.h"
#include "components/battery/BatteryController.h"
#include "components/ble/BleController.h"
#include "systemtask/SystemTask.h"

using namespace Pinetime::Applications::Widgets;

StatusIcons::StatusIcons() : batteryIcon(true) {
}

void StatusIcons::Load() {
  Container = lv_cont_create(lv_scr_act(), nullptr);
  lv_cont_set_layout(Container, LV_LAYOUT_ROW_TOP);
  lv_cont_set_fit(Container, LV_FIT_TIGHT);
  lv_obj_set_style_local_pad_inner(Container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 5);
  lv_obj_set_style_local_bg_opa(Container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);

  bleIcon = lv_label_create(Container, nullptr);
  lv_label_set_text_static(bleIcon, Screens::Symbols::bluetooth);

  batteryPlug = lv_label_create(Container, nullptr);
  lv_label_set_text_static(batteryPlug, Screens::Symbols::plug);

  batteryIcon.Create(Container);

  lv_obj_align(Container, nullptr, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
  update = true;
}

void StatusIcons::Refresh() {
  auto * app = System::SystemTask::displayApp;
  powerPresent = app->batteryController.IsPowerPresent();
  if (update || powerPresent.IsUpdated()) {
    lv_obj_set_hidden(batteryPlug, !powerPresent.Get());
  }

  batteryPercentRemaining = app->batteryController.PercentRemaining();
  if (update || batteryPercentRemaining.IsUpdated()) {
    auto batteryPercent = batteryPercentRemaining.Get();
    batteryIcon.SetBatteryPercentage(batteryPercent);
  }

  bleState = app->bleController.IsConnected();
  bleRadioEnabled = app->bleController.IsRadioEnabled();
  if (update || bleState.IsUpdated() || bleRadioEnabled.IsUpdated()) {
    lv_obj_set_hidden(bleIcon, !bleState.Get());
  }

  lv_obj_realign(Container);
  update = false;
}
