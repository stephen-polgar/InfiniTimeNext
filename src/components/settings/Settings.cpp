#include "components/settings/Settings.h"

using namespace Pinetime::Controllers;

Settings::Settings(Pinetime::Controllers::FS& fs) : fs {fs} {
}

void Settings::Init() {
  lfs_file_t settingsFile;
  if (fs.FileOpen(&settingsFile, file, LFS_O_RDONLY) == LFS_ERR_OK) {
    SettingsData bufferSettings;
    if (sizeof(settings) == fs.FileRead(&settingsFile, reinterpret_cast<uint8_t*>(&bufferSettings), sizeof(settings))) {
      fs.FileClose(&settingsFile);
      if (bufferSettings.version == settingsVersion) {
        settings = bufferSettings;
      }
    }
  }
}

void Settings::SaveSettings() {
  if (settingsChanged) {
    lfs_file_t settingsFile;
    if (fs.FileOpen(&settingsFile, file, LFS_O_WRONLY | LFS_O_CREAT) == LFS_ERR_OK) {
      if (sizeof(settings) == fs.FileWrite(&settingsFile, reinterpret_cast<uint8_t*>(&settings), sizeof(settings))) {
        settingsChanged = false;
      }
      fs.FileClose(&settingsFile);
    }
  }
}
