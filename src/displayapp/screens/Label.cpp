#include "Label.h"

// #define Log

#ifdef Log
  #include <nrf_log.h>
#endif

using namespace Pinetime::Applications::Screens;

// Widgets::PageIndicator Label::pageIndicator;

Label::Label(uint8_t screenID, uint8_t numScreens, Widgets::PageIndicator& pageIndicator)
  : screenID {screenID}, numScreens {numScreens}, pageIndicator {pageIndicator} {
#ifdef Log
  NRF_LOG_INFO("Label created %d", this);
#endif
}

void Label::Load() {
#ifdef Log
  NRF_LOG_INFO("Label:Load %d", this);
#endif
  running = true;
  pageIndicator.Create(screenID, numScreens);
}

bool Label::UnLoad() {
  if (running) {
#ifdef Log
    NRF_LOG_INFO("Label:UnLoad %d", this);
#endif
    running = false;
    lv_obj_clean(lv_scr_act());
  }
  return true;
}

Label::~Label() {
#ifdef Log
  NRF_LOG_INFO("Label deleted %d", this);
#endif
  UnLoad();
}
