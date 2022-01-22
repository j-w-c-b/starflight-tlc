#include "Button.h"
#include "Events.h"
#include "Game.h"
#include "ModeMgr.h"

using namespace std;

Button::Button(const string &initImgFileNormal,
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

Button::Button(const string &initImgFileNormal,
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

Button::Button(ALLEGRO_BITMAP *initImgBMPNormal,
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

Button::Button(ALLEGRO_BITMAP *initImgBMPNormal,
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

        al_draw_text(fontPtr,
                     textColor,
                     textX,
                     textY,
                     ALLEGRO_ALIGN_CENTER,
                     buttonText.c_str());
    }

    if (mouseIsOverButton) {
        Event e(mouseOverEvent);
        Game::modeMgr->BroadcastEvent(&e);
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

    Event e(clickEvent);
    Game::modeMgr->BroadcastEvent(&e);

    return true; // success
}

bool
Button::PtInBtn(int initX, int initY) {
    if ((initX >= x) && (initX < (x + al_get_bitmap_width(imgNormal))) &&
        (initY >= y) && (initY < (y + al_get_bitmap_height(imgNormal))))
        return true;

    return false;
}
