#include "components/ble/NotificationManager.h"
#include <algorithm>
#include <cassert>

using namespace Pinetime::Controllers;

void NotificationManager::Push(NotificationManager::Notification&& notif) {
  notif.id = ++nextId;
  notif.valid = true;
  newNotification = true;
  if (beginIdx > 0) {
    --beginIdx;
  } else {
    beginIdx = notifications.size() - 1;
  }
  notifications[beginIdx] = std::move(notif);
  if (size < notifications.size()) {
    size++;
  }
}

NotificationManager::Notification NotificationManager::GetLastNotification() {
  return IsEmpty() ? NotificationManager::Notification {} : At(0);
}

NotificationManager::Notification& NotificationManager::At(uint8_t idx) {
  if (idx >= notifications.size()) {
    assert(false);
    return notifications.at(beginIdx); // this should not happen
  }
  uint8_t read_idx = (beginIdx + idx) % notifications.size();
  return notifications.at(read_idx);
}

uint8_t NotificationManager::IndexOf(uint8_t id) {
  for (uint8_t i = 0; i < size; i++) {
    if (At(i).id == id)
      return i;
  }
  return size;
}

NotificationManager::Notification NotificationManager::Get(uint8_t id) {
  uint8_t idx = IndexOf(id);
  return idx == size ? NotificationManager::Notification {} : At(idx);
}

NotificationManager::Notification NotificationManager::GetNext(uint8_t id) {
  uint8_t idx = IndexOf(id);
  if (idx == size || idx == 0 || idx > notifications.size()) {
    return {};
  }
  return At(idx - 1);
}

NotificationManager::Notification NotificationManager::GetPrevious(uint8_t id) {
  uint8_t idx = IndexOf(id);
  if (idx == size || (idx + 1 >= (uint8_t)notifications.size())) {
    return {};
  }
  return At(idx + 1);
}

void NotificationManager::DismissIdx(uint8_t idx) {
  if (size) {
    if (idx >= size) {
      assert(false);
      return; // this should not happen
    }
    if (idx == 0) { // just remove the first element, don't need to change the other elements
      notifications.at(beginIdx).valid = false;
      beginIdx = (beginIdx + 1) % notifications.size();
    } else {
      // overwrite the specified entry by moving all later messages one index to the front
      for (uint8_t i = idx; i < size - 1; ++i) {
        At(i) = At(i + 1);
      }
      At(size - 1).valid = false;
    }
    --size;
  }
}

void NotificationManager::Dismiss(uint8_t id) {
  uint8_t idx = IndexOf(id);
  if (idx != size) DismissIdx(idx);
}

bool NotificationManager::AreNewNotificationsAvailable() {
  return newNotification;
}

bool NotificationManager::ClearNewNotificationFlag() {
  return newNotification.exchange(false);
}

uint8_t NotificationManager::NbNotifications() {
  return size;
}

const char* NotificationManager::Notification::Message() {
  const char* itField = std::find(message.begin(), message.begin() + size - 1, '\0');
  if (itField != message.begin() + size - 1) {
    const char* ptr = (itField) + 1;
    return ptr;
  }
  return message.data();
}

const char* NotificationManager::Notification::Title() {
  const char* itField = std::find(message.begin(), message.begin() + size - 1, '\0');
  if (itField != message.begin() + size - 1) {
    return message.data();
  }
  return {};
}
