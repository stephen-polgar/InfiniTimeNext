/*  Copyright (C) 2020 JF, Adam Pigg, Avamander

    This file is part of InfiniTime.

    InfiniTime is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    InfiniTime is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "Music.h"
#include "systemtask/SystemTask.h"
#include "components/ble/MusicService.h"
#include "displayapp/icons/music/disc.c"
#include "displayapp/icons/music/disc_f_1.c"
#include "displayapp/icons/music/disc_f_2.c"

using namespace Pinetime::Applications::Screens;

void Music::event_handler(lv_obj_t* obj, lv_event_t event) {
  (static_cast<Music*>(obj->user_data))->onObjectEvent(obj, event);
}

/**
 * Set the pixel array to display by the image
 * This just calls lv_img_set_src but adds type safety
 *
 * @param img pointer to an image object
 * @param data the image array
 */
inline void lv_img_set_src_arr(lv_obj_t* img, const lv_img_dsc_t* src_img) {
  lv_img_set_src(img, src_img);
}

/**
 * Music control watchapp
 *
 * TODO: Investigate Apple Media Service and AVRCPv1.6 support for seamless integration
 */
Music::Music() : Screen(Apps::Music) {
}

void Music::Load() {
  running = true;
  totalLength = 0;
  lastIncrement = 0;
  lv_style_init(&btn_style);
  lv_style_set_radius(&btn_style, LV_STATE_DEFAULT, 20);
  lv_style_set_bg_color(&btn_style, LV_STATE_DEFAULT, LV_COLOR_AQUA);
  lv_style_set_bg_opa(&btn_style, LV_STATE_DEFAULT, LV_OPA_50);

  btnVolDown = lv_btn_create(lv_scr_act(), NULL);
  btnVolDown->user_data = this;
  lv_obj_set_event_cb(btnVolDown, event_handler);
  lv_obj_set_size(btnVolDown, 76, 76);
  lv_obj_align(btnVolDown, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
  lv_obj_add_style(btnVolDown, LV_STATE_DEFAULT, &btn_style);
  lv_obj_t* label = lv_label_create(btnVolDown, NULL);
  lv_label_set_text_static(label, Symbols::volumDown);
  lv_obj_set_hidden(btnVolDown, true);

  btnVolUp = lv_btn_create(lv_scr_act(), NULL);
  btnVolUp->user_data = this;
  lv_obj_set_event_cb(btnVolUp, event_handler);
  lv_obj_set_size(btnVolUp, 76, 76);
  lv_obj_align(btnVolUp, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
  lv_obj_add_style(btnVolUp, LV_STATE_DEFAULT, &btn_style);
  label = lv_label_create(btnVolUp, NULL);
  lv_label_set_text_static(label, Symbols::volumUp);
  lv_obj_set_hidden(btnVolUp, true);

  btnPrev = lv_btn_create(lv_scr_act(), NULL);
  btnPrev->user_data = this;
  lv_obj_set_event_cb(btnPrev, event_handler);
  lv_obj_set_size(btnPrev, 76, 76);
  lv_obj_align(btnPrev, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
  lv_obj_add_style(btnPrev, LV_STATE_DEFAULT, &btn_style);
  label = lv_label_create(btnPrev, NULL);
  lv_label_set_text_static(label, Symbols::stepBackward);

  btnNext = lv_btn_create(lv_scr_act(), NULL);
  btnNext->user_data = this;
  lv_obj_set_event_cb(btnNext, event_handler);
  lv_obj_set_size(btnNext, 76, 76);
  lv_obj_align(btnNext, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, 0, 0);
  lv_obj_add_style(btnNext, LV_STATE_DEFAULT, &btn_style);
  label = lv_label_create(btnNext, NULL);
  lv_label_set_text_static(label, Symbols::stepForward);

  btnPlayPause = lv_btn_create(lv_scr_act(), NULL);
  btnPlayPause->user_data = this;
  lv_obj_set_event_cb(btnPlayPause, event_handler);
  lv_obj_set_size(btnPlayPause, 76, 76);
  lv_obj_align(btnPlayPause, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
  lv_obj_add_style(btnPlayPause, LV_STATE_DEFAULT, &btn_style);
  txtPlayPause = lv_label_create(btnPlayPause, NULL);
  lv_label_set_text_static(txtPlayPause, Symbols::play);

  txtTrackDuration = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_long_mode(txtTrackDuration, LV_LABEL_LONG_SROLL);
  lv_obj_align(txtTrackDuration, NULL, LV_ALIGN_IN_TOP_LEFT, 12, 20);
  lv_label_set_text_static(txtTrackDuration, "--:--/--:--");
  lv_label_set_align(txtTrackDuration, LV_ALIGN_IN_LEFT_MID);
  lv_obj_set_width(txtTrackDuration, LV_HOR_RES);

  constexpr uint8_t FONT_HEIGHT = 12;
  constexpr uint8_t LINE_PAD = 15;
  constexpr int8_t MIDDLE_OFFSET = -25;
  txtArtist = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_long_mode(txtArtist, LV_LABEL_LONG_SROLL_CIRC);
  lv_obj_align(txtArtist, NULL, LV_ALIGN_IN_LEFT_MID, 12, MIDDLE_OFFSET + 1 * FONT_HEIGHT);
  lv_label_set_align(txtArtist, LV_ALIGN_IN_LEFT_MID);
  lv_obj_set_width(txtArtist, LV_HOR_RES - 12);
  lv_label_set_text_static(txtArtist, "Artist Name");

  txtTrack = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_long_mode(txtTrack, LV_LABEL_LONG_SROLL_CIRC);
  lv_obj_align(txtTrack, NULL, LV_ALIGN_IN_LEFT_MID, 12, MIDDLE_OFFSET + 2 * FONT_HEIGHT + LINE_PAD);

  lv_label_set_align(txtTrack, LV_ALIGN_IN_LEFT_MID);
  lv_obj_set_width(txtTrack, LV_HOR_RES - 12);
  lv_label_set_text_static(txtTrack, "This is a very long getTrack name");

  /** Init animation */
  imgDisc = lv_img_create(lv_scr_act(), NULL);
  lv_img_set_src_arr(imgDisc, &disc);
  lv_obj_align(imgDisc, NULL, LV_ALIGN_IN_TOP_RIGHT, -15, 15);

  imgDiscAnim = lv_img_create(lv_scr_act(), NULL);
  lv_img_set_src_arr(imgDiscAnim, &disc_f_1);
  lv_obj_align(imgDiscAnim, NULL, LV_ALIGN_IN_TOP_RIGHT, -15 - 32, 15);

  frameB = false;

  System::SystemTask::displayApp->systemTask->nimble().music().event(Controllers::MusicService::EVENT_MUSIC_OPEN);

  taskRefresh = lv_task_create(RefreshTaskCallback, LV_DISP_DEF_REFR_PERIOD, LV_TASK_PRIO_MID, this);
}

bool Music::UnLoad() {
  if (running) {
    running = false;
    lv_task_del(taskRefresh);
    lv_style_reset(&btn_style);
    lv_obj_clean(lv_scr_act());
  }
  return true;
}

Music::~Music() {
  UnLoad();
}

void Music::Refresh() {
  auto& musicService = System::SystemTask::displayApp->systemTask->nimble().music();
  if (artist != musicService.getArtist()) {
    artist = musicService.getArtist();
    lv_label_set_text(txtArtist, artist.data());
  }

  if (track != musicService.getTrack()) {
    track = musicService.getTrack();
    lv_label_set_text(txtTrack, track.data());
  }

  if (album != musicService.getAlbum()) {
    album = musicService.getAlbum();
  }

  if (playing != musicService.isPlaying()) {
    playing = musicService.isPlaying();
  }

  if (currentPosition != musicService.getProgress()) {
    currentPosition = musicService.getProgress();
    updateLength();
  }

  if (totalLength != musicService.getTrackLength()) {
    totalLength = musicService.getTrackLength();
    updateLength();
  }

  if (playing) {
    lv_label_set_text_static(txtPlayPause, Symbols::pause);
    if (xTaskGetTickCount() - 1024 >= lastIncrement) {

      if (frameB) {
        lv_img_set_src(imgDiscAnim, &disc_f_1);
      } else {
        lv_img_set_src(imgDiscAnim, &disc_f_2);
      }
      frameB = !frameB;

      if (currentPosition >= totalLength) {
        // Let's assume the getTrack finished, paused when the timer ends
        //  and there's no new getTrack being sent to us
        playing = false;
      }
      lastIncrement = xTaskGetTickCount();
    }
  } else {
    lv_label_set_text_static(txtPlayPause, Symbols::play);
  }
}

void Music::updateLength() {
  if (totalLength > (99 * 60 * 60)) {
    lv_label_set_text_static(txtTrackDuration, "Inf/Inf");
  } else if (totalLength > (99 * 60)) {
    lv_label_set_text_fmt(txtTrackDuration,
                          "%02d:%02d/%02d:%02d",
                          (currentPosition / (60 * 60)) % 100,
                          ((currentPosition % (60 * 60)) / 60) % 100,
                          (totalLength / (60 * 60)) % 100,
                          ((totalLength % (60 * 60)) / 60) % 100);
  } else {
    lv_label_set_text_fmt(txtTrackDuration,
                          "%02d:%02d/%02d:%02d",
                          (currentPosition / 60) % 100,
                          (currentPosition % 60) % 100,
                          (totalLength / 60) % 100,
                          (totalLength % 60) % 100);
  }
}

void Music::onObjectEvent(lv_obj_t* obj, lv_event_t event) {
  if (event == LV_EVENT_CLICKED) {
    auto& musicService = System::SystemTask::displayApp->systemTask->nimble().music();
    if (obj == btnVolDown) {
      musicService.event(Controllers::MusicService::EVENT_MUSIC_VOLDOWN);
    } else if (obj == btnVolUp) {
      musicService.event(Controllers::MusicService::EVENT_MUSIC_VOLUP);
    } else if (obj == btnPrev) {
      musicService.event(Controllers::MusicService::EVENT_MUSIC_PREV);
    } else if (obj == btnPlayPause) {
      if (playing == Pinetime::Controllers::MusicService::MusicStatus::Playing) {
        musicService.event(Controllers::MusicService::EVENT_MUSIC_PAUSE);

        // Let's assume it stops playing instantly
        playing = Controllers::MusicService::NotPlaying;
      } else {
        musicService.event(Controllers::MusicService::EVENT_MUSIC_PLAY);

        // Let's assume it starts playing instantly
        // TODO: In the future should check for BT connection for better UX
        playing = Controllers::MusicService::Playing;
      }
    } else if (obj == btnNext) {
      musicService.event(Controllers::MusicService::EVENT_MUSIC_NEXT);
    }
  }
}

bool Music::OnTouchEvent(Applications::TouchEvents event) {
  switch (event) {
    case TouchEvents::SwipeUp: {
      lv_obj_set_hidden(btnVolDown, false);
      lv_obj_set_hidden(btnVolUp, false);

      lv_obj_set_hidden(btnNext, true);
      lv_obj_set_hidden(btnPrev, true);
      return true;
    }
    case TouchEvents::SwipeDown: {
      if (lv_obj_get_hidden(btnNext)) {
        lv_obj_set_hidden(btnNext, false);
        lv_obj_set_hidden(btnPrev, false);
        lv_obj_set_hidden(btnVolDown, true);
        lv_obj_set_hidden(btnVolUp, true);
        return true;
      }
      return false;
    }
    case TouchEvents::SwipeLeft: {
      System::SystemTask::displayApp->systemTask->nimble().music().event(Controllers::MusicService::EVENT_MUSIC_NEXT);
      return true;
    }
    case TouchEvents::SwipeRight: {
      System::SystemTask::displayApp->systemTask->nimble().music().event(Controllers::MusicService::EVENT_MUSIC_PREV);
      return true;
    }
    default: {
      return false;
    }
  }
}
