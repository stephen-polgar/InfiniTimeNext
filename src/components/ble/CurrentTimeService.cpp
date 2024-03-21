#include "components/ble/CurrentTimeService.h"
#include "components/datetime/DateTimeController.h"
#include "systemtask/SystemTask.h"

using namespace Pinetime::Controllers;

constexpr ble_uuid16_t CurrentTimeService::ctsUuid;
constexpr ble_uuid16_t CurrentTimeService::ctsCtChrUuid;
constexpr ble_uuid16_t CurrentTimeService::ctsLtChrUuid;

int CTSCallback(uint16_t /*conn_handle*/, uint16_t /*attr_handle*/, struct ble_gatt_access_ctxt* ctxt, void* arg) {
  auto cts = static_cast<CurrentTimeService*>(arg);

  return cts->OnCurrentTimeServiceAccessed(ctxt);
}

int CurrentTimeService::OnCurrentTimeServiceAccessed(struct ble_gatt_access_ctxt* ctxt) {
  switch (ble_uuid_u16(ctxt->chr->uuid)) {
    case ctsCurrentTimeCharId:
      return OnCurrentTimeAccessed(ctxt);
    case ctsLocalTimeCharId:
      return OnLocalTimeAccessed(ctxt);
  }
  return -1; // Unknown characteristic
}

void CurrentTimeService::Init() {
  int res;
  res = ble_gatts_count_cfg(serviceDefinition);
  ASSERT(res == 0);

  res = ble_gatts_add_svcs(serviceDefinition);

  ASSERT(res == 0);
}

int CurrentTimeService::OnCurrentTimeAccessed(struct ble_gatt_access_ctxt* ctxt) {
  if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
    CtsCurrentTimeData result;
    int res = os_mbuf_copydata(ctxt->om, 0, sizeof(CtsCurrentTimeData), &result);
    if (res < 0) {
      return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }

    uint16_t year = ((uint16_t) result.year_MSO << 8) + result.year_LSO;

    System::SystemTask::displayApp->dateTimeController
      .SetTime(year, result.month, result.dayofmonth, result.hour, result.minute, result.second);

  } else if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
    auto* dateTimeController = &System::SystemTask::displayApp->dateTimeController;
    CtsCurrentTimeData currentDateTime;
    currentDateTime.year_LSO = dateTimeController->Year() & 0xff;
    currentDateTime.year_MSO = (dateTimeController->Year() >> 8) & 0xff;
    currentDateTime.month = static_cast<u_int8_t>(dateTimeController->Month());
    currentDateTime.dayofmonth = dateTimeController->Day();
    currentDateTime.hour = dateTimeController->Hours();
    currentDateTime.minute = dateTimeController->Minutes();
    currentDateTime.second = dateTimeController->Seconds();
    currentDateTime.fractions256 = 0;

    int res = os_mbuf_append(ctxt->om, &currentDateTime, sizeof(CtsCurrentTimeData));
    return (res == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }

  return 0;
}

int CurrentTimeService::OnLocalTimeAccessed(struct ble_gatt_access_ctxt* ctxt) {
  if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
    CtsLocalTimeData result;
    int res = os_mbuf_copydata(ctxt->om, 0, sizeof(CtsLocalTimeData), &result);

    if (res < 0) {
      return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }
    System::SystemTask::displayApp->dateTimeController.SetTimeZone(result.timezone, result.dst);

  } else if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
    CtsLocalTimeData currentTimezone;
    currentTimezone.timezone = System::SystemTask::displayApp->dateTimeController.TzOffset();
    currentTimezone.dst = System::SystemTask::displayApp->dateTimeController.DstOffset();

    int res = os_mbuf_append(ctxt->om, &currentTimezone, sizeof(currentTimezone));
    return (res == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }

  return 0;
}

CurrentTimeService::CurrentTimeService()
  : characteristicDefinition {{.uuid = &ctsLtChrUuid.u,
                               .access_cb = CTSCallback,
                               .arg = this,
                               .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ},

                              {.uuid = &ctsCtChrUuid.u,
                               .access_cb = CTSCallback,
                               .arg = this,
                               .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ},
                              {0}},
    serviceDefinition {
      {/* Device Information Service */
       .type = BLE_GATT_SVC_TYPE_PRIMARY,
       .uuid = &ctsUuid.u,
       .characteristics = characteristicDefinition},
      {0},
    } {
}
