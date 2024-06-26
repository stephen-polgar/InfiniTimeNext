#include "components/ble/ServiceDiscovery.h"

using namespace Pinetime::Controllers;

ServiceDiscovery::ServiceDiscovery(std::array<BleClient*, 2>&& clients) : clients {clients} {
}

void ServiceDiscovery::StartDiscovery(uint16_t connectionHandle) {
  clientIterator = clients.begin();
  DiscoverNextService(connectionHandle);
}

void ServiceDiscovery::OnServiceDiscovered(uint16_t connectionHandle) {
  clientIterator++;
  if (clientIterator != clients.end()) {
    DiscoverNextService(connectionHandle);
  }
}

void ServiceDiscovery::DiscoverNextService(uint16_t connectionHandle) {
  auto discoverNextService = [this](uint16_t connectionHandle) {
    this->OnServiceDiscovered(connectionHandle);
  };
  (*clientIterator)->Discover(connectionHandle, discoverNextService);
}