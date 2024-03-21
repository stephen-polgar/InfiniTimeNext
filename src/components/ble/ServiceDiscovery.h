#pragma once

#include "components/ble/BleClient.h"
#include <array>

namespace Pinetime {
  namespace Controllers {
    class ServiceDiscovery {
    public:
      ServiceDiscovery(std::array<BleClient*, 2>&& bleClients);

      void StartDiscovery(uint16_t connectionHandle);

    private:
      BleClient** clientIterator;
      std::array<BleClient*, 2> clients;
      void OnServiceDiscovered(uint16_t connectionHandle);
      void DiscoverNextService(uint16_t connectionHandle);
    };
  }
}
