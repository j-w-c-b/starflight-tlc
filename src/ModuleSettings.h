#pragma once

#include <map>
#include <memory>
#include <optional>
#include <string>

#include <allegro5/allegro.h>

#include "Bitmap.h"
#include "Button.h"
#include "CheckBox.h"
#include "Game.h"
#include "Label.h"
#include "Module.h"
#include "ScrolledModule.h"

class VideoModeSelector : public Module {
  public:
    VideoModeSelector(
        int x,
        int y,
        int width,
        int height,
        VideoMode video_mode);
    virtual ~VideoModeSelector(){};

    VideoMode get_video_mode() const { return m_video_mode; }

  protected:
    virtual bool on_init() override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;
    virtual bool on_close() override;

  private:
    std::shared_ptr<Label> m_current_label;
    std::shared_ptr<Label> m_title;
    std::shared_ptr<Module> m_video_modes;
    std::map<int, VideoMode> m_video_mode_map;
    std::optional<int> m_mode_id;
    VideoMode m_video_mode;
};

class ModuleSettings : public Module {
  public:
    ModuleSettings();
    virtual ~ModuleSettings();

  protected:
    virtual bool on_init() override;
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;
    virtual bool on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;

  private:
    bool SaveConfigurationFile();

    std::shared_ptr<Bitmap> m_background;
    std::shared_ptr<TextButton> m_exit_button;
    std::shared_ptr<TextButton> m_save_button;
    std::shared_ptr<CheckBox> m_fullscreen_button;
    std::shared_ptr<CheckBox> m_enable_sound;
    std::shared_ptr<CheckBox> m_enable_music;
    std::shared_ptr<Label> m_control_keys;
    std::shared_ptr<Label> m_control_effect;
    std::shared_ptr<VideoModeSelector> m_video_mode_selector;
};
// vi: ft=cpp
