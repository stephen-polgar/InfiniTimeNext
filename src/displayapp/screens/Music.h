/*  Copyright (C) 2020 JF, Adam Pigg, Avamander

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

#include "ScreenRefresh.h"
#include <FreeRTOS.h>
#include <portmacro_cmsis.h>
#include <string>
#include "Symbols.h"

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class Music : public ScreenRefresh {
      public:
        Music();
        ~Music() override;
        bool OnTouchEvent(TouchEvents event) override;
        void Load() override;
        bool UnLoad() override;

      private:
        void Refresh() override;

        void onObjectEvent(lv_obj_t* obj, lv_event_t event);
        void updateLength();
        static void event_handler(lv_obj_t* obj, lv_event_t event);
        lv_obj_t* btnPrev;
        lv_obj_t* btnPlayPause;
        lv_obj_t* btnNext;
        lv_obj_t* btnVolDown;
        lv_obj_t* btnVolUp;
        lv_obj_t* txtArtist;
        lv_obj_t* txtTrack;
        lv_obj_t* txtPlayPause;

        lv_obj_t* imgDisc;
        lv_obj_t* imgDiscAnim;
        lv_obj_t* txtTrackDuration;

        lv_style_t btn_style;

        /** For the spinning disc animation */
        bool frameB;

        std::string artist;
        std::string album;
        std::string track;

        /** Total length in seconds */
        int totalLength;
        /** Current position in seconds */
        int currentPosition;
        /** Last time an animation update or timer was incremented */
        TickType_t lastIncrement;

        bool playing;

        /** Watchapp */
      };
    }

    template <>
    struct AppTraits<Apps::Music> {
      static constexpr Apps app = Apps::Music;
      static constexpr const char* icon = Screens::Symbols::music;

      static Screens::Screen* Create() {
        return new Screens::Music();
      };
    };
  }
}
