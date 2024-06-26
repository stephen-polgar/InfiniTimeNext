#include "components/ble/AlertNotificationClient.h"
#include "components/ble/NotificationManager.h"
#include "systemtask/SystemTask.h"

using namespace Pinetime::Controllers;
constexpr ble_uuid16_t AlertNotificationClient::ansServiceUuid;
constexpr ble_uuid16_t AlertNotificationClient::supportedNewAlertCategoryUuid;
constexpr ble_uuid16_t AlertNotificationClient::supportedUnreadAlertCategoryUuid;
constexpr ble_uuid16_t AlertNotificationClient::newAlertUuid;
constexpr ble_uuid16_t AlertNotificationClient::unreadAlertStatusUuid;
constexpr ble_uuid16_t AlertNotificationClient::controlPointUuid;

bool AlertNotificationClient::onDiscoveryEvent(uint16_t connectionHandle, const ble_gatt_error* error, const ble_gatt_svc* service) {
  if (service == nullptr && error->status == BLE_HS_EDONE) {
    if (isDiscovered) {
      ble_gattc_disc_all_chrs(connectionHandle, ansStartHandle, ansEndHandle, onAlertNotificationCharacteristicDiscoveredCallback, this);
    } else {
      onServiceDiscovered(connectionHandle);
    }
    return true;
  }

  if (service != nullptr && ble_uuid_cmp(&ansServiceUuid.u, &service->uuid.u) == 0) {
    ansStartHandle = service->start_handle;
    ansEndHandle = service->end_handle;
    isDiscovered = true;
  }
  return false;
}

int AlertNotificationClient::onCharacteristicsDiscoveryEvent(uint16_t connectionHandle,
                                                             const ble_gatt_error* error,
                                                             const ble_gatt_chr* characteristic) {
  if (error->status != 0 && error->status != BLE_HS_EDONE) {
    onServiceDiscovered(connectionHandle);
    return 0;
  }

  if (characteristic == nullptr && error->status == BLE_HS_EDONE) {
    if (isCharacteristicDiscovered) {
      ble_gattc_disc_all_dscs(connectionHandle, newAlertHandle, ansEndHandle, onAlertNotificationDescriptorDiscoveryEventCallback, this);
    } else
      onServiceDiscovered(connectionHandle);
  } else {
    if (characteristic != nullptr && ble_uuid_cmp(&supportedNewAlertCategoryUuid.u, &characteristic->uuid.u) == 0) {
      supportedNewAlertCategoryHandle = characteristic->val_handle;
    } else if (characteristic != nullptr && ble_uuid_cmp(&supportedUnreadAlertCategoryUuid.u, &characteristic->uuid.u) == 0) {
      supportedUnreadAlertCategoryHandle = characteristic->val_handle;
    } else if (characteristic != nullptr && ble_uuid_cmp(&newAlertUuid.u, &characteristic->uuid.u) == 0) {
      newAlertHandle = characteristic->val_handle;
      newAlertDefHandle = characteristic->def_handle;
      isCharacteristicDiscovered = true;
    } else if (characteristic != nullptr && ble_uuid_cmp(&unreadAlertStatusUuid.u, &characteristic->uuid.u) == 0) {
      unreadAlertStatusHandle = characteristic->val_handle;
    } else if (characteristic != nullptr && ble_uuid_cmp(&controlPointUuid.u, &characteristic->uuid.u) == 0) {
      controlPointHandle = characteristic->val_handle;
    } // else  NRF_LOG_INFO("ANS Characteristic discovered : 0x%x", characteristic->val_handle);
  }
  return 0;
}

int AlertNotificationClient::onNewAlertSubcribe(uint16_t connectionHandle, const ble_gatt_error*) {
  /*
  if (error->status == 0) {
    NRF_LOG_INFO("ANS New alert subscribe OK");
  } else {
    NRF_LOG_INFO("ANS New alert subscribe ERROR");
  }
  */
  onServiceDiscovered(connectionHandle);

  return 0;
}

