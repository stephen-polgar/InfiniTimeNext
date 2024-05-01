#include "QuickSettings.h"
#include "systemtask/SystemTask.h"
#include "displayapp/screens/Symbols.h"
#include "displayapp/InfiniTimeTheme.h"

using namespace Pinetime::Applications::Screens;

QuickSettings::QuickSettings() : Screen(Apps::QuickSettings) {
}

void QuickSettings::Load() {
  // This is the distance (padding) between all objects on this screen.
  static constexpr uint8_t innerDistance = 10;
  static constexpr uint8_t buttonHeight = (LV_VER_RES_MAX - innerDistance * 2) / 2;
  static constexpr uint8_t buttonWidth = (LV_HOR_RES_MAX - innerDistance) / 2; // wide buttons
  static constexpr uint8_t buttonXOffset = (LV_HOR_RES_MAX - buttonWidth * 2 - innerDistance) / 2;

  lv_style_init(&btn_style);
  lv_style_set_radius(&btn_style, LV_STATE_DEFAULT, buttonHeight / 4);
  lv_style_set_bg_color(&btn_style, LV_STATE_DEFAULT, Colors::bgAlt);

  btn1 = lv_btn_create(lv_scr_act(), nullptr);
  btn1->user_data = this;
  lv_obj_set_event_cb(btn1, buttonEventHandler);
  lv_obj_add_style(btn1, LV_BTN_PART_MAIN, &btn_style);
  lv_obj_set_size(btn1, buttonWidth, buttonHeight);
  lv_obj_align(btn1, nullptr, LV_ALIGN_IN_TOP_LEFT, buttonXOffset, innerDistance);

  btn1_lvl = lv_label_create(btn1, nullptr);
  lv_obj_set_style_local_text_font(btn1_lvl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_sys_48);
  lv_label_set_text_static(btn1_lvl, System::SystemTask::displayApp->brightnessController.GetIcon());

  btn2 = lv_btn_create(lv_scr_act(), nullptr);
  btn2->user_data = this;
  lv_obj_set_event_cb(btn2, buttonEventHandler);
  lv_obj_add_style(btn2, LV_BTN_PART_MAIN, &btn_style);
  lv_obj_set_size(btn2, buttonWidth, buttonHeight);
  lv_obj_align(btn2, nullptr, LV_ALIGN_IN_TOP_RIGHT, -buttonXOffset, innerDistance);

  lv_obj_t* lbl_btn;
  lbl_btn = lv_label_create(btn2, nullptr);
  lv_obj_set_style_local_text_font(lbl_btn, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_sys_48);
  lv_label_set_text_static(lbl_btn, Symbols::flashlight);

  btn3 = lv_btn_create(lv_scr_act(), nullptr);
  btn3->user_data = this;
  lv_obj_set_event_cb(btn3, buttonEventHandler);
  lv_obj_add_style(btn3, LV_BTN_PART_MAIN, &btn_style);
  lv_obj_set_style_local_bg_color(btn3, LV_BTN_PART_MAIN, static_cast<lv_state_t>(ButtonState::NotificationsOff), LV_COLOR_RED);
  static constexpr lv_color_t violet = LV_COLOR_MAKE(0x60, 0x00, 0xff);
  lv_obj_set_style_local_bg_color(btn3, LV_BTN_PART_MAIN, static_cast<lv_state_t>(ButtonState::Sleep), violet);
  lv_obj_set_size(btn3, buttonWidth, buttonHeight);
  lv_obj_align(btn3, nullptr, LV_ALIGN_IN_BOTTOM_LEFT, buttonXOffset, 0);

  btn3_lvl = lv_label_create(btn3, nullptr);
  lv_obj_set_style_local_text_font(btn3_lvl, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_sys_48);

  if (System::SystemTask::displayApp->settingsController.GetNotificationStatus() == Controllers::Settings::Notification::On) {
    lv_label_set_text_static(btn3_lvl, Symbols::notificationsOn);
    lv_obj_set_state(btn3, static_cast<lv_state_t>(ButtonState::NotificationsOn));
  } else if (System::SystemTask::displayApp->settingsController.GetNotificationStatus() == Controllers::Settings::Notification::Off) {
    lv_label_set_text_static(btn3_lvl, Symbols::notificationsOff);
  } else {
    lv_label_set_text_static(btn3_lvl, Symbols::sleep);
    lv_obj_set_state(btn3, static_cast<lv_state_t>(ButtonState::Sleep));
  }

  btn4 = lv_btn_create(lv_scr_act(), nullptr);
  btn4->user_data = this;
  lv_obj_set_event_cb(btn4, buttonEventHandler);
  lv_obj_add_style(btn4, LV_BTN_PART_MAIN, &btn_style);
  lv_obj_set_size(btn4, buttonWidth, buttonHeight);
  lv_obj_align(btn4, nullptr, LV_ALIGN_IN_BOTTOM_RIGHT, -buttonXOffset, 0);

  lbl_btn = lv_label_create(btn4, nullptr);
  lv_obj_set_style_local_text_font(lbl_btn, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &lv_font_sys_48);
  lv_label_set_text_static(lbl_btn, Symbols::settings);
  running = true;
}

