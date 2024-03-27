#include "displayapp/screens/FileView.h"
#ifdef UseGallery
#include "systemtask/SystemTask.h"
#include "displayapp/InfiniTimeTheme.h"

using namespace Pinetime::Applications::Screens;

FileView::FileView(uint8_t screenID, uint8_t nScreens, const char* path, Widgets::PageIndicator& pageIndicator)
  : screenID(screenID), nScreens(nScreens), Screen(Apps::FileView), pageIndicator{pageIndicator} {
  
  const char* c = strrchr(path, '/') + 1;
  if (c == nullptr)
    c = path;

  strncpy(name, c, LFS_NAME_MAX - 1);
  char* pchar = strchr(name, '_');
  while (pchar) {
    *pchar = ' ';
    pchar = strchr(pchar + 1, '_');
  }
}

void FileView::Load() {
  running = true;
  if (label != nullptr) {
    return;
  }
  label = lv_btn_create(lv_scr_act(), nullptr);
  label->user_data = this;

  lv_obj_set_height(label, 20);
  lv_obj_set_width(label, LV_HOR_RES);
  lv_obj_align(label, lv_scr_act(), LV_ALIGN_IN_BOTTOM_MID, 0, 0);

  lv_obj_t* txtMessage = lv_label_create(label, nullptr);
  lv_obj_set_style_local_bg_color(label, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_NAVY);
  lv_label_set_text_static(txtMessage, name);

  pageIndicator.Create(screenID, nScreens);
}

bool FileView::UnLoad() {
  if (running) {
    running = false;
  lv_obj_del(label);
  //pageIndicator.Hide();
  label = nullptr;
  }
  return true;
}

void FileView::ToggleInfo() {
  if (label == nullptr)
    Load();
  else
    UnLoad();
}

FileView::~FileView() {
  lv_obj_clean(lv_scr_act());
}

ImageView::ImageView(uint8_t screenID, uint8_t nScreens, const char* path, Widgets::PageIndicator& pageIndicator) : FileView(screenID, nScreens, path, pageIndicator) {
  lv_obj_t* image = lv_img_create(lv_scr_act(), nullptr);
  lv_img_set_src(image, path);
  lv_obj_align(image, lv_scr_act(), LV_ALIGN_CENTER, 0, 0);
}

TextView::TextView(uint8_t screenID, uint8_t nScreens, const char* path, Widgets::PageIndicator& pageIndicator)
  : FileView(screenID, nScreens, path, pageIndicator) {
  lv_obj_t* label = lv_label_create(lv_scr_act(), nullptr);
  lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);
  lv_obj_set_width(label, LV_HOR_RES);
  lv_obj_align(label, lv_scr_act(), LV_ALIGN_CENTER, 0, 0);

  lfs_info info = {0};
  auto& fs = System::SystemTask::displayApp->filesystem;
  if (fs.Stat(path + 2, &info) != LFS_ERR_OK) {
    lv_label_set_text_static(label, "could not stat file");
    return;
  }
  if (info.type != LFS_TYPE_REG) {
    lv_label_set_text_static(label, "not a file");
    return;
  }

  buf = (char*) lv_mem_alloc(info.size);
  if (buf == nullptr) {
    lv_label_set_text_static(label, "could not allocate buffer");
    return;
  }

  lfs_file_t fp;
  if (fs.FileOpen(&fp, path + 2, LFS_O_RDONLY) != LFS_ERR_OK) {
    lv_label_set_text_static(label, "could not open file");
    lv_mem_free(buf);
    return;
  }

  fs.FileRead(&fp, reinterpret_cast<uint8_t*>(buf), info.size);
  lv_label_set_text_static(label, buf);
  fs.FileClose(&fp);
}

TextView::~TextView() {
  if (buf)
    lv_mem_free(buf);
  lv_obj_clean(lv_scr_act());
}
#endif