#include "displayapp/widgets/PageIndicator.h"
#include "displayapp/InfiniTimeTheme.h"

using namespace Pinetime::Applications::Widgets;

void PageIndicator::Create(uint8_t nCurrentScreen) {
  static lv_point_t pageIndicatorBasePoints[2];
  pageIndicatorBasePoints[0].x = LV_HOR_RES - 1;
  pageIndicatorBasePoints[0].y = 0;
  pageIndicatorBasePoints[1].x = LV_HOR_RES - 1;
  pageIndicatorBasePoints[1].y = LV_VER_RES;

  pageIndicatorBase = lv_line_create(lv_scr_act(), NULL);
  lv_obj_set_style_local_line_width(pageIndicatorBase, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, 3);
  lv_obj_set_style_local_line_color(pageIndicatorBase, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, Colors::bgDark);
  lv_line_set_points(pageIndicatorBase, pageIndicatorBasePoints, 2);

  const lv_coord_t indicatorSize = LV_VER_RES / nScreens;
  const lv_coord_t indicatorPos = indicatorSize * nCurrentScreen;

  static lv_point_t pageIndicatorPoints[2];
  pageIndicatorPoints[0].x = LV_HOR_RES - 1;
  pageIndicatorPoints[0].y = indicatorPos;
  pageIndicatorPoints[1].x = LV_HOR_RES - 1;
  pageIndicatorPoints[1].y = indicatorPos + indicatorSize;

  lv_obj_t* pageIndicator = lv_line_create(pageIndicatorBase, NULL);
  lv_obj_set_style_local_line_width(pageIndicator, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, 3);
  lv_obj_set_style_local_line_color(pageIndicator, LV_LINE_PART_MAIN, LV_STATE_DEFAULT, Colors::lightGray);
  lv_line_set_points(pageIndicator, pageIndicatorPoints, 2);
}

void PageIndicator::Clean() {
  lv_obj_del(pageIndicatorBase);
}
