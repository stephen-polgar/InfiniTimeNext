#include "SystemInfo.h"
#include "Version.h"
#include "BootloaderVersion.h"
#include "displayapp/InfiniTimeTheme.h"

using namespace Pinetime::Applications::Screens;

int SystemInfo::mallocFailedCount, SystemInfo::stackOverflowCount;

SystemInfo::TasksScreen::TasksScreen(uint8_t screenID, Widgets::PageIndicator& pageIndicator) : Label(screenID, &pageIndicator) {
}

void SystemInfo::TasksScreen::Load() {
  Label::Load();
  lv_obj_t* infoTask = lv_table_create(lv_scr_act(), nullptr);
  lv_table_set_col_cnt(infoTask, 4);
  lv_table_set_row_cnt(infoTask, maxTaskCount + 1);
  lv_obj_set_style_local_pad_all(infoTask, LV_TABLE_PART_CELL1, LV_STATE_DEFAULT, 0);
  lv_obj_set_style_local_border_color(infoTask, LV_TABLE_PART_CELL1, LV_STATE_DEFAULT, Colors::lightGray);

  lv_table_set_cell_value(infoTask, 0, 0, "#");
  lv_table_set_col_width(infoTask, 0, 30);
  lv_table_set_cell_value(infoTask, 0, 1, "S"); // State
  lv_table_set_col_width(infoTask, 1, 30);
  lv_table_set_cell_value(infoTask, 0, 2, "Task");
  lv_table_set_col_width(infoTask, 2, 80);
  lv_table_set_cell_value(infoTask, 0, 3, "Free");
  lv_table_set_col_width(infoTask, 3, 90);

  auto nb = uxTaskGetSystemState(tasksStatus, maxTaskCount, nullptr);
  std::sort(tasksStatus, tasksStatus + nb, sortById);
  for (uint8_t i = 0; i < nb && i < maxTaskCount; i++) {
    char buffer[11] = {0};

    snprintf(buffer, sizeof(buffer), "%lu", tasksStatus[i].xTaskNumber);
    lv_table_set_cell_value(infoTask, i + 1, 0, buffer);
    switch (tasksStatus[i].eCurrentState) {
      case eReady:
      case eRunning:
        buffer[0] = 'R';
        break;
      case eBlocked:
        buffer[0] = 'B';
        break;
      case eSuspended:
        buffer[0] = 'S';
        break;
      case eDeleted:
        buffer[0] = 'D';
        break;
      default:
        buffer[0] = 'I'; // Invalid
        break;
    }
    buffer[1] = '\0';
    lv_table_set_cell_value(infoTask, i + 1, 1, buffer);
    lv_table_set_cell_value(infoTask, i + 1, 2, tasksStatus[i].pcTaskName);
    if (tasksStatus[i].usStackHighWaterMark < 20) {
      snprintf(buffer, sizeof(buffer), "%" PRIu16 " low", tasksStatus[i].usStackHighWaterMark);
    } else {
      snprintf(buffer, sizeof(buffer), "%" PRIu16, tasksStatus[i].usStackHighWaterMark);
    }
    lv_table_set_cell_value(infoTask, i + 1, 3, buffer);
  }
}

void SystemInfo::HardverScreen::Load() {
#ifndef TARGET_DEVICE_NAME
  #define TARGET_DEVICE_NAME "UNKNOWN"
#endif
  Label::Load();
  lv_obj_t* label = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_recolor(label, true);
  lv_label_set_text_fmt(label,
                        "#808080 Uptime#\n %02lud %02lu:%02lu:%02lu\n"
                        "#808080 Last reset# %s\n"
                        "#808080 Accel.# %s\n"
                        "#808080 Touch.# %x.%x.%x\n"
                        "#808080 Model# %s\n"
                        "#808080 BLE MAC#\n"
                        " %02x:%02x:%02x:%02x:%02x:%02x",
                        uptimeDays,
                        uptimeHours,
                        uptimeMinutes,
                        uptimeSeconds,
                        resetReason,
                        toString(System::SystemTask::displayApp->motionController.DeviceType()),
                        System::SystemTask::displayApp->touchPanel.GetChipId(),
                        System::SystemTask::displayApp->touchPanel.GetVendorId(),
                        System::SystemTask::displayApp->touchPanel.GetFwVersion(),
                        TARGET_DEVICE_NAME,
                        bleAddr[5],
                        bleAddr[4],
                        bleAddr[3],
                        bleAddr[2],
                        bleAddr[1],
                        bleAddr[0]);
  lv_obj_align(label, lv_scr_act(), LV_ALIGN_CENTER, 0, 0);
}

