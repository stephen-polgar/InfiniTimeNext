#pragma once

#include "Screen.h"
#include <array>
#include "systemtask/SystemTask.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      enum class ScreenListModes : uint8_t { UpDown, RightLeft, LongPress };

      template <uint8_t N>
      class ScreenList : public Screen {
      public:
        ScreenList(ScreenListModes mode = ScreenListModes::UpDown) : mode {mode} {
        }

        void Add(Screen* screen) {
          screens[screenIndex++] = screen;
        }

        void Load() override {
          if (!current) {
            current = screens[screenIndex = 0];
          }
          running = true;
          current->Load();
        }

        bool UnLoad() override {
          if (running) {
            running = false;
            current->UnLoad();
          }
          return true;
        }

        ~ScreenList() {
          uint8_t i = screens.size();
          while (i--)
            delete screens[i];
        }

        bool OnTouchEvent(TouchEvents event) override {
          if (!running)
            return false;
          if (mode == ScreenListModes::UpDown) {
            switch (event) {
              case TouchEvents::SwipeDown:
                if (screenIndex > 0) {
                  current->UnLoad();
                  System::SystemTask::displayApp->SetFullRefresh(Screen::FullRefreshDirections::Down);
                  current = screens[--screenIndex];
                  current->Load();
                  return true;
                } else {
                  return false;
                }
              case TouchEvents::SwipeUp:
                if (screenIndex < screens.size() - 1) {
                  current->UnLoad();
                  System::SystemTask::displayApp->SetFullRefresh(Screen::FullRefreshDirections::Up);
                  current = screens[++screenIndex];
                  current->Load();
                }
                return true;
              default:
                return false;
            }
          } else if (mode == ScreenListModes::RightLeft) {
            switch (event) {
              case TouchEvents::SwipeRight:
                if (screenIndex > 0) {
                  current->UnLoad();
                  System::SystemTask::displayApp->SetFullRefresh(Screen::FullRefreshDirections::None);
                  current = screens[--screenIndex];
                  current->Load();
                  return true;
                } else {
                  return false;
                }
              case TouchEvents::SwipeLeft:
                if (screenIndex < screens.size() - 1) {
                  current->UnLoad();
                  System::SystemTask::displayApp->SetFullRefresh(Screen::FullRefreshDirections::None);
                  current = screens[++screenIndex];
                  current->Load();
                }
                return true;
              default:
                return false;
            }
          } else if (event == TouchEvents::LongTap) {
            if (screenIndex < screens.size() - 1) {
              screenIndex++;
            } else {
              screenIndex = 0;
            }
            current->UnLoad();
            System::SystemTask::displayApp->SetFullRefresh(Screen::FullRefreshDirections::None);
            current = screens[screenIndex];
            current->Load();
            return true;
          }
          return false;
        }

      private:
        std::array<Screen*, N> screens;
        ScreenListModes mode;
        uint8_t screenIndex = 0;
        Screen* current = NULL;
      };
    }
  }
}
