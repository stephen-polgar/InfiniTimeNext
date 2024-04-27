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
#include <functional>

using namespace Pinetime::Applications::Screens;

namespace Pinetime {
  namespace Utility {
    class ArrayTouchHandler {
    public:
      ArrayTouchHandler(uint8_t maxItems,
                        std::function<void(uint8_t indexBegin, uint8_t indexEnd, Screen::FullRefreshDirections)> load,
                        uint8_t arraySize = 0)
        : maxItems {maxItems}, load {load} {
        if (arraySize)
          SetArraySize(arraySize);
      }

      void SetArraySize(uint8_t arraySize) {
        this->arraySize = arraySize;
        size = arraySize >= maxItems ? maxItems : arraySize;
        index = 0;
      }

      bool OnTouchEvent(Applications::TouchEvents event) {
        switch (event) {
          case Applications::TouchEvents::SwipeUp:
            if (index + size < arraySize) {
              index += size;
              size = arraySize - index;
              if (size > maxItems)
                size = maxItems;
              load(index, index + size, Screen::FullRefreshDirections::Up);
              return true;
            }
            break;
          case Applications::TouchEvents::SwipeDown:
            if (index) {
              index = index > maxItems ? index - maxItems : 0;
              size = maxItems;
              load(index, index + size, Screen::FullRefreshDirections::Down);
              return true;
            }
            break;
          default:
            break;
        }
        return false;
      }

      void LoadCurrentPos() {
        // used in Screen::Load() and the direction is ignored
        load(index, index + size, Screen::FullRefreshDirections::None);
      }

    private:
      const uint8_t maxItems;
      uint8_t arraySize, size, index;
      std::function<void(uint8_t indexBegin, uint8_t indexEnd, Screen::FullRefreshDirections)> load;
    };
  }
}