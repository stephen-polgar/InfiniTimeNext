#include "Notifications.h"
#include "components/motor/MotorController.h"
#include "systemtask/SystemTask.h"
#include "Symbols.h"
#include "displayapp/InfiniTimeTheme.h"

using namespace Pinetime::Applications::Screens;
extern lv_font_t jetbrains_mono_extrabold_compressed;
extern lv_font_t jetbrains_mono_bold_20;

Notifications::Notifications(Apps id) : Screen(id) {
}

void Notifications::Load() {
  running = true;
  afterDismissNextMessageFromAbove = false;
  timeoutLinePoints[0] = {0, 1};
  timeoutLinePoints[1] = {239, 1};
  dismissingNotification = false;

  auto* notificationManager = &System::SystemTask::displayApp->notificationManager;
  notificationManager->ClearNewNotificationFlag();
  auto notification = notificationManager->GetLastNotification();
  if (notification.valid) {
    currentId = notification.id;
    currentItem = std::make_unique<NotificationItem>(notification.Title(),
                                                     notification.Message(),
                                                     1,
                                                     notification.category,
                                                     notificationManager->NbNotifications());
    validDisplay = true;
  } else {
    currentItem = std::make_unique<NotificationItem>();
    validDisplay = false;
  }
  if (Id == Apps::NotificationsPreview) {
    System::SystemTask::displayApp->systemTask->PushMessage(System::Messages::DisableSleeping);
    if (notification.category == Controllers::NotificationManager::Categories::IncomingCall) {
      System::SystemTask::displayApp->motorController.StartRinging();
    } else {
      System::SystemTask::displayApp->motorController.RunForDuration(35);
    }

    timeoutLine = lv_line_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_line_width(timeoutLine, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, 3);
    lv_obj_set_style_local_line_color(timeoutLine, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    lv_obj_set_style_local_line_rounded(timeoutLine, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, true);

    lv_line_set_points(timeoutLine, timeoutLinePoints, 2);
    timeoutTickCountStart = xTaskGetTickCount();
    interacted = false;
  } else {
    interacted = true;
    timeoutLine = NULL;
  }
  taskRefresh = lv_task_create(RefreshTaskCallback, LV_DISP_DEF_REFR_PERIOD, LV_TASK_PRIO_MID, this);
}

void Notifications::deleteTimeOut() {
  if (timeoutLine) {
    lv_obj_del(timeoutLine);
    timeoutLine = NULL;
  }
}

bool Notifications::UnLoad() {
  if (taskRefresh) {
    lv_task_del(taskRefresh);
    taskRefresh = NULL;
    currentItem.reset(); 
    // make sure we stop any vibrations before exiting
    System::SystemTask::displayApp->motorController.StopRinging();
    System::SystemTask::displayApp->systemTask->PushMessage(System::Messages::EnableSleeping);
    lv_obj_clean(lv_scr_act());
  }
  return running;
}

Notifications::~Notifications() {
  UnLoad();
}

void Notifications::Refresh() {
  if (Id == Apps::NotificationsPreview && timeoutLine) {
    TickType_t tick = xTaskGetTickCount();
    int32_t pos = LV_HOR_RES - ((tick - timeoutTickCountStart) / (timeoutLength / LV_HOR_RES));
    if (pos <= 0) {
      running = false;
    } else {
      timeoutLinePoints[1].x = pos;
      lv_line_set_points(timeoutLine, timeoutLinePoints, 2);
    }
  } else if (Id == Apps::NotificationsPreview && dismissingNotification) {
    running = false;
    currentItem = std::make_unique<NotificationItem>();
  } else if (dismissingNotification) {
    dismissingNotification = false;
    auto notification = System::SystemTask::displayApp->notificationManager.Get(currentId);
    if (!notification.valid) {
      notification = System::SystemTask::displayApp->notificationManager.GetLastNotification();
    }
    currentId = notification.id;
    if (!notification.valid) {
      validDisplay = false;
    }
    currentItem.reset();
    if (afterDismissNextMessageFromAbove) {
      System::SystemTask::displayApp->SetFullRefresh(Screen::FullRefreshDirections::Down);
    } else {
      System::SystemTask::displayApp->SetFullRefresh(Screen::FullRefreshDirections::Up);
    }
    if (validDisplay) {
      uint8_t currentIdx = System::SystemTask::displayApp->notificationManager.IndexOf(currentId);
      currentItem = std::make_unique<NotificationItem>(notification.Title(),
                                                       notification.Message(),
                                                       currentIdx + 1,
                                                       notification.category,
                                                       System::SystemTask::displayApp->notificationManager.NbNotifications());
    } else {
      currentItem = std::make_unique<NotificationItem>();
    }
  }
  running = currentItem->IsRunning() && running;
}

void Notifications::onPreviewInteraction() {
  System::SystemTask::displayApp->systemTask->PushMessage(System::Messages::EnableSleeping);
  System::SystemTask::displayApp->motorController.StopRinging();
  deleteTimeOut();
}

void Notifications::dismissToBlack() {
  currentItem.reset();
  System::SystemTask::displayApp->SetFullRefresh(Screen::FullRefreshDirections::RightAnim);
  // create black transition screen to let the notification dismiss to blackness
  lv_obj_t* blackBox = lv_obj_create(lv_scr_act(), nullptr);
  lv_obj_set_size(blackBox, LV_HOR_RES, LV_VER_RES);
  lv_obj_set_style_local_bg_color(blackBox, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  dismissingNotification = true;
}

void Notifications::onPreviewDismiss() {
  System::SystemTask::displayApp->notificationManager.Dismiss(currentId);
  deleteTimeOut();
  dismissToBlack();
}

void Notifications::load(Controllers::NotificationManager::Notification& notification, FullRefreshDirections direction) {
  validDisplay = true;
  currentItem.reset();
  System::SystemTask::displayApp->SetFullRefresh(direction);
  currentItem = std::make_unique<NotificationItem>(notification.Title(),
                                                   notification.Message(),
                                                   System::SystemTask::displayApp->notificationManager.IndexOf(currentId) + 1,
                                                   notification.category,
                                                   System::SystemTask::displayApp->notificationManager.NbNotifications());
}

bool Notifications::OnTouchEvent(Pinetime::Applications::TouchEvents event) {
  if (Id == Apps::NotificationsPreview) {
    switch (event) {
      case TouchEvents::Tap:
        if (!interacted) {
          interacted = true;
          onPreviewInteraction();
          return true;
        }
        return false;
      case TouchEvents::SwipeRight:
        onPreviewDismiss();
        break;
      case TouchEvents::SwipeDown: {
        Controllers::NotificationManager::Notification previousNotification;
        if (validDisplay) {
          previousNotification = System::SystemTask::displayApp->notificationManager.GetPrevious(currentId);
        } else {
          previousNotification = System::SystemTask::displayApp->notificationManager.GetLastNotification();
        }
        if (!previousNotification.valid) {
          return true;
        }
        deleteTimeOut();
        Id = Apps::Notifications;
        currentId = previousNotification.id;
        load(previousNotification, FullRefreshDirections::Down);
      } break;
      default:
        return false;
    }
    return true;
  }

  switch (event) {
    case TouchEvents::SwipeRight:
      if (validDisplay) {
        auto previousMessage = System::SystemTask::displayApp->notificationManager.GetPrevious(currentId);
        auto nextMessage = System::SystemTask::displayApp->notificationManager.GetNext(currentId);
        afterDismissNextMessageFromAbove = previousMessage.valid;
        System::SystemTask::displayApp->notificationManager.Dismiss(currentId);
        if (previousMessage.valid) {
          currentId = previousMessage.id;
        } else if (nextMessage.valid) {
          currentId = nextMessage.id;
        } else {
          // don't update id, won't be found be refresh and try to load latest message or no message box
        }
        dismissToBlack();
        return true;
      }
      return false;
    case TouchEvents::SwipeDown: {
      Controllers::NotificationManager::Notification previousNotification;
      if (validDisplay) {
        previousNotification = System::SystemTask::displayApp->notificationManager.GetPrevious(currentId);
      } else {
        previousNotification = System::SystemTask::displayApp->notificationManager.GetLastNotification();
      }
      if (!previousNotification.valid) {
        return true;
      }
      currentId = previousNotification.id;
      load(previousNotification, FullRefreshDirections::Down);
    } break;
    case TouchEvents::SwipeUp: {
      Controllers::NotificationManager::Notification nextNotification;
      if (validDisplay) {
        nextNotification = System::SystemTask::displayApp->notificationManager.GetNext(currentId);
      } else {
        nextNotification = System::SystemTask::displayApp->notificationManager.GetLastNotification();
      }

      if (!nextNotification.valid) {
        running = false;
        return false;
      }

      currentId = nextNotification.id;
      load(nextNotification, FullRefreshDirections::Up);
    } break;
    default:
      return false;
  }
  return true;
}

Notifications::NotificationItem::NotificationItem()
  : NotificationItem("Notification", "No notification to display", 0, Controllers::NotificationManager::Categories::Unknown, 0) {
}

Notifications::NotificationItem::NotificationItem(const char* title,
                                                  const char* msg,
                                                  uint8_t notifNr,
                                                  Controllers::NotificationManager::Categories category,
                                                  uint8_t notifNb) {
  container = lv_cont_create(lv_scr_act(), nullptr);
  lv_obj_set_size(container, LV_HOR_RES, LV_VER_RES);
  lv_obj_set_style_local_bg_color(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_set_style_local_pad_all(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_pad_inner(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_border_width(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);

  subject_container = lv_cont_create(container, nullptr);
  lv_obj_set_style_local_bg_color(subject_container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, Colors::bgAlt);
  lv_obj_set_style_local_pad_all(subject_container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 10);
  lv_obj_set_style_local_pad_inner(subject_container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 5);
  lv_obj_set_style_local_border_width(subject_container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);

  lv_obj_set_pos(subject_container, 0, 50);
  lv_obj_set_size(subject_container, LV_HOR_RES, LV_VER_RES - 50);
  lv_cont_set_layout(subject_container, LV_LAYOUT_COLUMN_LEFT);
  lv_cont_set_fit(subject_container, LV_FIT_NONE);

  lv_obj_t* alert_count = lv_label_create(container, nullptr);
  lv_label_set_text_fmt(alert_count, "%i/%i", notifNr, notifNb);
  lv_obj_align(alert_count, nullptr, LV_ALIGN_IN_TOP_RIGHT, 0, 16);

  lv_obj_t* alert_type = lv_label_create(container, nullptr);
  lv_obj_set_style_local_text_color(alert_type, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::orange);
  if (!title) {
    lv_label_set_text_static(alert_type, "Notification");
  } else {
    // copy title to label and replace newlines with spaces
    lv_label_set_text(alert_type, title);
    char* pchar = strchr(lv_label_get_text(alert_type), '\n');
    while (pchar != nullptr) {
      *pchar = ' ';
      pchar = strchr(pchar + 1, '\n');
    }
    lv_label_refr_text(alert_type);
  }
  lv_label_set_long_mode(alert_type, LV_LABEL_LONG_SROLL_CIRC);
  lv_obj_set_width(alert_type, 180);
  lv_obj_align(alert_type, nullptr, LV_ALIGN_IN_TOP_LEFT, 0, 16);

  lv_obj_t* alert_subject = lv_label_create(subject_container, nullptr);
  lv_label_set_long_mode(alert_subject, LV_LABEL_LONG_BREAK);
  lv_obj_set_width(alert_subject, LV_HOR_RES - 20);

  switch (category) {
    default:
      lv_label_set_text(alert_subject, msg);
      break;
    case Controllers::NotificationManager::Categories::IncomingCall: {
      lv_obj_set_height(subject_container, 108);
      lv_label_set_text_static(alert_subject, "Incoming call from");

      lv_obj_t* alert_caller = lv_label_create(subject_container, nullptr);
      lv_obj_align(alert_caller, alert_subject, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
      lv_label_set_long_mode(alert_caller, LV_LABEL_LONG_BREAK);
      lv_obj_set_width(alert_caller, LV_HOR_RES - 20);
      lv_label_set_text(alert_caller, msg);

      bt_accept = lv_btn_create(container, nullptr);
      bt_accept->user_data = this;
      lv_obj_set_event_cb(bt_accept, callEventHandler);
      lv_obj_set_size(bt_accept, 76, 76);
      lv_obj_align(bt_accept, nullptr, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
      label_accept = lv_label_create(bt_accept, nullptr);
      lv_label_set_text_static(label_accept, Symbols::phone);
      lv_obj_set_style_local_bg_color(bt_accept, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::highlight);

      bt_reject = lv_btn_create(container, nullptr);
      bt_reject->user_data = this;
      lv_obj_set_event_cb(bt_reject, callEventHandler);
      lv_obj_set_size(bt_reject, 76, 76);
      lv_obj_align(bt_reject, nullptr, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
      label_reject = lv_label_create(bt_reject, nullptr);
      lv_label_set_text_static(label_reject, Symbols::phoneSlash);
      lv_obj_set_style_local_bg_color(bt_reject, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_RED);

      bt_mute = lv_btn_create(container, nullptr);
      bt_mute->user_data = this;
      lv_obj_set_event_cb(bt_mute, callEventHandler);
      lv_obj_set_size(bt_mute, 76, 76);
      lv_obj_align(bt_mute, nullptr, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
      label_mute = lv_label_create(bt_mute, nullptr);
      lv_label_set_text_static(label_mute, Symbols::volumMute);
      lv_obj_set_style_local_bg_color(bt_mute, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::lightGray);
    } break;
  }
}

void Notifications::NotificationItem::onCallButtonEvent(lv_obj_t* obj) {
  System::SystemTask::displayApp->motorController.StopRinging();
  if (obj == bt_accept) {
    System::SystemTask::displayApp->systemTask->nimbleController.alertService.AcceptIncomingCall();
  } else if (obj == bt_reject) {
    System::SystemTask::displayApp->systemTask->nimbleController.alertService.RejectIncomingCall();
  } else if (obj == bt_mute) {
    System::SystemTask::displayApp->systemTask->nimbleController.alertService.MuteIncomingCall();
  }
  running = false;
}

void Notifications::NotificationItem::callEventHandler(lv_obj_t* obj, lv_event_t event) {
  if (event == LV_EVENT_CLICKED)
    static_cast<Notifications::NotificationItem*>(obj->user_data)->onCallButtonEvent(obj);
}

Notifications::NotificationItem::~NotificationItem() {
  lv_obj_clean(lv_scr_act());
}
