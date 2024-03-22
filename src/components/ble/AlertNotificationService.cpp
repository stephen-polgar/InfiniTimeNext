#include "components/ble/AlertNotificationService.h"
#include <hal/nrf_rtc.h>
#include "systemtask/SystemTask.h"

using namespace Pinetime::Controllers;

constexpr ble_uuid16_t AlertNotificationService::ansUuid;
constexpr ble_uuid16_t AlertNotificationService::ansCharUuid;
constexpr ble_uuid128_t AlertNotificationService::notificationEventUuid;

void AlertNotificationService::Init() {
  int res;
  res = ble_gatts_count_cfg(serviceDefinition);
  ASSERT(res == 0);

  res = ble_gatts_add_svcs(serviceDefinition);
  ASSERT(res == 0);
}

AlertNotificationService::AlertNotificationService()
  : characteristicDefinition {{.uuid = &ansCharUuid.u, .access_cb = alertNotificationCallback, .arg = this, .flags = BLE_GATT_CHR_F_WRITE},
                              {.uuid = &notificationEventUuid.u,
                               .access_cb = alertNotificationCallback,
                               .arg = this,
                               .flags = BLE_GATT_CHR_F_NOTIFY,
                               .val_handle = &eventHandle},
                              {0}},
    serviceDefinition {
      {/* Device Information Service */
       .type = BLE_GATT_SVC_TYPE_PRIMARY,
       .uuid = &ansUuid.u,
       .characteristics = characteristicDefinition},
      {0},
    }{
}

int AlertNotificationService::onAlert(struct ble_gatt_access_ctxt* ctxt) {
  if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
    constexpr uint8_t stringTerminatorSize = 1; // end of string '\0'
    constexpr uint8_t headerSize = 3;    
    const uint8_t maxBufferSize = NotificationManager::MaximumMessageSize + headerSize;

    // Ignore notifications with empty message
    const auto packetLen = OS_MBUF_PKTLEN(ctxt->om);
    if (packetLen <= headerSize) {
      return 0;
    }

    uint8_t bufferSize = std::min(size_t(packetLen + stringTerminatorSize), size_t(maxBufferSize));
    uint8_t messageSize = std::min(size_t(NotificationManager::MaximumMessageSize), size_t(bufferSize - headerSize));
    Categories category;

    NotificationManager::Notification notif;
    os_mbuf_copydata(ctxt->om, headerSize, messageSize - 1, notif.message.data());
    os_mbuf_copydata(ctxt->om, 0, 1, &category);
    notif.message[messageSize - 1] = '\0';
    notif.size = messageSize;

    // TODO convert all ANS categories to NotificationController categories
    switch (category) {
      case Categories::Call:
        notif.category = Controllers::NotificationManager::Categories::IncomingCall;
        break;
      default:
        notif.category = Controllers::NotificationManager::Categories::SimpleAlert;
        break;
    }   
    System::SystemTask::displayApp->notificationManager.Push(std::move(notif));
    System::SystemTask::displayApp->systemTask->PushMessage(System::Messages::OnNewNotification);
  }
  return 0;
}
int AlertNotificationService::alertNotificationCallback(uint16_t /*conn_handle*/, uint16_t /*attr_handle*/, struct ble_gatt_access_ctxt* ctxt, void* arg) {
  return static_cast<AlertNotificationService*>(arg)->onAlert(ctxt);
}


void AlertNotificationService::AcceptIncomingCall() {
  auto response = IncomingCallResponses::Answer;
  auto* om = ble_hs_mbuf_from_flat(&response, 1);

  uint16_t connectionHandle = System::SystemTask::displayApp->systemTask->nimbleController.connHandle();

  if (connectionHandle == 0 || connectionHandle == BLE_HS_CONN_HANDLE_NONE) {
    return;
  }

  ble_gattc_notify_custom(connectionHandle, eventHandle, om);
}

void AlertNotificationService::RejectIncomingCall() {
  auto response = IncomingCallResponses::Reject;
  auto* om = ble_hs_mbuf_from_flat(&response, 1);

  uint16_t connectionHandle = System::SystemTask::displayApp->systemTask->nimbleController.connHandle();

  if (connectionHandle == 0 || connectionHandle == BLE_HS_CONN_HANDLE_NONE) {
    return;
  }

  ble_gattc_notify_custom(connectionHandle, eventHandle, om);
}

void AlertNotificationService::MuteIncomingCall() {
  auto response = IncomingCallResponses::Mute;
  auto* om = ble_hs_mbuf_from_flat(&response, 1);

  uint16_t connectionHandle = System::SystemTask::displayApp->systemTask->nimbleController.connHandle();

  if (connectionHandle == 0 || connectionHandle == BLE_HS_CONN_HANDLE_NONE) {
    return;
  }

  ble_gattc_notify_custom(connectionHandle, eventHandle, om);
}
