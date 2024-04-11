/*  Copyright (C) 2021  Adam Pigg

    This file is part of InfiniTime.

    InfiniTime is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    InfiniTime is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include "Screen.h"
#include "components/fs/FS.h"
#include "Symbols.h"
#include <string>


namespace Pinetime {
   namespace Applications {
    namespace Screens {
      class Navigation : public Screen {
      public:
        explicit Navigation();
        ~Navigation() override;
        void Load() override;
        bool UnLoad() override;
        void Refresh() override;
        static bool IsAvailable(Controllers::FS& filesystem);

      private:
        lv_obj_t* imgFlag;
        lv_obj_t* txtNarrative;
        lv_obj_t* txtManDist;
        lv_obj_t* barProgress;

        std::string flag;
        std::string narrative;
        std::string manDist;
        int progress;

        lv_task_t* taskRefresh;
      };
    }

    template <>
    struct AppTraits<Apps::Navigation> {
      static constexpr Apps app = Apps::Navigation;
      static constexpr const char* icon = Screens::Symbols::map;

      static Screens::Screen* Create() {
        return new Screens::Navigation();
      };
    };
  }
}
