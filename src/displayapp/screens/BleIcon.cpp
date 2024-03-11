#include "BleIcon.h"

using namespace Pinetime::Applications::Screens;

const char* BleIcon::GetIcon(bool isConnected) {
  if (isConnected) {
    return Symbols::bluetooth;
  }
  return Symbols::none;
}
