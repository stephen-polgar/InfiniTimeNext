
#include "displayapp/screens/FileManager.h"
#ifdef UseFileManager
  #include "WatchFaceScreen.h"
  #include "systemtask/SystemTask.h"
  #include "displayapp/InfiniTimeTheme.h"

// #define Log

  #ifdef Log
    #include <nrf_log.h>
  #endif

using namespace Pinetime::Applications::Screens;

FileManager::FileManager(const char* dir)
  : Screen(Apps::FileManager),
    dirPath {dir},
    arrayTouchHandler {maxItems, [this](uint8_t indexBegin, uint8_t indexEnd, Screen::FullRefreshDirections direction) {
                         load(indexBegin, indexEnd, direction);
                       }} {
  fs = &System::SystemTask::displayApp->filesystem;
}

void FileManager::Load() {
  lv_obj_set_style_local_bg_color(lv_scr_act(), LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, lv_color_make(0, 0, 0));
  container = lv_cont_create(lv_scr_act(), NULL);
  container->user_data = this;
  static constexpr uint8_t innerPad = 4;
  lv_obj_set_style_local_bg_opa(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
  lv_obj_set_style_local_pad_inner(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, innerPad);
  lv_obj_set_style_local_border_width(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_size(container, LV_HOR_RES - 8, LV_VER_RES);
  lv_cont_set_layout(container, LV_LAYOUT_COLUMN_LEFT);
  static constexpr uint16_t btnHeight = (LV_HOR_RES_MAX - ((maxItems - 1) * innerPad)) / maxItems;
  for (uint8_t i = 0; i < maxItems; i++) {
    lv_obj_t* btn = lv_btn_create(container, NULL);
    lv_obj_set_style_local_radius(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, btnHeight / 3);
    lv_obj_set_style_local_bg_color(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, Colors::bgAlt);
    lv_obj_set_width(btn, LV_HOR_RES - 8);
    lv_obj_set_height(btn, btnHeight);
    lv_btn_set_layout(btn, LV_LAYOUT_OFF);
    lv_obj_set_event_cb(btn, fileEventHandler);
    lv_obj_set_style_local_clip_corner(btn, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, true);
    lv_obj_t* icon = lv_label_create(btn, NULL);
    lv_obj_set_style_local_text_color(icon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_YELLOW);
    lv_label_set_align(icon, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(icon, NULL, LV_ALIGN_IN_LEFT_MID, 8, 0);
    lv_obj_t* text = lv_label_create(btn, NULL);
    lv_obj_set_width(text, LV_HOR_RES - 20);
    // lv_label_set_long_mode(text, LV_LABEL_LONG_SROLL_CIRC);
    lv_obj_align(text, icon, LV_ALIGN_OUT_RIGHT_MID, -30, 0);
  }
  updateSize();
  arrayTouchHandler.LoadCurrentPos();
  running = true;
}

void FileManager::load(uint8_t indexBegin, uint8_t indexEnd, Screen::FullRefreshDirections direction) {
  #ifdef Log
  NRF_LOG_INFO("FileManager indexBegin=%d indexEnd=%d", indexBegin, indexEnd);
  #endif
  if (running) {
    System::SystemTask::displayApp->SetFullRefresh(direction);
  }
  pageIndicator.Load(indexBegin >= maxItems ? indexBegin / maxItems : 0);

  uint8_t i = 0, n = 0;
  lfs_dir_t dir;
  if (0 == fs->DirOpen(dirPath.c_str(), &dir)) {
    lfs_info info;
    uint8_t skeep = dirPath.starts_with(rootDir) ? 2 : 1;
    while (fs->DirRead(&dir, &info) && n < maxItems) {
      if (skeep)
        skeep--;
      else {
        if (i >= indexBegin) {
          File& f = files[n++];
          f.path = info.name;
          f.dir = info.type == LFS_TYPE_DIR;
          f.size = info.size;
        }
        i++;
      }
    }
    assert(fs->DirClose(&dir) == 0);
  }

  lv_obj_t* btn = NULL;
  while (indexEnd-- > indexBegin) {
    btn = lv_obj_get_child(container, btn);
    lv_obj_set_hidden(btn, false);
    File& file = files[indexEnd - indexBegin];
    btn->user_data = (void*) &file;
    lv_obj_t* text = lv_obj_get_child(btn, NULL);
    lv_label_set_text_static(lv_obj_get_child(btn, text), file.dir ? "D" : "F");
    lv_label_set_text_static(text, file.path.c_str());
  }
  while (btn) {
    btn = lv_obj_get_child(container, btn);
    if (btn)
      lv_obj_set_hidden(btn, true);
  }
}

bool FileManager::UnLoad() {
  running = false;
  if (menuContainer) {
    lv_obj_del(menuContainer);
    menuContainer = NULL;
  }
  if (image) {
    lv_obj_del(image);
    image = NULL;
  }
  pageIndicator.UnLoad();
  lv_obj_del(container);
  #ifdef Log
  NRF_LOG_INFO("FileManager UnLoad end");
  #endif
  return true;
}

FileManager::~FileManager() {
  #ifdef Log
  NRF_LOG_INFO("FileManager delete");
  #endif
  UnLoad();
}

void FileManager::updateSize() {
  uint8_t i = 0;
  lfs_dir_t dir;
  if (0 == fs->DirOpen(dirPath.c_str(), &dir)) {
    lfs_info info;
    uint8_t skeep = dirPath.starts_with(rootDir) ? 2 : 1;
    while (fs->DirRead(&dir, &info)) {
      if (skeep)
        skeep--;
      else
        i++;
  #ifdef Log
      NRF_LOG_INFO("FileManager file=%s", info.name);
  #endif
    }
    assert(fs->DirClose(&dir) == 0);
  }
  #ifdef Log
  NRF_LOG_INFO("FileManager files=%d", i);
  #endif
  arrayTouchHandler.SetArraySize(i);
  pageIndicator.SetScreens(i / maxItems + 1);
}

void FileManager::openFile(lv_obj_t* item) {
  if (menuContainer)
    return;
  File* file = static_cast<File*>(item->user_data);
  if (file->dir) {
    dirPath = file->path;
  #ifdef Log
    NRF_LOG_INFO("FileManager path=%s", file->path.c_str());
  #endif
    updateSize();
    arrayTouchHandler.LoadCurrentPos();
  } else if (file->path.ends_with(".bin")) {
    std::string path = "F:" + (dirPath.starts_with(rootDir) ? "" : "/" + dirPath) + "/" + file->path;
    pageIndicator.UnLoad();
    lv_obj_set_hidden(container, true);
    image = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(image, path.c_str());
  #ifdef Log
    NRF_LOG_INFO("FileManager open image=%s", path.c_str());
  #endif
    if (lv_obj_get_height(image)) {
      image->user_data = item; // item
      lv_obj_align(image, lv_scr_act(), LV_ALIGN_CENTER, 0, 0);
    } else {
      showMenu("Unsupported image", item);
    }
  } else if (file->path.ends_with(".txt")) {
    lv_obj_t* label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(label, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(label, LV_HOR_RES);
    lv_obj_align(label, lv_scr_act(), LV_ALIGN_CENTER, 0, 0);
    static char* buf = (char*) lv_mem_alloc(file->size);
    if (buf) {
      lfs_file_t fp;
      if (fs->FileOpen(&fp, file->path.c_str(), LFS_O_RDONLY) == LFS_ERR_OK) {
        fs->FileRead(&fp, reinterpret_cast<uint8_t*>(buf), file->size);
        lv_label_set_text(label, buf);
        fs->FileClose(&fp);
      } else {
        lv_label_set_text_static(label, "could not open file");
      }
      lv_mem_free(buf);
    } else
      lv_label_set_text_static(label, "Not Enough RAM");
  } // else showMenu(NULL, item);
}

void FileManager::deleteFile(lv_obj_t* item) {
  File* file = static_cast<File*>(item->user_data);
  std::string path = (dirPath.starts_with(rootDir) ? "" : "/" + dirPath) + "/" + file->path;
  #ifdef Log
  NRF_LOG_INFO("FileManager delete=%s", path.c_str());
  #endif
  if (LFS_ERR_OK == fs->FileDelete(path.c_str())) {
    lv_obj_set_hidden(item, true);
  }
  closeMenu();
}

void FileManager::showMenu(const char* title, lv_obj_t* item, bool image) {
  menuContainer = lv_cont_create(lv_scr_act(), NULL);
  menuContainer->user_data = this;
  // lv_obj_set_pos(menuContainer, 0, 40);
  lv_obj_set_style_local_bg_color(menuContainer, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_obj_set_style_local_bg_opa(menuContainer, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_70);
  lv_obj_set_style_local_pad_inner(menuContainer, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 20);
  // lv_obj_set_style_local_border_width(container, LV_CONT_PART_MAIN, LV_STATE_DEFAULT, 0);
  lv_obj_set_size(menuContainer, LV_HOR_RES, LV_VER_RES);
  lv_cont_set_layout(menuContainer, LV_LAYOUT_COLUMN_MID);

  if (title) {
    lv_obj_t* label = lv_label_create(menuContainer, NULL);
    lv_label_set_long_mode(label, LV_LABEL_LONG_EXPAND);
    lv_label_set_text_static(label, title);
  }

  lv_obj_t* label = lv_label_create(menuContainer, NULL);
  lv_label_set_long_mode(label, LV_LABEL_LONG_EXPAND);
  lv_label_set_text_static(label, static_cast<File*>(item->user_data)->path.c_str());

  lv_obj_t* btn = lv_btn_create(menuContainer, NULL);
  btn->user_data = item;
  label = lv_label_create(btn, label);
  lv_label_set_text_static(label, "Delete");
  lv_obj_set_size(btn, lv_obj_get_width(label) + 8, 50);
  lv_obj_set_event_cb(btn, [](lv_obj_t* obj, lv_event_t event) {
    if (event == LV_EVENT_LONG_PRESSED) {
      static_cast<FileManager*>(obj->parent->user_data)->deleteFile(static_cast<lv_obj_t*>(obj->user_data));
    }
  });

  if (image) {
    btn = lv_btn_create(menuContainer, NULL);
    btn->user_data = item;
    label = lv_label_create(btn, label);
    lv_label_set_text_static(label, "Set as background");
    lv_obj_set_size(btn, lv_obj_get_width(label) + 8, 50);
    lv_obj_set_event_cb(btn, [](lv_obj_t* obj, lv_event_t event) {
      if (event == LV_EVENT_CLICKED) {
        WatchFaceScreen::bgImage = (char*) lv_img_get_src(static_cast<FileManager*>(obj->parent->user_data)->image);
        System::SystemTask::displayApp->StartApp(Apps::Clock, Screen::FullRefreshDirections::None);
        //  static_cast<FileManager*>(obj->parent->user_data)->running = false;
      }
    });
  }

  btn = lv_btn_create(menuContainer, NULL);
  label = lv_label_create(btn, label);
  lv_label_set_text_static(label, "X");
  lv_obj_set_size(btn, 120, 50);
  lv_obj_set_event_cb(btn, [](lv_obj_t* obj, lv_event_t event) {
    if (event == LV_EVENT_CLICKED) {
      static_cast<FileManager*>(obj->parent->user_data)->closeMenu();
    }
  });
}

void FileManager::closeMenu() {
  if (menuContainer) {
    lv_obj_del(menuContainer);
    menuContainer = NULL;
  }
  if (image) {
    lv_obj_del(image);
    image = NULL;
    lv_obj_set_hidden(container, false);
  }
}

void FileManager::fileEventHandler(lv_obj_t* item, lv_event_t event) {
  if (event == LV_EVENT_CLICKED) {
    static_cast<FileManager*>(item->parent->user_data)->openFile(item);
  } else if (event == LV_EVENT_LONG_PRESSED && !static_cast<File*>(item->user_data)->dir) {
    static_cast<FileManager*>(item->parent->user_data)->showMenu(NULL, item);
  }
}

bool FileManager::OnTouchEvent(Applications::TouchEvents event) {
  return arrayTouchHandler.OnTouchEvent(event);
}

bool FileManager::OnButtonPushed() {
  if (image) {
    closeMenu();
    return true;
  }
  return false;
}

bool FileManager::OnTouchEvent(uint16_t, uint16_t) {
  if (image && !menuContainer) {
    showMenu(NULL, static_cast<lv_obj_t*>(image->user_data), true);
    return true;
  }
  return false;
}

#endif
