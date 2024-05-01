#pragma once

#include "displayapp/apps/Apps.h"
#include "displayapp/TouchEvents.h"
#include <lvgl/lvgl.h>

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class Screen {
      public:
        enum class FullRefreshDirections : uint8_t { None, Up, Down, Left, Right, LeftAnim, RightAnim };
      
        virtual ~Screen();

        bool IsRunning() const {
          return running;
        }

        /** @return false if the button hasn't been handled by the app, true if it has been handled */
        virtual bool OnButtonPushed() {
          return false;
        }

        virtual void Load() {
        }

        virtual bool UnLoad();

        /** @return false if the event hasn't been handled by the app, true if it has been handled and will cancel lvgl tap*/
        virtual bool OnTouchEvent(TouchEvents /*event*/) {
          return false;
        }

        virtual bool OnTouchEvent(uint16_t /*x*/, uint16_t /*y*/) {
          return false;
        }

        bool operator==(const Screen& other) const;
        bool operator==(const Apps other) const;

        Apps Id;
        FullRefreshDirections direction = FullRefreshDirections::None;

        virtual void Refresh() {
        }

      protected:
        Screen(Apps Id = Apps::None);
        Screen(WatchFace Id);
        bool running = false;        
      };
    }
  }
}
