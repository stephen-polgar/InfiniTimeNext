#pragma once

#include "components/ble/AlertNotificationClient.h"
#include "components/ble/AlertNotificationService.h"
#include "components/ble/BatteryInformationService.h"
#include "components/ble/CurrentTimeClient.h"
#include "components/ble/CurrentTimeService.h"
#include "components/ble/DeviceInformationService.h"
#include "components/ble/DfuService.h"
#include "components/ble/FSService.h"
#include "components/ble/ImmediateAlertService.h"
#include "components/ble/MusicService.h"
#include "components/ble/NavigationService.h"
#include "components/ble/ServiceDiscovery.h"
#include "components/ble/SimpleWeatherService.h"

namespace Pinetime {
  namespace Controllers {
    class NimbleController {

    public:
      NimbleController();
      void Init();
      void StartAdvertising();     
      void StartDiscovery();
      
      uint16_t connHandle();
      void NotifyBatteryLevel(uint8_t level);

      void RestartFastAdv() {
        fastAdvCount = 0;
      };

      void EnableRadio();
      void DisableRadio();

      MusicService musicService;
      NavigationService navService;
      AlertNotificationService alertService;
      SimpleWeatherService weatherService;     
     
    private:
      void PersistBond(struct ble_gap_conn_desc& desc);
      void RestoreBond();

      DfuService dfuService;
      DeviceInformationService deviceInformationService;
      CurrentTimeClient currentTimeClient;
      AlertNotificationClient alertNotificationClient;
      CurrentTimeService currentTimeService;
      BatteryInformationService batteryInformationService;
      ImmediateAlertService immediateAlertService;
      FSService fsService;
      ServiceDiscovery serviceDiscovery;

      uint8_t addrType;
      uint16_t connectionHandle = BLE_HS_CONN_HANDLE_NONE;
      uint8_t fastAdvCount = 0;
      uint8_t bondId[16] = {0};
      
      int onGAPEvent(ble_gap_event* event);
      static int gapEventCallback(struct ble_gap_event* event, void* arg);
      static void nimble_on_reset(int /*reason*/);
      static void nimble_on_sync(void);

      static constexpr ble_uuid128_t dfuServiceUuid {
        .u {.type = BLE_UUID_TYPE_128},
        .value = {0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x30, 0x15, 0x00, 0x00}};
    };

    static constexpr const char* deviceName = "InfiniTimeNext";
    static constexpr const char* fileName = "/bond.dat";
    static NimbleController* instance;
  }
}
