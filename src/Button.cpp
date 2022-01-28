#include "Button.h"
#include "AudioSystem.h"
#include "Events.h"
#include "Game.h"
#include "ModeMgr.h"

ALLEGRO_DEBUG_CHANNEL("Button")

using namespace std;

NewButton::NewButton(
    int x,
    int y,
    int width,
    int height,
    EventType mouse_over_event,
    EventType click_event,
    ALLEGRO_BITMAP *normal,
    ALLEGRO_BITMAP *mouse_over,
    ALLEGRO_BITMAP *disabled,
    const string &sound_name)
    : Module(x, y, width, height), m_mouse_over_event(mouse_over_event),
      m_click_event(click_event), m_is_visible(true), m_is_enabled(true),
      m_is_highlight(false), m_normal(nullptr), m_mouse_over(nullptr),
      m_disabled(nullptr), m_sound_name(sound_name) {
    ALLEGRO_ASSERT(normal != NULL);
    if (width == -1 || height == 1) {
        if (width == -1) {
            width = al_get_bitmap_width(normal);
        }
        if (height == -1) {
            height = al_get_bitmap_height(normal);
        }

        resize(width, height);
    }

    if (m_sound_name != ""
        && !g_game->audioSystem->SampleExists(m_sound_name)) {
        ALLEGRO_ERROR("Unable to use sound %s\n", m_sound_name.c_str());
        m_sound_name = "";
    }

    m_normal = new Bitmap(normal, x, y);
    add_child_module(m_normal);
    m_current_bitmap = m_normal;

    if (mouse_over != nullptr) {
        m_mouse_over = new Bitmap(mouse_over, x, y);
        add_child_module(m_mouse_over);
        if (m_is_highlight) {
            m_current_bitmap = m_mouse_over;
        }
    }
    if (disabled != nullptr) {
        m_disabled = new Bitmap(disabled, x, y);
        add_child_module(m_disabled);
        if (!m_is_enabled) {
            m_current_bitmap = m_disabled;
        }
    }
}

bool
NewButton::on_update() {
    /*
     * on_init is setting all children to active, so we decide which one is
     * really active here
     */
    if (m_current_bitmap == m_normal) {
        m_normal->set_active(true);
    } else {
        m_normal->set_active(false);
    }
    if (m_mouse_over) {
        if (m_current_bitmap == m_mouse_over) {
            m_mouse_over->set_active(true);
        } else {
            m_mouse_over->set_active(false);
        }
    }

    if (m_disabled) {
        if (m_current_bitmap == m_disabled) {
            m_disabled->set_active(true);
        } else {
            m_disabled->set_active(false);
        }
    }
    return true;
}

bool
NewButton::on_mouse_move(ALLEGRO_MOUSE_EVENT *event) {
    bool need_event = false;

    if (m_is_visible) {
        if (!m_is_enabled) {
            if (m_disabled) {
                m_current_bitmap = m_disabled;
            } else {
                m_current_bitmap = m_normal;
            }
        } else if (
            (need_event = point_within_module(event->x, event->y))
            || m_is_highlight) {
            if (m_mouse_over) {
                m_current_bitmap = m_mouse_over;
            } else {
                m_current_bitmap = m_normal;
            }
        } else {
            m_current_bitmap = m_normal;
        }
        if (need_event) {
            ALLEGRO_EVENT e = {
                .type = static_cast<unsigned int>(m_mouse_over_event)};
            g_game->broadcast_event(&e);
        }
    } else {
        m_current_bitmap = nullptr;
    }
    return true;
}

bool
NewButton::on_mouse_button_up(ALLEGRO_MOUSE_EVENT *event) {
    if (!m_is_enabled || !m_is_visible || event->button != 1) {
        return true;
    }
    if (is_mouse_click(event)) {
        // make sure button sound isn't playing
        if (m_sound_name != "") {
            if (Game::audioSystem->IsPlaying(m_sound_name))
                Game::audioSystem->Stop(m_sound_name);

            // play button sound
            Game::audioSystem->Play(m_sound_name);
        }

        ALLEGRO_EVENT e = {.type = static_cast<unsigned int>(m_click_event)};
        g_game->broadcast_event(&e);

        return false;
    }
    return true;
}

