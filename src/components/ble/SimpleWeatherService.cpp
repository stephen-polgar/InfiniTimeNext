/*  Copyright (C) 2023 Jean-François Milants

    This file is part of InfiniTime.

    InfiniTime is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    InfiniTime is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "SimpleWeatherService.h"
#include "components/datetime/DateTimeController.h"
#include "systemtask/SystemTask.h"

// #define Log

#ifdef Log
  #include <nrf_log.h>
#endif

using namespace Pinetime::Controllers;

uint64_t SimpleWeatherService::ToUInt64(const uint8_t* data) {
  return data[0] + (data[1] << 8) + (data[2] << 16) + (data[3] << 24) + (static_cast<uint64_t>(data[4]) << 32) +
         (static_cast<uint64_t>(data[5]) << 40) + (static_cast<uint64_t>(data[6]) << 48) + (static_cast<uint64_t>(data[7]) << 56);
}

int16_t SimpleWeatherService::ToInt16(const uint8_t* data) {
  return data[0] + (data[1] << 8);
}

SimpleWeatherService::CurrentWeather SimpleWeatherService::CreateCurrentWeather(const uint8_t* dataBuffer) {
  char cityName[33]; // 32 char + \0 (end of string)
  memcpy(cityName, &dataBuffer[16], 32);
  cityName[32] = '\0';
  return SimpleWeatherService::CurrentWeather(ToUInt64(&dataBuffer[2]),
                                              ToInt16(&dataBuffer[10]),
                                              ToInt16(&dataBuffer[12]),
                                              ToInt16(&dataBuffer[14]),
                                              SimpleWeatherService::Icons {dataBuffer[16 + 32]},
                                              cityName);
}

SimpleWeatherService::Forecast SimpleWeatherService::CreateForecast(const uint8_t* dataBuffer) {
  auto timestamp = static_cast<uint64_t>(ToUInt64(&dataBuffer[2]));
  const uint8_t nbDaysInBuffer = dataBuffer[10];
  const uint8_t nbDays = std::min(SimpleWeatherService::MaxNbForecastDays, nbDaysInBuffer);
  std::array<SimpleWeatherService::Forecast::Day, SimpleWeatherService::MaxNbForecastDays> days;
  for (int i = 0; i < nbDays; i++) {
    days[i] = SimpleWeatherService::Forecast::Day {ToInt16(&dataBuffer[11 + (i * 5)]),
                                                   ToInt16(&dataBuffer[13 + (i * 5)]),
                                                   SimpleWeatherService::Icons {dataBuffer[15 + (i * 5)]}};
  }
  return SimpleWeatherService::Forecast {timestamp, nbDays, days};
}

Pinetime::Controllers::SimpleWeatherService::MessageType SimpleWeatherService::getMessageType(const uint8_t* data) {
  auto messageType = static_cast<MessageType>(*data);
  if (messageType > MessageType::Unknown) {
    return MessageType::Unknown;
  }
  return messageType;
}

int SimpleWeatherService::weatherCallback(uint16_t /*connHandle*/, uint16_t /*attrHandle*/, struct ble_gatt_access_ctxt* ctxt, void* arg) {
  return static_cast<Pinetime::Controllers::SimpleWeatherService*>(arg)->onCommand(ctxt);
}

SimpleWeatherService::SimpleWeatherService() {
}

void SimpleWeatherService::Init() {
  ble_gatts_count_cfg(serviceDefinition);
  ble_gatts_add_svcs(serviceDefinition);
}

int SimpleWeatherService::onCommand(struct ble_gatt_access_ctxt* ctxt) {
  const auto* buffer = ctxt->om;
  const auto* dataBuffer = buffer->om_data;

  switch (getMessageType(dataBuffer)) {
    case MessageType::CurrentWeather:
      if (getVersion(dataBuffer) == 0) {
        currentWeather = CreateCurrentWeather(dataBuffer);
#ifdef Log
        NRF_LOG_INFO("Current weather :\n\tTimestamp : %d\n\tTemperature:%d\n\tMin:%d\n\tMax:%d\n\tIcon:%d\n\tLocation:%s",
                     currentWeather->timestamp,
                     currentWeather->temperature,
                     currentWeather->minTemperature,
                     currentWeather->maxTemperature,
                     currentWeather->iconId,
                     currentWeather->location.data());
#endif
      }
      break;
    case MessageType::Forecast:
      if (getVersion(dataBuffer) == 0) {
        forecast = CreateForecast(dataBuffer);
#ifdef Log
        NRF_LOG_INFO("Forecast : Timestamp : %d", forecast->timestamp);
        for (int i = 0; i < 5; i++) {
          NRF_LOG_INFO("\t[%d] Min: %d - Max : %d - Icon : %d",
                       i,
                       forecast->days[i].minTemperature,
                       forecast->days[i].maxTemperature,
                       forecast->days[i].iconId);
        }
#endif
      }
      break;
    default:
      break;
  }
  return 0;
}

std::optional<SimpleWeatherService::CurrentWeather> SimpleWeatherService::Current() const {
  if (currentWeather) {
    auto currentTime = System::SystemTask::displayApp->dateTimeController.CurrentDateTime().time_since_epoch();
    auto weatherTpSecond = std::chrono::seconds {currentWeather->timestamp};
    auto weatherTp = std::chrono::duration_cast<std::chrono::seconds>(weatherTpSecond);
    auto delta = currentTime - weatherTp;

    if (delta < std::chrono::hours {24}) {
      return currentWeather;
    }
  }
  return {};
}

std::optional<SimpleWeatherService::Forecast> SimpleWeatherService::GetForecast() const {
  if (forecast) {
    auto currentTime = System::SystemTask::displayApp->dateTimeController.CurrentDateTime().time_since_epoch();
    auto weatherTpSecond = std::chrono::seconds {forecast->timestamp};
    auto weatherTp = std::chrono::duration_cast<std::chrono::seconds>(weatherTpSecond);
    auto delta = currentTime - weatherTp;

    if (delta < std::chrono::hours {24}) {
      return forecast;
    }
  }
  return {};
}

bool SimpleWeatherService::CurrentWeather::operator==(const SimpleWeatherService::CurrentWeather& other) const {
  return iconId == other.iconId && temperature == other.temperature && timestamp == other.timestamp &&
         maxTemperature == other.maxTemperature && minTemperature == other.minTemperature && location == other.location;
}

bool SimpleWeatherService::Forecast::Day::operator==(const SimpleWeatherService::Forecast::Day& other) const {
  return iconId == other.iconId && maxTemperature == other.maxTemperature && minTemperature == other.minTemperature;
}

bool SimpleWeatherService::Forecast::operator==(const SimpleWeatherService::Forecast& other) const {
  for (int i = 0; i < nbDays; i++) {
    if (days[i] != other.days[i]) {
      return false;
    }
  }
  return timestamp == other.timestamp && nbDays == other.nbDays;
}
