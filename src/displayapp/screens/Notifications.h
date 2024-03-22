#pragma once

#include "Screen.h"
#include "components/ble/NotificationManager.h"
#include "components/motor/MotorController.h" // TODO : remove but required for build in simulator
#include <memory>

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class Notifications : public Screen {
      public:
        explicit Notifications();
        ~Notifications() override;

        void Load() override;
        bool UnLoad() override;
        void Refresh() override;
        bool OnTouchEvent(Applications::TouchEvents event) override;

        void DismissToBlack();
        void OnPreviewInteraction();
        void OnPreviewDismiss();

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
          static void callEventHandler(lv_obj_t* obj, lv_event_t event);
          void onCallButtonEvent(lv_obj_t*);
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
        std::unique_ptr<NotificationItem> currentItem;
        uint8_t currentId;
        bool validDisplay;
        bool afterDismissNextMessageFromAbove;

        lv_point_t timeoutLinePoints[2];
        lv_obj_t* timeoutLine;
        TickType_t timeoutTickCountStart;

        static const TickType_t timeoutLength = pdMS_TO_TICKS(7000);
        bool interacted;
        bool previewMode;
        bool dismissingNotification;

        lv_task_t* taskRefresh = NULL;
      };
    }
  }
}
