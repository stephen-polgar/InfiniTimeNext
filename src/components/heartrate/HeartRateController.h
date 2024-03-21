#pragma once

#include "components/ble/HeartRateService.h"

namespace Pinetime {  
  namespace Controllers {
    class HeartRateController {
    public:
      enum class States : uint8_t { Stopped, NotEnoughData, NoTouch, Running };

      HeartRateController() = default;
      void Start();
      void Stop();
      void Update(States newState, uint8_t heartRate);
     
      States State() const {
        return state;
      }

      uint8_t HeartRate() const {
        return heartRate;
      }

     Controllers::HeartRateService service;

    private:
      States state = States::Stopped;
      uint8_t heartRate = 0;      
    };
  }
}