int AlertNotificationClient::onDescriptorDiscoveryEventCallback(uint16_t connectionHandle,
                                                                const ble_gatt_error* error,
                                                                uint16_t characteristicValueHandle,
                                                                const ble_gatt_dsc* descriptor) {
  if (error->status == 0) {
    if (characteristicValueHandle == newAlertHandle && ble_uuid_cmp(&newAlertUuid.u, &descriptor->uuid.u)) {
      if (newAlertDescriptorHandle == 0) {
        newAlertDescriptorHandle = descriptor->handle;
        isDescriptorFound = true;
        uint8_t value[2];
        value[0] = 1;
        value[1] = 0;
        ble_gattc_write_flat(connectionHandle, newAlertDescriptorHandle, value, sizeof(value), newAlertSubcribeCallback, this);
      }
    }
  } else {
    if (!isDescriptorFound)
      onServiceDiscovered(connectionHandle);
  }
  return 0;
}

void AlertNotificationClient::OnNotification(ble_gap_event* event) {
  if (event->notify_rx.attr_handle == newAlertHandle) {
    constexpr uint8_t stringTerminatorSize = 1; // end of string '\0'
    constexpr uint8_t headerSize = 3;
    const uint8_t maxBufferSize = NotificationManager::MaximumMessageSize + headerSize;

    // Ignore notifications with empty message
    const auto packetLen = OS_MBUF_PKTLEN(event->notify_rx.om);
    if (packetLen <= headerSize)
      return;

    uint8_t bufferSize = std::min(size_t(packetLen + stringTerminatorSize), size_t(maxBufferSize));
    uint8_t messageSize = std::min( size_t(NotificationManager::MaximumMessageSize),  size_t(bufferSize - headerSize));

    NotificationManager::Notification notif;
    os_mbuf_copydata(event->notify_rx.om, headerSize, messageSize - 1, notif.message.data());
    notif.message[messageSize - 1] = '\0';
    notif.size = messageSize;
    notif.category = Pinetime::Controllers::NotificationManager::Categories::SimpleAlert;
    System::SystemTask::displayApp->notificationManager.Push(std::move(notif));
    System::SystemTask::displayApp->systemTask->PushMessage(Pinetime::System::Messages::OnNewNotification);
  }
}

void AlertNotificationClient::Reset() {
  ansStartHandle = 0;
  ansEndHandle = 0;
  supportedNewAlertCategoryHandle = 0;
  supportedUnreadAlertCategoryHandle = 0;
  newAlertHandle = 0;
  newAlertDescriptorHandle = 0;
  newAlertDefHandle = 0;
  unreadAlertStatusHandle = 0;
  controlPointHandle = 0;
  isDiscovered = false;
  isCharacteristicDiscovered = false;
  isDescriptorFound = false;
}

void AlertNotificationClient::Discover(uint16_t connectionHandle, std::function<void(uint16_t)> onServiceDiscovered) {
  this->onServiceDiscovered = onServiceDiscovered;
  ble_gattc_disc_svc_by_uuid(connectionHandle, &ansServiceUuid.u, onDiscoveryEventCallback, this);
}


int AlertNotificationClient::onDiscoveryEventCallback(uint16_t conn_handle,
                                                      const struct ble_gatt_error* error,
                                                      const struct ble_gatt_svc* service,
                                                      void* arg) {
  return static_cast<AlertNotificationClient*>(arg)->onDiscoveryEvent(conn_handle, error, service);
}

int AlertNotificationClient::onAlertNotificationCharacteristicDiscoveredCallback(uint16_t conn_handle,
                                                                                 const struct ble_gatt_error* error,
                                                                                 const struct ble_gatt_chr* chr,
                                                                                 void* arg) {
  return static_cast<AlertNotificationClient*>(arg)->onCharacteristicsDiscoveryEvent(conn_handle, error, chr);
}

int AlertNotificationClient::onAlertNotificationDescriptorDiscoveryEventCallback(uint16_t conn_handle,
                                                                                 const struct ble_gatt_error* error,
                                                                                 uint16_t chr_val_handle,
                                                                                 const struct ble_gatt_dsc* dsc,
                                                                                 void* arg) {
  return static_cast<AlertNotificationClient*>(arg)->onDescriptorDiscoveryEventCallback(conn_handle, error, chr_val_handle, dsc);
}

int AlertNotificationClient::newAlertSubcribeCallback(uint16_t conn_handle,
                                                      const struct ble_gatt_error* error,
                                                      struct ble_gatt_attr* /*attr*/,
                                                      void* arg) {
  return static_cast<AlertNotificationClient*>(arg)->onNewAlertSubcribe(conn_handle, error);
}
