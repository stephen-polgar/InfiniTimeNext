
#include "displayapp/screens/Gallery.h"
#ifdef UseGallery
#include "systemtask/SystemTask.h"

using namespace Pinetime::Applications::Screens;

Gallery::Gallery() : Screen(Apps::Gallery) {
}

void Gallery::Load() {
  running = true;
  ListDir();
  if (nScreens == 0) {
    lv_obj_t* title = lv_label_create(lv_scr_act(), nullptr);
    lv_label_set_text_static(title, "no files found");
    lv_label_set_align(title, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(title, lv_scr_act(), LV_ALIGN_CENTER, 0, 0);
  } else {
    Open(0, FullRefreshDirections::None);
  }
}

bool Gallery::UnLoad() {
  if (running) {
    running = false;
    lv_obj_clean(lv_scr_act());
  }
  return true;
}

Gallery::~Gallery() {
  UnLoad();
}

bool Gallery::OnTouchEvent(Applications::TouchEvents event) {
  switch (event) {
    case Applications::TouchEvents::SwipeRight:
      return Open(index - 1, FullRefreshDirections::Right);
    case Applications::TouchEvents::SwipeLeft:
      return Open(index + 1, FullRefreshDirections::Left);
    case Applications::TouchEvents::LongTap:
    case Applications::TouchEvents::DoubleTap:
      current->ToggleInfo();
      return true;
    default:
    break;
  }
  return false;
}

void Gallery::ListDir() {
  lfs_dir_t dir;
  auto& fs = System::SystemTask::displayApp->filesystem;
  if (0 == fs.DirOpen(directory, &dir)) {
    lfs_info info;
    nScreens = 0;
    while (fs.DirRead(&dir, &info)) {
      if (info.type == LFS_TYPE_DIR)
        continue;
      nScreens++;
    }
    assert(fs.DirClose(&dir) == 0);
  }
}

bool Gallery::Open(uint8_t n, FullRefreshDirections direction) {
  if (n >= nScreens)
    return false;
  lfs_dir_t dir;
  auto& fs = System::SystemTask::displayApp->filesystem;
  if (0 != fs.DirOpen(directory, &dir))
    return false;
  lfs_info info;
  index = n;
  uint8_t i = 0;
  while (fs.DirRead(&dir, &info)) {
    if (info.type == LFS_TYPE_DIR)
      continue;
    if (n == i)
      break;
    i++;
  }
  assert(fs.DirClose(&dir) == 0);

  if (current) {
    current.reset();
    System::SystemTask::displayApp->SetFullRefresh(direction);
  }

  char fullname[LFS_NAME_MAX] = "F:";
  strncat(fullname, directory, sizeof(fullname) - 2 - 1);
  strncat(fullname, info.name, sizeof(fullname) - strlen(directory) - 2 - 1);

  if (StringEndsWith(fullname, ".bin")) {
    current = std::make_unique<ImageView>(n, nScreens, fullname, pageIndicator);
  } else if (StringEndsWith(fullname, ".txt")) {
    current = std::make_unique<TextView>(n, nScreens, fullname, pageIndicator);
  } else {
    return false;
  }
  return true;
}

uint8_t Gallery::StringEndsWith(const char* str, const char* suffix) {
  uint8_t str_len = strlen(str);
  uint8_t suffix_len = strlen(suffix);
  return (str_len >= suffix_len) && (0 == strcmp(str + (str_len - suffix_len), suffix));
}
#endif
