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

#include "ScreenTree.h"
#include "systemtask/SystemTask.h"

// #include <nrf_log.h>

using namespace Pinetime::Applications;

ScreenTree::ScreenTree(Widgets::PageIndicator* pageIndicator) {
  this->pageIndicator = pageIndicator;
#ifdef NRF_LOG_INFO
  NRF_LOG_INFO("ScreenTree new=%d", this);
#endif
}

void ScreenTree::Add(ScreenTree* screen) {
  ScreenTree* tree = this;
  while (tree->next) {
    tree = tree->next;
  }
  screen->parent = tree;
  tree->next = screen;
}

Screen* ScreenTree::GetCurrent() {
  if (!current)
    current = this;
  return current;
}

void ScreenTree::Load() {
  if (pageIndicator) {
    ScreenTree* tree = this;
    uint8_t currentScreen = 0, screens = 1;
    while (tree->parent) {
      currentScreen++;
      tree = tree->parent;
    }
    while (tree->next) {
      screens++;
      tree = tree->next;
    }
#ifdef NRF_LOG_INFO
    NRF_LOG_INFO("ScreenTree:Load screens=%d, currentScreen=%d", screens, currentScreen);
#endif
    pageIndicator->Create(screens, currentScreen);
  }
}

ScreenTree::~ScreenTree() {
#ifdef NRF_LOG_INFO
  NRF_LOG_INFO("ScreenTree del=%d", this);
#endif
  if (parent)
    parent->next = next;
  if (next)
    next->parent = parent;
}

void ScreenTree::DeleteAll() {
  while (next) {
    delete next;
  }
  while (parent) {
    delete parent;
  }
  delete this;
}

bool ScreenTree::OnTouchEvent(TouchEvents event) {
  switch (event) {
    case TouchEvents::SwipeDown:
      if (current->parent) {
        current->UnLoad();
        System::SystemTask::displayApp->SetFullRefresh(Screen::FullRefreshDirections::Down);
        current = current->parent;
        current->Load();
        return true;
      }
      return false;
    case TouchEvents::SwipeUp:
      if (current->next) {
        current->UnLoad();
        System::SystemTask::displayApp->SetFullRefresh(Screen::FullRefreshDirections::Up);
        current = current->next;
        current->Load();
        return true;
      }
      break;
    default:
      break;
  }
  return event != TouchEvents::Tap;
}
