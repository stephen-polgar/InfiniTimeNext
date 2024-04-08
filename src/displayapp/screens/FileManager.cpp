
#include "displayapp/screens/FileManager.h"
#ifdef UseFileManager
  #include "systemtask/SystemTask.h"

  #define Log

  #ifdef Log
    #include <nrf_log.h>
  #endif

using namespace Pinetime::Applications::Screens;

FileManager::FileManager() : Screen(Apps::FileManager) {
  fs = &System::SystemTask::displayApp->filesystem;
}

void FileManager::Load() {
  running = true;
  
}

bool FileManager::UnLoad() {
  if (running) {
    running = false;
    lv_obj_clean(lv_scr_act());
  }
  return true;
}

FileManager::~FileManager() {
  UnLoad();
}

bool FileManager::OnTouchEvent(Applications::TouchEvents event) {
 
  return false;
}


#endif
