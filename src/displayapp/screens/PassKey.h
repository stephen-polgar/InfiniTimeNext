#pragma once

#include "Screen.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class PassKey : public Screen {
      public:
        explicit PassKey(uint32_t key);
        void Load() override;

      private:
        uint32_t key;
      };
    }
  }
}
