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
#pragma once

#include "displayapp/screens/Screen.h"
//#include <nrf_log.h>

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
#ifdef NRF_LOG_INFO
      void content(const char* msg = NULL);
#endif
      Screen* screenArray[N];
      uint8_t size = 0;
    };

#ifdef NRF_LOG_INFO
    template <uint8_t N>
    void ScreenStack<N>::content(const char* msg) {
      if (msg)
        NRF_LOG_INFO("ScreenStack:msg=%s", msg);
      uint8_t i = size;
      while (i--) {
        NRF_LOG_INFO("ScreenStack:content %d %d", screenArray[i], uint8_t(screenArray[i]->Id));
      }
    }
#endif

    template <uint8_t N>
    Screen* ScreenStack<N>::Pop() {
#ifdef NRF_LOG_INFO
      Screen* screen = size ? screenArray[--size] : NULL;
      NRF_LOG_INFO("ScreenStack:Pop %d %d size=%d", screen, (screen ? uint8_t(screen->Id) : 0), size);
      return screen;
#else
      return size ? screenArray[--size] : NULL;
#endif
    }

    template <uint8_t N>
    Screen* ScreenStack<N>::Get(Applications::Apps id) {
#ifdef NRF_LOG_INFO
      NRF_LOG_INFO("ScreenStack:Get %d", uint8_t(id));
      content();
#endif
      Screen* screen;
      uint8_t i = size;
      while (i--) {
        if (id == *screenArray[i]) {
          screen = screenArray[i];
          size--;
          while (i < size) {
            //  screenArray[i] = screenArray[++i];  compiler bug !  ++i === i++
            screenArray[i] = screenArray[i + 1];
            i++;
          }
#ifdef NRF_LOG_INFO
          content("get found");
#endif
          return screen;
        }
      }
#ifdef NRF_LOG_INFO
      NRF_LOG_INFO("get not found");
#endif
      return NULL;
    }

    template <uint8_t N>
    void ScreenStack<N>::Push(Screen* screen) {
#ifdef NRF_LOG_INFO
      NRF_LOG_INFO("ScreenStack:Push %d %d available=%d", screen, uint8_t(screen->Id), N - size);
      content();
#endif
      uint8_t i;
      for (i = 0; i < size; i++) {
        if (screenArray[i] == screen) {
#ifdef NRF_LOG_INFO
          NRF_LOG_ERROR("ScreenStack:Push the same %d %d", screen, uint8_t(screen->Id));
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
      content("push end");
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
