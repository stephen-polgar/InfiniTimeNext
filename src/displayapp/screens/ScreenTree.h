#pragma once

#include "Screen.h"
#include "displayapp/widgets/PageIndicator.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class ScreenTree : public Screen {
      public:
        ScreenTree(Widgets::PageIndicator* pageIndicator);
        virtual ~ScreenTree();
        bool OnTouchEvent(TouchEvents event) override;
        void Add(ScreenTree* screen);
        void DeleteAll();
        Screen* GetCurrent();

      protected:
        void Load() override;

      private:
        bool UnLoad() override {
          return false;
        }

        Widgets::PageIndicator* pageIndicator;
        ScreenTree* parent = NULL;
        ScreenTree* next = NULL;
        ScreenTree* current = NULL;
      };
    }
  }
}
