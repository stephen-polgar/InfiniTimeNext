#pragma once

#include <FreeRTOS.h>
#include <timers.h>
#include "ScreenRefresh.h"
#include "components/ble/NotificationManager.h"
#include <memory>

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class Notifications : public ScreenRefresh {
      public:
        explicit Notifications(Apps id);
        ~Notifications() override;

        void Load() override;
        bool UnLoad() override;
        bool OnTouchEvent(Applications::TouchEvents event) override;

        class NotificationItem {
        public:
          NotificationItem();
          NotificationItem(const char* title,
                           const char* msg,
                           uint8_t notifNr,
                           Controllers::NotificationManager::Categories,
                           uint8_t notifNb);
          ~NotificationItem();

          bool IsRunning() const {
            return running;
          }

        private:
          void onCallButtonEvent(lv_obj_t*);
          static void callEventHandler(lv_obj_t* obj, lv_event_t event);
          lv_obj_t* container;
          lv_obj_t* subject_container;
          lv_obj_t* bt_accept;
          lv_obj_t* bt_mute;
          lv_obj_t* bt_reject;
          lv_obj_t* label_accept;
          lv_obj_t* label_mute;
          lv_obj_t* label_reject;
          bool running = true;
        };

      private:
        void Refresh() override;
        void load(Controllers::NotificationManager::Notification& notification, FullRefreshDirections direction);
        void dismissToBlack();
        void onPreviewInteraction();
        void onPreviewDismiss();

        std::unique_ptr<NotificationItem> currentItem;
        uint8_t currentId;
        bool validDisplay;
        bool afterDismissNextMessageFromAbove;

        lv_point_t timeoutLinePoints[2];
        lv_obj_t* timeoutLine;
        TickType_t timeoutTickCountStart;
        void deleteTimeOut();

        static const TickType_t timeoutLength = pdMS_TO_TICKS(7000);
        bool interacted;
        bool dismissingNotification;       
      };
    }
  }
}
