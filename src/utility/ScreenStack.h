#include <array>
#include "displayapp/screens/Screen.h"

// #define StackLog

#ifdef StackLog
  #include <nrf_log.h>
#endif

using namespace Pinetime::Applications::Screens;

namespace Pinetime {
  namespace Utility {
    template <uint8_t N>
    class ScreenStack {
    public:
      Screen* Pop();
      bool Push(Screen*);
      void Reset();
      Screen* Top();
      bool Empty();
      void DeleteAll(Applications::Apps id);

    private:
      std::array<Screen*, N> elementArray;
      uint8_t stackPointer = 0;
    };

    template <uint8_t N>
    Screen* ScreenStack<N>::Pop() {
#ifdef StackLog
      Screen* screen = stackPointer ? elementArray[--stackPointer] : NULL;
      NRF_LOG_INFO("ScreenStack:Pop %d %d", screen, stackPointer);
      return screen;
#else
      return stackPointer ? elementArray[--stackPointer] : NULL;
#endif
    }

    template <uint8_t N>
    bool ScreenStack<N>::Push(Screen* screen) {
      bool ok = stackPointer < elementArray.size();
      if (ok) {
        elementArray[stackPointer++] = screen;
      }
#ifdef StackLog
      NRF_LOG_INFO("ScreenStack:Push %d %d", screen, stackPointer);
#endif
      return ok;
    }

    template <uint8_t N>
    void ScreenStack<N>::Reset() {
#ifdef StackLog
      NRF_LOG_INFO("ScreenStack:Reset");
#endif
      for (uint8_t i = 0; i < stackPointer; i++) {
        delete elementArray[i];
      }
      stackPointer = 0;
    }

    template <uint8_t N>
    void ScreenStack<N>::DeleteAll(Applications::Apps id) {
      bool ready = false;
      uint8_t i = 0;
      while (!ready) {
        while (i < stackPointer) {
          if (id == elementArray[i]->Id) {
            delete elementArray[i];
            break;
          } else
            i++;
        }
        if (i == stackPointer)
          ready = true;
        else {
          stackPointer--;
          for (uint8_t i2 = i; i2 < stackPointer; i2++) {
            elementArray[i2] = elementArray[i2 + 1];
          }
        }
      }
    }

    template <uint8_t N>
    bool ScreenStack<N>::Empty() {
      return !stackPointer;
    }

    template <uint8_t N>
    Screen* ScreenStack<N>::Top() {
      return stackPointer ? elementArray[stackPointer - 1] : NULL;
    }
  }
}
