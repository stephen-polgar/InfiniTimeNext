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
// #include <nrf_log.h>

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
      std::array<Screen*, N> screenArray;
      uint8_t size = 0;
    };

    template <uint8_t N>
    Screen* ScreenStack<N>::Pop() {
#ifdef NRF_LOG_INFO
      Screen* screen = size ? screenArray[--size] : NULL;
      NRF_LOG_INFO("ScreenStack:Pop %d %d %d", screen, (screen ? uint8_t(screen->Id) : 0), size);
      return screen;
#else
      return size ? screenArray[--size] : NULL;
#endif
    }

    template <uint8_t N>
    Screen* ScreenStack<N>::Get(Applications::Apps id) {
      Screen* screen = NULL;
      uint8_t i = size;
      while (i--) {
        if (id == screenArray[i]->Id) {
          screen = screenArray[i];
          size--;
          while (i < size) {
            screenArray[i] = screenArray[++i];
          }
          break;
        }
      }
#ifdef NRF_LOG_INFO
      NRF_LOG_INFO("ScreenStack:Get %d %d %d", screen, uint8_t(id), size);
#endif
      return screen;
    }

    template <uint8_t N>
    void ScreenStack<N>::Push(Screen* screen) {
      uint8_t i;
      for (i = 0; i < size; i++) {
        if (screenArray[i] == screen) {
#ifdef StackLog
          NRF_LOG_INFO("ScreenStack:Push remove %d %d %d", screenArray[0], uint8_t(screenArray[0]->Id), size);
#endif
          size--;
          for (uint8_t i2 = i; i2 < size; i2++) {
            screenArray[i2] = screenArray[i2 + 1];
          }
        }
      }
      if (size == N) {
#ifdef NRF_LOG_INFO
        NRF_LOG_INFO("ScreenStack:Push delete %d %d %d", screenArray[0], uint8_t(screenArray[0]->Id), size);
#endif
        delete screenArray[0];
        size--;
        for (i = 0; i < size; i++) {
          screenArray[i] = screenArray[i + 1];
        }
      }
      screenArray[size++] = screen;
#ifdef NRF_LOG_INFO
      NRF_LOG_INFO("ScreenStack:Push %d %d %d", screen, uint8_t(screen->Id), size);
#endif
    }

    template <uint8_t N>
    void ScreenStack<N>::Reset() {
#ifdef NRF_LOG_INFO
      NRF_LOG_INFO("ScreenStack:Reset");
#endif
      while (size) {
        delete screenArray[--size];
      }
    }

    template <uint8_t N>
    void ScreenStack<N>::DeleteAll(Applications::Apps id) {
      for (uint8_t i = 0; i < size; i++) {
        if (id == screenArray[i]->Id) {
          delete screenArray[i];
          size--;
          for (uint8_t i2 = i; i2 < size; i2++) {
            screenArray[i2] = screenArray[i2 + 1];
          }
        }
      }
    }

    template <uint8_t N>
    bool ScreenStack<N>::Empty() {
      return !size;
    }

    template <uint8_t N>
    Screen* ScreenStack<N>::Top() {
      return size ? screenArray[size - 1] : NULL;
    }
  }
}
