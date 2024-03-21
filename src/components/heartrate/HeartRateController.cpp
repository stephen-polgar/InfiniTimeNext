#include "components/heartrate/HeartRateController.h"
#include "heartratetask/HeartRateTask.h"
#include "systemtask/SystemTask.h"

using namespace Pinetime::Controllers;

void HeartRateController::Update(HeartRateController::States newState, uint8_t heartRate) {
  this->state = newState;
  if (this->heartRate != heartRate) {
    this->heartRate = heartRate;
    service.OnNewHeartRateValue(heartRate);
  }
}

void HeartRateController::Start() {
  state = States::NotEnoughData;
  System::SystemTask::displayApp->systemTask->heartRateTask.PushMessage(Applications::HeartRateTask::Messages::StartMeasurement);
}

void HeartRateController::Stop() {
  state = States::Stopped;
  System::SystemTask::displayApp->systemTask->heartRateTask.PushMessage(Applications::HeartRateTask::Messages::StopMeasurement);
}