Button::Button(
    const string &initImgFileNormal,
    const string &initImgFileMouseOver,
    const string &initImgFileDisabled,
    int initX,
    int initY,
    int initMouseOverEvent,
    int initClickEvent,
    const string &initButtonSound /*= ""*/,
    bool initEnabled /*= true*/,
    bool initVisible /*= true*/)
    : imgNormal(NULL), imgMouseOver(NULL), imgDisabled(NULL),
      buttonSound(initButtonSound), x(initX), y(initY),
      mouseOverEvent(initMouseOverEvent), clickEvent(initClickEvent),
      enabled(initEnabled), visible(initVisible), buttonText(""),
      initialized(false), deleteBitmaps(true), highlight(false), lastMouseX(0),
      lastMouseY(0), fontPtr(NULL), textColor(BLACK) {
    imgNormal = al_load_bitmap(initImgFileNormal.c_str());
    al_convert_mask_to_alpha(imgNormal, MASK_COLOR);
    imgMouseOver = al_load_bitmap(initImgFileMouseOver.c_str());
    al_convert_mask_to_alpha(imgMouseOver, MASK_COLOR);
    imgDisabled = al_load_bitmap(initImgFileDisabled.c_str());
    al_convert_mask_to_alpha(imgDisabled, MASK_COLOR);

    if (imgNormal != NULL /*&& imgMouseOver != NULL && imgDisabled != NULL*/)
        initialized = true;
}

Button::Button(
    const string &initImgFileNormal,
    const string &initImgFileMouseOver,
    const string &initImgFileDisabled,
    int initX,
    int initY,
    int initMouseOverEvent,
    int initClickEvent,
    ALLEGRO_FONT *initFontPtr,
    const string &initButtonText,
    ALLEGRO_COLOR initTextColor,
    const string &initButtonSound /*= ""*/,
    bool initEnabled /*= true*/,
    bool initVisible /*= true*/)
    : imgNormal(NULL), imgMouseOver(NULL), imgDisabled(NULL),
      buttonSound(initButtonSound), x(initX), y(initY),
      mouseOverEvent(initMouseOverEvent), clickEvent(initClickEvent),
      enabled(initEnabled), visible(initVisible), buttonText(initButtonText),
      initialized(false), deleteBitmaps(true), highlight(false), lastMouseX(0),
      lastMouseY(0), fontPtr(initFontPtr), textColor(initTextColor) {
    imgNormal = al_load_bitmap(initImgFileNormal.c_str());
    al_convert_mask_to_alpha(imgNormal, MASK_COLOR);
    imgMouseOver = al_load_bitmap(initImgFileMouseOver.c_str());
    al_convert_mask_to_alpha(imgMouseOver, MASK_COLOR);
    imgDisabled = al_load_bitmap(initImgFileDisabled.c_str());
    al_convert_mask_to_alpha(imgDisabled, MASK_COLOR);

    if (imgNormal != NULL /*&& imgMouseOver != NULL && imgDisabled != NULL*/)
        initialized = true;
}

Button::Button(
    ALLEGRO_BITMAP *initImgBMPNormal,
    ALLEGRO_BITMAP *initImgBMPMouseOver,
    ALLEGRO_BITMAP *initImgBMPDisabled,
    int initX,
    int initY,
    int initMouseOverEvent,
    int initClickEvent,
    const string &initButtonSound /*= ""*/,
    bool initEnabled /*= true*/,
    bool initVisible /*= true*/)
    : imgNormal(initImgBMPNormal), imgMouseOver(initImgBMPMouseOver),
      imgDisabled(initImgBMPDisabled), buttonSound(initButtonSound), x(initX),
      y(initY), mouseOverEvent(initMouseOverEvent), clickEvent(initClickEvent),
      enabled(initEnabled), visible(initVisible), buttonText(""),
      initialized(false), deleteBitmaps(false), highlight(false), lastMouseX(0),
      lastMouseY(0), fontPtr(NULL), textColor(BLACK) {
    if (imgNormal != NULL /*&& imgMouseOver != NULL && imgDisabled != NULL*/)
        initialized = true;
}

Button::Button(
    ALLEGRO_BITMAP *initImgBMPNormal,
    ALLEGRO_BITMAP *initImgBMPMouseOver,
    ALLEGRO_BITMAP *initImgBMPDisabled,
    int initX,
    int initY,
    int initMouseOverEvent,
    int initClickEvent,
    ALLEGRO_FONT *initFontPtr,
    const string &initButtonText,
    ALLEGRO_COLOR initTextColor,
    const string &initButtonSound /*= ""*/,
    bool initEnabled /*= true*/,
    bool initVisible /*= true*/)
    : imgNormal(initImgBMPNormal), imgMouseOver(initImgBMPMouseOver),
      imgDisabled(initImgBMPDisabled), buttonSound(initButtonSound), x(initX),
      y(initY), mouseOverEvent(initMouseOverEvent), clickEvent(initClickEvent),
      enabled(initEnabled), visible(initVisible), buttonText(initButtonText),
      initialized(false), deleteBitmaps(false), highlight(false), lastMouseX(0),
      lastMouseY(0), fontPtr(initFontPtr), textColor(initTextColor) {
    if (imgNormal != NULL /*&& imgMouseOver != NULL && imgDisabled != NULL*/)
        initialized = true;
}

