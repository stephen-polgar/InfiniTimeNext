#pragma once

#include "Screen.h"
#include "systemtask/SystemTask.h"
#include <array>
#include <functional>

namespace Pinetime {
  namespace Applications {
    namespace Screens {

      enum class ScreenListModes : uint8_t { UpDown, RightLeft, LongPress };

      template <uint8_t N>
      class ScreenList : public Screen {
      public:
        ScreenList(const std::array<std::function<std::unique_ptr<Screen>()>, N>&& screens,
                   uint8_t initScreen = 0,
                   ScreenListModes mode = ScreenListModes::UpDown)
          : screens {std::move(screens)}, screenIndex {initScreen}, mode {mode}, current {screens[initScreen]()} {
        }

        void Load() override {
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

        ScreenList(const ScreenList&) = delete;
        ScreenList& operator=(const ScreenList&) = delete;
        ScreenList(ScreenList&&) = delete;
        ScreenList& operator=(ScreenList&&) = delete;

        ~ScreenList() {
          UnLoad();
        }

        bool OnTouchEvent(TouchEvents event) override {
          if (mode == ScreenListModes::UpDown) {
            switch (event) {
              case TouchEvents::SwipeDown:
                if (screenIndex > 0) {
                  current.reset();
                  System::SystemTask::displayApp->SetFullRefresh(Screen::FullRefreshDirections::Down);
                  current = screens[--screenIndex]();
                  current->Load();
                  return true;
                } else {
                  return false;
                }
              case TouchEvents::SwipeUp:
                if (screenIndex < screens.size() - 1) {
                  current.reset();
                  System::SystemTask::displayApp->SetFullRefresh(Screen::FullRefreshDirections::Up);
                  current = screens[++screenIndex]();
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
                  current.reset();
                  System::SystemTask::displayApp->SetFullRefresh(Screen::FullRefreshDirections::None);
                  current = screens[--screenIndex]();
                  current->Load();
                  return true;
                } else {
                  return false;
                }

              case TouchEvents::SwipeLeft:
                if (screenIndex < screens.size() - 1) {
                  current.reset();
                  System::SystemTask::displayApp->SetFullRefresh(Screen::FullRefreshDirections::None);
                  current = screens[++screenIndex]();
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
            current.reset();
            System::SystemTask::displayApp->SetFullRefresh(Screen::FullRefreshDirections::None);
            current = screens[screenIndex]();
            current->Load();
            return true;
          }
          return false;
        }

      private:
        const std::array<std::function<std::unique_ptr<Screen>()>, N> screens;
        ScreenListModes mode;
        uint8_t screenIndex;
        std::unique_ptr<Screen> current;
      };
    }
  }
}