void SystemInfo::FirmwareScreen::Load() {
  Label::Load();
  lv_obj_t* label = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_recolor(label, true);
  lv_label_set_text_fmt(label,
                        "#FFFF00 InfiniTimeNext#\n\n"
                        "#808080 Custom V.# %ld.%ld.%ld\n"
                        "#808080 Short Ref# %s\n"
                        "#808080 Build date#\n"
                        "%s\n"
                        "%s\n\n"
                        "#808080 Bootloader# %s",
                        Pinetime::Version::Major(),
                        Pinetime::Version::Minor(),
                        Pinetime::Version::Patch(),
                        Pinetime::Version::GitCommitHash(),
                        __DATE__,
                        __TIME__,
                        Pinetime::BootloaderVersion::VersionString());
  lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(label, lv_scr_act(), LV_ALIGN_CENTER, 0, 0);
}

SystemInfo::SystemInfo() : Screen(Apps::SysInfo) {
  screens = new MemoryInfo(0, pageIndicator);
  screens->Add(new TasksScreen(1, pageIndicator));
  screens->Add(new HardverScreen(2, pageIndicator));
  screens->Add(new FirmwareScreen(3, pageIndicator));
  screens->Add(new LicenseScreen(4, pageIndicator));
}

void SystemInfo::Load() {
  screens->GetCurrent()->Load();
  running = true;
}

bool SystemInfo::UnLoad() {
  if (running) {
    running = false;
    screens->GetCurrent()->UnLoad();
  }
  return true;
}

SystemInfo::~SystemInfo() {
  screens->DeleteAll();
}

bool SystemInfo::OnTouchEvent(Applications::TouchEvents event) {
  return screens->OnTouchEvent(event);
}

const char* SystemInfo::toString(const Controllers::MotionController::DeviceTypes deviceType) {
  switch (deviceType) {
    case Controllers::MotionController::DeviceTypes::BMA421:
      return "BMA421";
    case Controllers::MotionController::DeviceTypes::BMA425:
      return "BMA425";
    case Controllers::MotionController::DeviceTypes::Unknown:
      return "???";
  }
  return "???";
}

SystemInfo::MemoryInfo::MemoryInfo(uint8_t screenID, Widgets::PageIndicator& pageIndicator) : Label(screenID, &pageIndicator) {
}

void SystemInfo::MemoryInfo::Load() {
  Label::Load();
  lv_obj_t* label = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_recolor(label, true);
  lv_label_set_text_fmt(label,
                        "\t#808080 Memory heap#\n"
                        " #808080 Free# %d\n"
                        " #808080 Min free# %d\n"
                        " #808080 Alloc err# %d\n"
                        " #808080 Ovrfl err# %d\n",
                        xPortGetFreeHeapSize(),
                        xPortGetMinimumEverFreeHeapSize(),
                        mallocFailedCount,
                        stackOverflowCount);
  lv_obj_align(label, lv_scr_act(), LV_ALIGN_CENTER, 0, 0);
}

bool SystemInfo::sortById(const TaskStatus_t& lhs, const TaskStatus_t& rhs) {
  return lhs.xTaskNumber < rhs.xTaskNumber;
}

void SystemInfo::LicenseScreen::Load() {
  Label::Load();
  lv_obj_t* label = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_recolor(label, true);
  lv_label_set_text_static(label,
                           "GNU General Public\n"
                           "License v3\n\n"
                           "#FFFF00 https://github.com/\n"
                           "#FFFF00 stephen-polgar/\n"
                           "#FFFF00 InfiniTimeNext");
  lv_label_set_align(label, LV_LABEL_ALIGN_CENTER);
  lv_obj_align(label, lv_scr_act(), LV_ALIGN_CENTER, 0, 0);
}
