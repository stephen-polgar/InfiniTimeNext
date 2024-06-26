#include "Weather.h"
#include "systemtask/SystemTask.h"
#include "displayapp/screens/WeatherSymbols.h"
#include "displayapp/InfiniTimeTheme.h"

using namespace Pinetime::Applications::Screens;

Weather::Weather() : ScreenRefresh(Apps::Weather) {
}

void Weather::Load() {
  temperature = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(temperature, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_obj_set_style_local_text_font(temperature, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_42);
  lv_obj_align(temperature, nullptr, LV_ALIGN_CENTER, 0, -30);
  lv_obj_set_auto_realign(temperature, true);

  minTemperature = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(minTemperature, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::bg);
  lv_obj_align(minTemperature, temperature, LV_ALIGN_OUT_LEFT_MID, -10, 0);
  lv_obj_set_auto_realign(minTemperature, true);

  maxTemperature = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(maxTemperature, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::bg);
  lv_obj_align(maxTemperature, temperature, LV_ALIGN_OUT_RIGHT_MID, 10, 0);
  lv_obj_set_auto_realign(maxTemperature, true);

  condition = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(condition, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, Colors::lightGray);
  lv_obj_align(condition, temperature, LV_ALIGN_OUT_TOP_MID, 0, -10);
  lv_obj_set_auto_realign(condition, true);

  icon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(icon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_obj_set_style_local_text_font(icon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &fontawesome_weathericons);
  lv_obj_align(icon, condition, LV_ALIGN_OUT_TOP_MID, 0, 0);
  lv_obj_set_auto_realign(icon, true);

  forecast = lv_table_create(lv_scr_act(), nullptr);
  lv_table_set_col_cnt(forecast, Controllers::SimpleWeatherService::MaxNbForecastDays);
  lv_table_set_row_cnt(forecast, 4);
  // LV_TABLE_PART_CELL1: Default table style
  lv_obj_set_style_local_border_color(forecast, LV_TABLE_PART_CELL1, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_set_style_local_text_color(forecast, LV_TABLE_PART_CELL1, LV_STATE_DEFAULT, Colors::lightGray);
  // LV_TABLE_PART_CELL2: Condition icon
  lv_obj_set_style_local_border_color(forecast, LV_TABLE_PART_CELL2, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_set_style_local_text_color(forecast, LV_TABLE_PART_CELL2, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_obj_set_style_local_text_font(forecast, LV_TABLE_PART_CELL2, LV_STATE_DEFAULT, &fontawesome_weathericons);
  // LV_TABLE_PART_CELL3: Freezing
  lv_obj_set_style_local_border_color(forecast, LV_TABLE_PART_CELL3, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_set_style_local_text_color(forecast, LV_TABLE_PART_CELL3, LV_STATE_DEFAULT, Colors::blue);
  // LV_TABLE_PART_CELL4: Ice
  lv_obj_set_style_local_border_color(forecast, LV_TABLE_PART_CELL4, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_set_style_local_text_color(forecast, LV_TABLE_PART_CELL4, LV_STATE_DEFAULT, LV_COLOR_CYAN);
  // LV_TABLE_PART_CELL5: Normal
  lv_obj_set_style_local_border_color(forecast, LV_TABLE_PART_CELL5, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_set_style_local_text_color(forecast, LV_TABLE_PART_CELL5, LV_STATE_DEFAULT, Colors::orange);
  // LV_TABLE_PART_CELL6: Hot
  lv_obj_set_style_local_border_color(forecast, LV_TABLE_PART_CELL6, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_set_style_local_text_color(forecast, LV_TABLE_PART_CELL6, LV_STATE_DEFAULT, Colors::deepOrange);

  lv_obj_align(forecast, nullptr, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);

  for (uint8_t i = 0; i < Controllers::SimpleWeatherService::MaxNbForecastDays; i++) {
    lv_table_set_col_width(forecast, i, 48);
    lv_table_set_cell_type(forecast, 1, i, LV_TABLE_PART_CELL2);
    lv_table_set_cell_align(forecast, 0, i, LV_LABEL_ALIGN_CENTER);
    lv_table_set_cell_align(forecast, 1, i, LV_LABEL_ALIGN_CENTER);
    lv_table_set_cell_align(forecast, 2, i, LV_LABEL_ALIGN_CENTER);
    lv_table_set_cell_align(forecast, 3, i, LV_LABEL_ALIGN_CENTER);
  }
  Refresh();
  createRefreshTask(1000, LV_TASK_PRIO_MID);
  running = true;
}

lv_color_t Weather::temperatureColor(int16_t temperature) {
  if (temperature <= 0) { // freezing
    return Colors::blue;
  } else if (temperature <= 400) { // ice
    return LV_COLOR_CYAN;
  } else if (temperature >= 2700) { // hot
    return Colors::deepOrange;
  }
  return Colors::orange; // normal
}

uint8_t Weather::temperatureStyle(int16_t temperature) {
  if (temperature <= 0) { // freezing
    return LV_TABLE_PART_CELL3;
  } else if (temperature <= 400) { // ice
    return LV_TABLE_PART_CELL4;
  } else if (temperature >= 2700) { // hot
    return LV_TABLE_PART_CELL6;
  }
  return LV_TABLE_PART_CELL5; // normal
}

int16_t Weather::roundTemperature(int16_t temp) {
  return temp = temp / 100 + (temp % 100 >= 50 ? 1 : 0);
}

void Weather::Refresh() {
  currentWeather = System::SystemTask::displayApp->systemTask->nimbleController.weatherService.Current();
  if (!running || currentWeather.IsUpdated()) {
    auto optCurrentWeather = currentWeather.Get();
    if (optCurrentWeather) {
      int16_t temp = optCurrentWeather->temperature;
      int16_t minTemp = optCurrentWeather->minTemperature;
      int16_t maxTemp = optCurrentWeather->maxTemperature;
      lv_obj_set_style_local_text_color(temperature, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, temperatureColor(temp));
      char tempUnit;
#ifdef UnitFormat_Metric
      tempUnit = 'C';
#else
      tempUnit = 'F';
      temp = Controllers::SimpleWeatherService::CelsiusToFahrenheit(temp);
      minTemp = Controllers::SimpleWeatherService::CelsiusToFahrenheit(minTemp);
      maxTemp = Controllers::SimpleWeatherService::CelsiusToFahrenheit(maxTemp);
#endif
      lv_label_set_text(icon, Symbols::GetSymbol(optCurrentWeather->iconId));
      lv_label_set_text(condition, Symbols::GetCondition(optCurrentWeather->iconId));
      lv_label_set_text_fmt(temperature, "%d°%c", roundTemperature(temp), tempUnit);
      lv_label_set_text_fmt(minTemperature, "%d°", roundTemperature(minTemp));
      lv_label_set_text_fmt(maxTemperature, "%d°", roundTemperature(maxTemp));
    } else {
      lv_label_set_text_static(icon, "");
      lv_label_set_text_static(condition, "");
      lv_label_set_text_static(temperature, "---");
      lv_obj_set_style_local_text_color(temperature, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
      lv_label_set_text_static(minTemperature, "");
      lv_label_set_text_static(maxTemperature, "");
    }
  }

  currentForecast = System::SystemTask::displayApp->systemTask->nimbleController.weatherService.GetForecast();
  if (!running || currentForecast.IsUpdated()) {
    auto optCurrentForecast = currentForecast.Get();
    if (optCurrentForecast) {
      std::tm localTime = *std::localtime(reinterpret_cast<const time_t*>(&optCurrentForecast->timestamp));

      for (uint8_t i = 0; i < Controllers::SimpleWeatherService::MaxNbForecastDays; i++) {
        int16_t maxTemp = optCurrentForecast->days[i].maxTemperature;
        int16_t minTemp = optCurrentForecast->days[i].minTemperature;
        lv_table_set_cell_type(forecast, 2, i, temperatureStyle(maxTemp));
        lv_table_set_cell_type(forecast, 3, i, temperatureStyle(minTemp));
#ifndef UnitFormat_Metric
        maxTemp = Controllers::SimpleWeatherService::CelsiusToFahrenheit(maxTemp);
        minTemp = Controllers::SimpleWeatherService::CelsiusToFahrenheit(minTemp);
#endif
        uint8_t wday = localTime.tm_wday + i + 1;
        if (wday > 7) {
          wday -= 7;
        }
        maxTemp = roundTemperature(maxTemp);
        minTemp = roundTemperature(minTemp);
        const char* dayOfWeek = Controllers::DateTime::DayOfWeekShortToStringLow(static_cast<Controllers::DateTime::Days>(wday));
        lv_table_set_cell_value(forecast, 0, i, dayOfWeek);
        lv_table_set_cell_value(forecast, 1, i, Symbols::GetSymbol(optCurrentForecast->days[i].iconId));
        // Pad cells based on the largest number of digits on each column
        char maxPadding[3] = "  ";
        char minPadding[3] = "  ";
        int diff = snprintf(nullptr, 0, "%d", maxTemp) - snprintf(nullptr, 0, "%d", minTemp);
        if (diff <= 0) {
          maxPadding[-diff] = '\0';
          minPadding[0] = '\0';
        } else {
          maxPadding[0] = '\0';
          minPadding[diff] = '\0';
        }
        lv_table_set_cell_value_fmt(forecast, 2, i, "%s%d", maxPadding, maxTemp);
        lv_table_set_cell_value_fmt(forecast, 3, i, "%s%d", minPadding, minTemp);
      }
    } else {
      for (uint8_t i = 0; i < Controllers::SimpleWeatherService::MaxNbForecastDays; i++) {
        lv_table_set_cell_value(forecast, 0, i, "");
        lv_table_set_cell_value(forecast, 1, i, "");
        lv_table_set_cell_value(forecast, 2, i, "");
        lv_table_set_cell_value(forecast, 3, i, "");
        lv_table_set_cell_type(forecast, 2, i, LV_TABLE_PART_CELL1);
        lv_table_set_cell_type(forecast, 3, i, LV_TABLE_PART_CELL1);
      }
    }
  }
}
