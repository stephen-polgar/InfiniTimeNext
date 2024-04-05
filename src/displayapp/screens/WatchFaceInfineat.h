#pragma once

#include "displayapp/screens/Screen.h"
#ifdef UseWatchFaceInfineat
  #include "components/fs/FS.h"
  #include "utility/DirtyValue.h"
  #include "components/settings/Settings.h"
  #include <array>
  #include <chrono>

namespace Pinetime {
  namespace Applications {
    namespace Screens {
      class WatchFaceInfineat : public Screen {
      public:
        WatchFaceInfineat();
        ~WatchFaceInfineat() override;
        void Load() override;
        bool UnLoad() override;
        bool OnTouchEvent(TouchEvents event) override;
        bool OnButtonPushed() override;
        void Refresh() override;
        static bool IsAvailable(Controllers::FS& filesystem);

      private:
        static constexpr uint8_t nLines = 9;
        static constexpr uint8_t nColors = 7; // must match number of colors in InfineatColors
        enum class colors { orange, blue, green, rainbow, gray, nordBlue, nordGreen };

        static constexpr lv_point_t linePoints[nLines][2] = {{{30, 25}, {68, -8}},
                                                             {{26, 167}, {43, 216}},
                                                             {{27, 40}, {27, 196}},
                                                             {{12, 182}, {65, 249}},
                                                             {{17, 99}, {17, 144}},
                                                             {{14, 81}, {40, 127}},
                                                             {{14, 163}, {40, 118}},
                                                             {{-20, 124}, {25, -11}},
                                                             {{-29, 89}, {27, 254}}};

        static constexpr lv_style_int_t lineWidths[nLines] = {18, 15, 14, 22, 20, 18, 18, 52, 48};
        static constexpr std::array<lv_color_t, nLines> orangeColors = {LV_COLOR_MAKE(0xfd, 0x87, 0x2b),
                                                                        LV_COLOR_MAKE(0xdb, 0x33, 0x16),
                                                                        LV_COLOR_MAKE(0x6f, 0x10, 0x00),
                                                                        LV_COLOR_MAKE(0xfd, 0x7a, 0x0a),
                                                                        LV_COLOR_MAKE(0xff, 0xff, 0xff),
                                                                        LV_COLOR_MAKE(0xff, 0xff, 0xff),
                                                                        LV_COLOR_MAKE(0xff, 0xff, 0xff),
                                                                        LV_COLOR_MAKE(0xe8, 0x51, 0x02),
                                                                        LV_COLOR_MAKE(0xea, 0x1c, 0x00)};
        static constexpr std::array<lv_color_t, nLines> blueColors = {LV_COLOR_MAKE(0xe7, 0xf8, 0xff),
                                                                      LV_COLOR_MAKE(0x22, 0x32, 0xd0),
                                                                      LV_COLOR_MAKE(0x18, 0x2a, 0x8b),
                                                                      LV_COLOR_MAKE(0xe7, 0xf8, 0xff),
                                                                      LV_COLOR_MAKE(0xff, 0xff, 0xff),
                                                                      LV_COLOR_MAKE(0xff, 0xff, 0xff),
                                                                      LV_COLOR_MAKE(0xff, 0xff, 0xff),
                                                                      LV_COLOR_MAKE(0x59, 0x91, 0xff),
                                                                      LV_COLOR_MAKE(0x16, 0x36, 0xff)};
        static constexpr std::array<lv_color_t, nLines> greenColors = {LV_COLOR_MAKE(0xb8, 0xff, 0x9b),
                                                                       LV_COLOR_MAKE(0x08, 0x86, 0x08),
                                                                       LV_COLOR_MAKE(0x00, 0x4a, 0x00),
                                                                       LV_COLOR_MAKE(0xb8, 0xff, 0x9b),
                                                                       LV_COLOR_MAKE(0xff, 0xff, 0xff),
                                                                       LV_COLOR_MAKE(0xff, 0xff, 0xff),
                                                                       LV_COLOR_MAKE(0xff, 0xff, 0xff),
                                                                       LV_COLOR_MAKE(0x62, 0xd5, 0x15),
                                                                       LV_COLOR_MAKE(0x00, 0x74, 0x00)};
        static constexpr std::array<lv_color_t, nLines> rainbowColors = {LV_COLOR_MAKE(0x2d, 0xa4, 0x00),
                                                                         LV_COLOR_MAKE(0xac, 0x09, 0xc4),
                                                                         LV_COLOR_MAKE(0xfe, 0x03, 0x03),
                                                                         LV_COLOR_MAKE(0x0d, 0x57, 0xff),
                                                                         LV_COLOR_MAKE(0xff, 0xff, 0xff),
                                                                         LV_COLOR_MAKE(0xff, 0xff, 0xff),
                                                                         LV_COLOR_MAKE(0xff, 0xff, 0xff),
                                                                         LV_COLOR_MAKE(0xe0, 0xb9, 0x00),
                                                                         LV_COLOR_MAKE(0xe8, 0x51, 0x02)};

