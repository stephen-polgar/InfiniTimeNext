#include "displayapp/screens/FileView.h"
#ifdef UseFileManager
  #include "systemtask/SystemTask.h"
  #include "components/fs/FS.h"
  #include "displayapp/InfiniTimeTheme.h"

  #define Log

  #ifdef Log
    #include <nrf_log.h>
  #endif

using namespace Pinetime::Applications::Screens;

File::File(lfs_info& info) {
  path = info.name;
  dir = info.type == LFS_TYPE_DIR;
  size = info.size;
}

File::File(const char* path, bool dir) : path {path}, dir {dir} {
}

File::~File() {

}

bool File::IsDir() {
  return dir;
}

bool File::Open() {
  if (dir) {
    lfs_dir_t dir;
    auto& fs = System::SystemTask::displayApp->filesystem;
    if (0 == fs.DirOpen(path.c_str(), &dir)) {
      lfs_info info;
      // nScreens = 0;
      while (fs.DirRead(&dir, &info)) {
  #ifdef Log
        NRF_LOG_INFO("FileManager file=%s", info.name);
  #endif
      }
      assert(fs.DirClose(&dir) == 0);
    }
  } else if (path.ends_with(".bin")) {
    lv_obj_t* image = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(image, path.c_str());
    lv_obj_align(image, lv_scr_act(), LV_ALIGN_CENTER, 0, 0);
  } else if (path.ends_with(".txt")) {
    lv_obj_t* label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(label, LV_HOR_RES);
    lv_obj_align(label, lv_scr_act(), LV_ALIGN_CENTER, 0, 0);

    auto& fs = System::SystemTask::displayApp->filesystem;
    static char* buf = (char*) lv_mem_alloc(size);
    if (!buf) {
      lv_label_set_text_static(label, "could not allocate buffer");
      return;
    }

    lfs_file_t fp;
    if (fs.FileOpen(&fp, path.c_str(), LFS_O_RDONLY) != LFS_ERR_OK) {
      lv_label_set_text_static(label, "could not open file");
      lv_mem_free(buf);
      return;
    }

    fs.FileRead(&fp, reinterpret_cast<uint8_t*>(buf), size);
    lv_label_set_text_static(label, buf);
    fs.FileClose(&fp);
  }
}

FileView::FileView() : Screen(Apps::FileView) {
}

void FileView::Load() {
  running = true;

  lv_obj_t* label = lv_btn_create(lv_scr_act(), NULL);
  label->user_data = this;
  lv_obj_set_height(label, 20);
  lv_obj_set_width(label, LV_HOR_RES);
  lv_obj_align(label, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, 0);

  lv_obj_t* txtMessage = lv_label_create(label, NULL);
  lv_obj_set_style_local_bg_color(label, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_NAVY);
  // lv_label_set_text_static(txtMessage, path.c_str());
}

bool FileView::UnLoad() {
  if (running) {
    running = false;
    lv_obj_clean(lv_scr_act());
  }
  return true;
}

FileView::~FileView() {
  lv_obj_clean(lv_scr_act());
}

#endif