bool QuickSettings::UnLoad() {
  if (running)
    lv_style_reset(&btn_style);
  return Screen::UnLoad();
}

QuickSettings::~QuickSettings() {
  UnLoad();
  System::SystemTask::displayApp->settingsController.SaveSettings();
}

void QuickSettings::buttonEventHandler(lv_obj_t* obj, lv_event_t event) {
  if (event == LV_EVENT_CLICKED) {
    static_cast<QuickSettings*>(obj->user_data)->onButtonEvent(obj);
  }
}

void QuickSettings::onButtonEvent(lv_obj_t* object) {
  if (enableEvent) {
    if (object == btn2) {
      System::SystemTask::displayApp->StartApp(Apps::FlashLight);
    } else if (object == btn1) {
      System::SystemTask::displayApp->brightnessController.Step();
      lv_label_set_text_static(btn1_lvl, System::SystemTask::displayApp->brightnessController.GetIcon());
      System::SystemTask::displayApp->settingsController.SetBrightness(System::SystemTask::displayApp->brightnessController.Level());
    } else if (object == btn3) {
      if (System::SystemTask::displayApp->settingsController.GetNotificationStatus() == Controllers::Settings::Notification::On) {
        System::SystemTask::displayApp->settingsController.SetNotificationStatus(Controllers::Settings::Notification::Off);
        lv_label_set_text_static(btn3_lvl, Symbols::notificationsOff);
        lv_obj_set_state(btn3, static_cast<lv_state_t>(ButtonState::NotificationsOff));
      } else if (System::SystemTask::displayApp->settingsController.GetNotificationStatus() == Controllers::Settings::Notification::Off) {
        System::SystemTask::displayApp->settingsController.SetNotificationStatus(Controllers::Settings::Notification::Sleep);
        lv_label_set_text_static(btn3_lvl, Symbols::sleep);
        lv_obj_set_state(btn3, static_cast<lv_state_t>(ButtonState::Sleep));
      } else {
        System::SystemTask::displayApp->settingsController.SetNotificationStatus(Controllers::Settings::Notification::On);
        lv_label_set_text_static(btn3_lvl, Symbols::notificationsOn);
        lv_obj_set_state(btn3, static_cast<lv_state_t>(ButtonState::NotificationsOn));
        System::SystemTask::displayApp->motorController.RunForDuration(35);
      }
    } else if (object == btn4) {
      System::SystemTask::displayApp->StartApp(Apps::Settings);
    }
  }
}

bool QuickSettings::OnTouchEvent(Applications::TouchEvents event) {
  enableEvent = event == TouchEvents::Tap;
  return enableEvent;
}