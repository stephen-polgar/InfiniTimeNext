#pragma once

#include "Screen.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {   
      class ScreenTree : public Screen {
      public:
        ScreenTree();
        virtual ~ScreenTree();       
        bool OnTouchEvent(TouchEvents event) override;
        void Add(ScreenTree* screen);        
        void DeleteAll(); 
        Screen* GetCurrent();    
      private:      
        void Load() {}
        bool UnLoad() { return false; }
        ScreenTree* parent = NULL;
        ScreenTree* next = NULL;
        ScreenTree* current = NULL;        
      };
    }
  }
}
