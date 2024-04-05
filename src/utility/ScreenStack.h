/*
 * This file is part of the InfinitimeNext distribution (https://github.com/stephen-polgar/InfiniTimeNext).
 * Copyright (c) 2024 Istvan Polgar.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#include "displayapp/screens/Screen.h"
#include <array>

//#define StackLog

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
      void Push(Screen*);
      void Reset();
      Screen* Top();
      bool Empty();
      void DeleteAll(Applications::Apps id);
      Screen* Get(Applications::Apps id);

    private:
      std::array<Screen*, N> elementArray;
      uint8_t stackPointer = 0;
    };

    template <uint8_t N>
    Screen* ScreenStack<N>::Pop() {
#ifdef StackLog
      Screen* screen = stackPointer ? elementArray[--stackPointer] : NULL;
      NRF_LOG_INFO("ScreenStack:Pop %d %d %d", screen, (screen ? uint8_t(screen->Id) : 0), stackPointer);
      return screen;
#else
      return stackPointer ? elementArray[--stackPointer] : NULL;
#endif
    }

    template <uint8_t N>
    Screen* ScreenStack<N>::Get(Applications::Apps id) {
      Screen* screen = NULL;
      if (stackPointer) {
        uint8_t i = stackPointer;
        while (--i) {
          if (id == elementArray[i]->Id) {
            screen = elementArray[i];
            stackPointer--;
            while (i < stackPointer) {
              elementArray[i] = elementArray[++i];
            }
            break;
          }
        }
      }
#ifdef StackLog
      NRF_LOG_INFO("ScreenStack:Get %d %d %d", screen, uint8_t(id), stackPointer);
#endif
      return screen;
    }

    template <uint8_t N>
    void ScreenStack<N>::Push(Screen* screen) {
      if (stackPointer == N) {
        delete elementArray[0];
        for (uint8_t i = 0; i < --stackPointer; i++) {
          elementArray[i] = elementArray[i + 1];
        }
      }
      elementArray[stackPointer++] = screen;
#ifdef StackLog
      NRF_LOG_INFO("ScreenStack:Push %d %d %d", screen, uint8_t(screen->Id), stackPointer);
#endif
    }

    template <uint8_t N>
    void ScreenStack<N>::Reset() {
#ifdef StackLog
      NRF_LOG_INFO("ScreenStack:Reset");
#endif
      while (stackPointer) {
        delete elementArray[--stackPointer];
      }
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
