#pragma once

#include <array>
#include <atomic>

namespace Pinetime {
  namespace Controllers {
    class NotificationManager {
    public:
      enum class Categories : uint8_t {
        Unknown,
        SimpleAlert,
        Email,
        News,
        IncomingCall,
        MissedCall,
        Sms,
        VoiceMail,
        Schedule,
        HighProriotyAlert,
        InstantMessage
      };
      static constexpr uint8_t MaximumMessageSize = 100;

      struct Notification {
        std::array<char, MaximumMessageSize + 1> message;
        uint8_t size;
        Categories category = Categories::Unknown;
        uint8_t id = 0;
        bool valid = false;
        const char* Message();
        const char* Title();
      };

      void Push(Notification&& notif);
      Notification GetLastNotification();
      Notification Get(uint8_t id);
      Notification GetNext(uint8_t id);
      Notification GetPrevious(uint8_t id);
      uint8_t IndexOf(uint8_t id);
      // Return the index of the notification with the specified id, if not found return NbNotifications()
    
      bool ClearNewNotificationFlag();
      bool AreNewNotificationsAvailable();
      void Dismiss(uint8_t id);

      bool IsEmpty() const {
        return size == 0;
      }

      uint8_t NbNotifications();

    private:     
      uint8_t nextId = 0;     
      Notification& At(uint8_t idx);
      void DismissIdx(uint8_t idx);
      static constexpr uint8_t TotalNbNotifications = 8;
      std::array<Notification, TotalNbNotifications> notifications;
      uint8_t beginIdx = TotalNbNotifications - 1; // index of the newest notification
      uint8_t size = 0;                           // number of valid notifications in buffer
      std::atomic<bool> newNotification = false;
    };
  }
}