        static constexpr std::array<lv_color_t, nLines> grayColors = {LV_COLOR_MAKE(0xee, 0xee, 0xee),
                                                                      LV_COLOR_MAKE(0x98, 0x95, 0x9b),
                                                                      LV_COLOR_MAKE(0x19, 0x19, 0x19),
                                                                      LV_COLOR_MAKE(0xee, 0xee, 0xee),
                                                                      LV_COLOR_MAKE(0xff, 0xff, 0xff),
                                                                      LV_COLOR_MAKE(0xff, 0xff, 0xff),
                                                                      LV_COLOR_MAKE(0xff, 0xff, 0xff),
                                                                      LV_COLOR_MAKE(0x91, 0x91, 0x91),
                                                                      LV_COLOR_MAKE(0x3a, 0x3a, 0x3a)};
        static constexpr std::array<lv_color_t, nLines> nordBlueColors = {LV_COLOR_MAKE(0xc3, 0xda, 0xf2),
                                                                          LV_COLOR_MAKE(0x4d, 0x78, 0xce),
                                                                          LV_COLOR_MAKE(0x15, 0x34, 0x51),
                                                                          LV_COLOR_MAKE(0xc3, 0xda, 0xf2),
                                                                          LV_COLOR_MAKE(0xff, 0xff, 0xff),
                                                                          LV_COLOR_MAKE(0xff, 0xff, 0xff),
                                                                          LV_COLOR_MAKE(0xff, 0xff, 0xff),
                                                                          LV_COLOR_MAKE(0x5d, 0x8a, 0xd2),
                                                                          LV_COLOR_MAKE(0x21, 0x51, 0x8a)};
        static constexpr std::array<lv_color_t, nLines> nordGreenColors = {LV_COLOR_MAKE(0xd5, 0xf0, 0xe9),
                                                                           LV_COLOR_MAKE(0x23, 0x83, 0x73),
                                                                           LV_COLOR_MAKE(0x1d, 0x41, 0x3f),
                                                                           LV_COLOR_MAKE(0xd5, 0xf0, 0xe9),
                                                                           LV_COLOR_MAKE(0xff, 0xff, 0xff),
                                                                           LV_COLOR_MAKE(0xff, 0xff, 0xff),
                                                                           LV_COLOR_MAKE(0xff, 0xff, 0xff),
                                                                           LV_COLOR_MAKE(0x2f, 0xb8, 0xa2),
                                                                           LV_COLOR_MAKE(0x11, 0x70, 0x5a)};

        static const std::array<lv_color_t, nLines>* returnColor(colors color) {
          if (color == colors::orange) {
            return &orangeColors;
          }
          if (color == colors::blue) {
            return &blueColors;
          }
          if (color == colors::green) {
            return &greenColors;
          }
          if (color == colors::rainbow) {
            return &rainbowColors;
          }
          if (color == colors::gray) {
            return &grayColors;
          }
          if (color == colors::nordBlue) {
            return &nordBlueColors;
          }
          return &nordGreenColors;
        }

        void closeMenu();
        void updateSelected(lv_obj_t* object);
        static void event_handler(lv_obj_t* obj, lv_event_t event);
        uint32_t savedTick;
        uint8_t chargingBatteryPercent;
        Utility::DirtyValue<uint8_t> batteryPercentRemaining;
        Utility::DirtyValue<bool> isCharging;
        Utility::DirtyValue<bool> bleState;
        Utility::DirtyValue<bool> bleRadioEnabled;
        Utility::DirtyValue<std::chrono::time_point<std::chrono::system_clock, std::chrono::minutes>> currentDateTime;
        Utility::DirtyValue<uint32_t> stepCount;
        Utility::DirtyValue<bool> notificationState;
        Utility::DirtyValue<std::chrono::time_point<std::chrono::system_clock, std::chrono::days>> currentDate;

        Controllers::Settings* settingsController;

        // Lines making up the side cover
        lv_obj_t* lineBattery;

        lv_point_t lineBatteryPoints[2];

        lv_obj_t* logoPine;

        lv_obj_t* timeContainer;
        lv_obj_t* labelHour;
        lv_obj_t* labelMinutes;
        lv_obj_t* labelTimeAmPm;
        lv_obj_t* dateContainer;
        lv_obj_t* labelDate;
        lv_obj_t* bleIcon;
        lv_obj_t* stepIcon;
        lv_obj_t* stepValue;
        lv_obj_t* notificationIcon;
        lv_obj_t* btnClose;
        lv_obj_t* btnNextColor;
        lv_obj_t* btnToggleCover;
        lv_obj_t* btnPrevColor;
        lv_obj_t* btnSettings;
        lv_obj_t* labelBtnSettings;
        lv_obj_t* lblToggle;

        lv_obj_t* lines[nLines];

        void setBatteryLevel(uint8_t batteryPercent);
        void toggleBatteryIndicatorColor(bool showSideCover);

        lv_font_t* font_teko = nullptr;
        lv_font_t* font_bebas = nullptr;
      };
    }

    template <>
    struct WatchFaceTraits<WatchFace::Infineat> {
      static constexpr WatchFace watchFace = WatchFace::Infineat;
      static constexpr const char* name = "Infineat face";

      static Screens::Screen* Create() {
        return new Screens::WatchFaceInfineat();
      };

      static bool IsAvailable(Controllers::FS& filesystem) {
        return Screens::WatchFaceInfineat::IsAvailable(filesystem);
      }
    };
  }
}
#endif