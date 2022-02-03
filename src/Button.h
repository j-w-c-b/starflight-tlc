#ifndef BUTTON_H
#define BUTTON_H
#pragma once

#include <string>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include "Bitmap.h"
#include "Label.h"
#include "Module.h"

class NewButton : public Module {
  public:
    NewButton(
        int x,
        int y,
        int width,
        int height,
        EventType mouse_over_event,
        EventType click_event,
        ALLEGRO_BITMAP *normal,
        ALLEGRO_BITMAP *mouse_over = nullptr,
        ALLEGRO_BITMAP *disabled = nullptr,
        const std::string &sound_name = "");

    NewButton(
        int x,
        int y,
        EventType mouse_over_event,
        EventType click_event,
        ALLEGRO_BITMAP *normal,
        ALLEGRO_BITMAP *mouse_over = nullptr,
        ALLEGRO_BITMAP *disabled = nullptr,
        const std::string &sound_name = "")
        : NewButton(
            x,
            y,
            -1,
            -1,
            mouse_over_event,
            click_event,
            normal,
            mouse_over,
            disabled,
            sound_name) {}

    void set_enabled(bool enabled);
    void set_highlight(bool highlight);

  protected:
    virtual bool on_init() override;
    virtual bool on_mouse_move(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_mouse_button_click(ALLEGRO_MOUSE_EVENT *event) override;

  private:
    void update_active_bitmap(bool mouse_over = false);

    EventType m_mouse_over_event;
    EventType m_click_event;

    bool m_is_enabled;
    bool m_is_highlight;

    std::shared_ptr<Bitmap> m_normal;
    std::shared_ptr<Bitmap> m_mouse_over;
    std::shared_ptr<Bitmap> m_disabled;

    std::string m_sound_name;
};

class TextButton : public NewButton {
  public:
    TextButton(
        std::shared_ptr<Label> text,
        int x,
        int y,
        int width,
        int height,
        EventType mouse_over_event,
        EventType click_event,
        ALLEGRO_BITMAP *normal,
        ALLEGRO_BITMAP *mouse_over = nullptr,
        ALLEGRO_BITMAP *disabled = nullptr,
        const std::string &sound_name = "");
    TextButton(
        const std::string &text,
        ALLEGRO_FONT *font,
        ALLEGRO_COLOR color,
        int flags,
        int x,
        int y,
        int width,
        int height,
        EventType mouse_over_event,
        EventType click_event,
        ALLEGRO_BITMAP *normal,
        ALLEGRO_BITMAP *mouse_over = nullptr,
        ALLEGRO_BITMAP *disabled = nullptr,
        const std::string &sound_name = "");
    TextButton(
        const std::string &text,
        ALLEGRO_FONT *font,
        ALLEGRO_COLOR color,
        int flags,
        int x,
        int y,
        EventType mouse_over_event,
        EventType click_event,
        ALLEGRO_BITMAP *normal,
        ALLEGRO_BITMAP *mouse_over = nullptr,
        ALLEGRO_BITMAP *disabled = nullptr,
        const std::string &sound_name = "")
        : TextButton(
            text,
            font,
            color,
            flags,
            x,
            y,
            -1,
            -1,
            mouse_over_event,
            click_event,
            normal,
            mouse_over,
            disabled,
            sound_name) {}

    void set_text(const std::string &text) { m_text->set_text(text); }

  protected:
    std::shared_ptr<Label> m_text;
};

/**
 * re-usable button class
 */
class Button {
  public:
    // ctors
    Button(
        const std::string &initImgFileNormal,
        const std::string &initImgFileMouseOver,
        const std::string &initImgFileDisabled,
        int initX,
        int initY,
        int initMouseOverEvent,
        int initClickEvent,
        const std::string &initButtonSound = "",
        bool initEnabled = true,
        bool initVisible = true);

    Button(
        const std::string &initImgFileNormal,
        const std::string &initImgFileMouseOver,
        const std::string &initImgFileDisabled,
        int initX,
        int initY,
        int initMouseOverEvent,
        int initClickEvent,
        ALLEGRO_FONT *initFontPtr,
        const std::string &initButtonText,
        ALLEGRO_COLOR initTextColor,
        const std::string &initButtonSound = "",
        bool initEnabled = true,
        bool initVisible = true);

    Button(
        ALLEGRO_BITMAP *initImgBMPNormal,
        ALLEGRO_BITMAP *initImgBMPMouseOver,
        ALLEGRO_BITMAP *initImgBMPDisabled,
        int initX,
        int initY,
        int initMouseOverEvent,
        int initClickEvent,
        const std::string &initButtonSound = "",
        bool initEnabled = true,
        bool initVisible = true);

    Button(
        ALLEGRO_BITMAP *initImgBMPNormal,
        ALLEGRO_BITMAP *initImgBMPMouseOver,
        ALLEGRO_BITMAP *initImgBMPDisabled,
        int initX,
        int initY,
        int initMouseOverEvent,
        int initClickEvent,
        ALLEGRO_FONT *initFontPtr,
        const std::string &initButtonText,
        ALLEGRO_COLOR initTextColor,
        const std::string &initButtonSound = "",
        bool initEnabled = true,
        bool initVisible = true);

    virtual ~Button();

    // accessors
    ALLEGRO_BITMAP *GetImgNormal() const;
    ALLEGRO_BITMAP *GetImgMouseOver() const;
    ALLEGRO_BITMAP *GetImgDisabled() const;
    int GetX() const;
    int GetY() const;
    bool IsVisible() const;
    std::string GetButtonText() const;
    bool IsInitialized() const;
    int GetWidth() const;
    int GetHeight() const;

    // mutators
    void SetX(int initX);
    void SetY(int initY);
    void SetClickEvent(int initClickEvent);
    void SetEnabled(bool enabled);
    void SetVisible(bool visible);
    void SetButtonText(const std::string &initButtonText);
    void SetTextColor(ALLEGRO_COLOR initTextColor);
    void SetHighlight(bool initHighlight);

    // functions
    void Destroy();

    bool Run(ALLEGRO_BITMAP *canvas);
    bool OnMouseMove(int initX, int initY);
    bool OnMouseReleased(int button, int initX, int initY);
    bool PtInBtn(int initX, int initY);

  private:
    ALLEGRO_BITMAP *imgNormal;
    ALLEGRO_BITMAP *imgMouseOver;
    ALLEGRO_BITMAP *imgDisabled;
    std::string buttonSound;
    int x;
    int y;
    int mouseOverEvent;
    int clickEvent;
    bool enabled;
    bool visible;
    std::string buttonText;
    bool initialized;
    bool deleteBitmaps;
    bool highlight;
    int lastMouseX;
    int lastMouseY;
    ALLEGRO_FONT *fontPtr;
    ALLEGRO_COLOR textColor;

    // private functions
};

#endif
// vi: ft=cpp
