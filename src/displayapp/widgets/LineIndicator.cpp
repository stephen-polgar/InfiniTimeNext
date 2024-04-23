#include "displayapp/widgets/LineIndicator.h"
#include "displayapp/InfiniTimeTheme.h"

using namespace Pinetime::Applications::Widgets;

LineIndicator::LineIndicator(uint8_t screens) : PageIndicator(screens) {
}

void LineIndicator::Load(uint8_t currentScreen) {
  static lv_point_t lineIndicatorBasePoints[2];
  if (!lineIndicatorBase) {
    lineIndicatorBase = lv_line_create(lv_scr_act(), NULL);
    lineIndicatorBasePoints[0].x = LV_HOR_RES - 1;
    lineIndicatorBasePoints[0].y = 0;
    lineIndicatorBasePoints[1].x = LV_HOR_RES - 1;
    lineIndicatorBasePoints[1].y = LV_VER_RES;
    lv_line_set_points(lineIndicatorBase, lineIndicatorBasePoints, 2);
    lv_obj_set_style_local_line_width(lineIndicatorBase, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, 3);
    lv_obj_set_style_local_line_color(lineIndicatorBase, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, Colors::bgDark);
  }

  const lv_coord_t indicatorSize = LV_VER_RES / screens;
  const lv_coord_t indicatorPos = indicatorSize * currentScreen;

  static lv_point_t lineIndicatorPoints[2];  
  lineIndicatorPoints[0].y = indicatorPos;  
  lineIndicatorPoints[1].y = indicatorPos + indicatorSize;
  
  if (!lineIndicator) {
    lineIndicator = lv_line_create(lineIndicatorBase, NULL);   
    lineIndicatorPoints[0].x = LV_HOR_RES - 1;
    lineIndicatorPoints[1].x = LV_HOR_RES - 1;
    lv_obj_set_style_local_line_width(lineIndicator, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, 3);
    lv_obj_set_style_local_line_color(lineIndicator, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, Colors::lightGray);
  }
   lv_line_set_points(lineIndicator, lineIndicatorPoints, 2);
}

void LineIndicator::UnLoad() {
  lv_obj_del(lineIndicatorBase);
  lineIndicatorBase = lineIndicator = NULL;
}