Button::~Button() { Destroy(); }

// accessors
ALLEGRO_BITMAP *
Button::GetImgNormal() const {
    return imgNormal;
}
ALLEGRO_BITMAP *
Button::GetImgMouseOver() const {
    return imgMouseOver;
}
ALLEGRO_BITMAP *
Button::GetImgDisabled() const {
    return imgDisabled;
}
int
Button::GetX() const {
    return x;
}
int
Button::GetY() const {
    return y;
}
bool
Button::IsVisible() const {
    return visible;
}
std::string
Button::GetButtonText() const {
    return buttonText;
}
bool
Button::IsInitialized() const {
    return initialized;
}
int
Button::GetWidth() const {
    if (initialized)
        return al_get_bitmap_width(imgNormal);
    return 0;
}
int
Button::GetHeight() const {
    if (initialized)
        return al_get_bitmap_height(imgNormal);
    return 0;
}

// mutators
void
Button::SetX(int initX) {
    x = initX;
}
void
Button::SetY(int initY) {
    y = initY;
}
void
Button::SetClickEvent(int initClickEvent) {
    clickEvent = initClickEvent;
}
void
Button::SetEnabled(bool initEnabled) {
    enabled = initEnabled;
}
void
Button::SetVisible(bool initVisible) {
    visible = initVisible;
}
void
Button::SetButtonText(const string &initButtonText) {
    buttonText = initButtonText;
}
void
Button::SetTextColor(ALLEGRO_COLOR initTextColor) {
    textColor = initTextColor;
}
void
Button::SetHighlight(bool initHighlight) {
    highlight = initHighlight;
}

void
Button::Destroy() {
    if (deleteBitmaps) {
        if (imgNormal != NULL) {
            al_destroy_bitmap(imgNormal);
            imgNormal = NULL;
        }

        if (imgMouseOver != NULL) {
            al_destroy_bitmap(imgMouseOver);
            imgMouseOver = NULL;
        }

        if (imgDisabled != NULL) {
            al_destroy_bitmap(imgDisabled);
            imgDisabled = NULL;
        }
    }
    initialized = false;
}

bool
Button::Run(ALLEGRO_BITMAP *canvas) {
    if (!initialized)
        return false;

    if (!visible)
        return false;

    ALLEGRO_BITMAP *imgToDraw = imgNormal;

    al_set_target_bitmap(canvas);

    bool mouseIsOverButton = PtInBtn(lastMouseX, lastMouseY);

    if (!enabled && (imgDisabled != NULL))
        imgToDraw = imgDisabled;
    else if (enabled && mouseIsOverButton && (imgMouseOver != NULL))
        imgToDraw = imgMouseOver;
    else if (enabled && highlight && (imgMouseOver != NULL))
        imgToDraw = imgMouseOver;

    al_draw_bitmap(imgToDraw, x, y, 0);

    if (fontPtr != NULL && buttonText.length() > 0) {
        // get center of the button
        int textX = x + GetWidth() / 2;
        int textY = y + GetHeight() / 2 - al_get_font_line_height(fontPtr) / 2;

        al_draw_text(
            fontPtr,
            textColor,
            textX,
            textY,
            ALLEGRO_ALIGN_CENTER,
            buttonText.c_str());
    }

    if (mouseIsOverButton) {
        ALLEGRO_EVENT e = {.type = static_cast<unsigned int>(mouseOverEvent)};
        g_game->broadcast_event(&e);
    }
    return true;
}

bool
Button::OnMouseMove(int initX, int initY) {
    lastMouseX = initX;
    lastMouseY = initY;

    return PtInBtn(initX, initY);
}

bool
Button::OnMouseReleased(int /*button*/, int initX, int initY) {
    if (!initialized)
        return false;

    if (!visible)
        return false;

    if (!enabled)
        return false;

    if (!PtInBtn(initX, initY))
        return false;

    // make sure button sound isnt playing
    if (buttonSound != "") {
        if (Game::audioSystem->IsPlaying(buttonSound))
            Game::audioSystem->Stop(buttonSound);

        // play button sound
        Game::audioSystem->Play(buttonSound);
    }

    ALLEGRO_EVENT e = {.type = static_cast<unsigned int>(clickEvent)};
    g_game->broadcast_event(&e);

    return true;
}

bool
Button::PtInBtn(int initX, int initY) {
    if ((initX >= x) && (initX < (x + al_get_bitmap_width(imgNormal)))
        && (initY >= y) && (initY < (y + al_get_bitmap_height(imgNormal))))
        return true;

    return false;
}
// vi: ft=cpp
