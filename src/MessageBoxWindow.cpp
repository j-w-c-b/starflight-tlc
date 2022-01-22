#include "MessageBoxWindow.h"
#include "Button.h"
#include "Events.h"
#include "Game.h"
#include "Label.h"
#include "ModeMgr.h"
#include "gui_resources.h"

using namespace std;
using namespace gui_resources;

MessageBoxWindow::MessageBoxWindow(const string &initheading,
                                   const string &initText,
                                   int initX,
                                   int initY,
                                   int initWidth,
                                   int initHeight,
                                   ALLEGRO_COLOR initTextColor,
                                   bool initCentered)
    : heading(initheading), text(initText), x(initX), y(initY),
      width(initWidth), height(initHeight), textColor(initTextColor),
      centered(initCentered), visible(true), m_resources(GUI_IMAGES) {

    m_ok_button = new Button(m_resources[I_GENERIC_EXIT_BTN_NORM],
                             m_resources[I_GENERIC_EXIT_BTN_OVER],
                             m_resources[I_GENERIC_EXIT_BTN_OVER],
                             x,
                             y,
                             EVENT_MOUSEOVER,
                             EVENT_CLOSE,
                             g_game->font24,
                             "Ok",
                             WHITE);

    int top = y - height / 2;

    if (centered) {
        m_ok_button->SetX(x - m_ok_button->GetWidth() / 2);
        m_ok_button->SetY((y + height / 2) - (m_ok_button->GetHeight() + 7));
        labelText = new Label(text,
                              (x - width / 2) + 20,
                              top + 60,
                              width - 34,
                              height - 20,
                              initTextColor,
                              g_game->font20);
        labelHeading = new Label(heading,
                                 (x - width / 2) + 20,
                                 top + 20,
                                 width - 34,
                                 height - 20,
                                 initTextColor,
                                 g_game->font20);
    } else {
        m_ok_button->SetX((x + width / 2) - (m_ok_button->GetWidth() / 2));
        m_ok_button->SetY((y + height) - (m_ok_button->GetHeight() + 7));
        labelText = new Label(text,
                              x + 20,
                              y + 30,
                              width - 34,
                              height - 20,
                              initTextColor,
                              g_game->font20);
        labelHeading = new Label(heading,
                                 x + 20,
                                 y + 10,
                                 width - 34,
                                 height - 20,
                                 initTextColor,
                                 g_game->font20);
    }

    labelText->Refresh();
    labelHeading->Refresh();
}

MessageBoxWindow::~MessageBoxWindow() {
    if (labelText)
        delete labelText;
    if (labelHeading)
        delete labelHeading;
}

// accessors
int
MessageBoxWindow::GetX() const {
    return x;
}
int
MessageBoxWindow::GetY() const {
    return y;
}
int
MessageBoxWindow::GetWidth() const {
    return width;
}
int
MessageBoxWindow::GetHeight() const {
    return height;
}
bool
MessageBoxWindow::IsVisible() const {
    return visible;
}

// mutators
void
MessageBoxWindow::SetText(const string &initText) {
    text = initText;
    labelText->SetText(text);
    labelText->Refresh();
}
void
MessageBoxWindow::SetX(int initX) {
    x = initX;
    labelText->SetX(x);
    labelText->Refresh();
}
void
MessageBoxWindow::SetY(int initY) {
    y = initY;
    labelText->SetY(y);
    labelText->Refresh();
}
void
MessageBoxWindow::SetTextColor(ALLEGRO_COLOR initTextColor) {
    textColor = initTextColor;
}
void
MessageBoxWindow::SetVisible(bool visibility) {
    visible = visibility;
}

// other funcs
bool
MessageBoxWindow::OnMouseMove(int x, int y) {
    bool result = false;

    if (m_ok_button)
        result = m_ok_button->OnMouseMove(x, y);

    return result;
}
bool
MessageBoxWindow::OnMouseReleased(int button, int x, int y) {
    bool result = false;

    if (m_ok_button)
        result = m_ok_button->OnMouseReleased(button, x, y);

    return result;
}

bool
MessageBoxWindow::OnMouseClick(int /*button*/, int x, int y) {
    bool result = false;

    if (m_ok_button)
        result = m_ok_button->PtInBtn(x, y);

    return result;
}

bool
MessageBoxWindow::OnMousePressed(int /*button*/, int x, int y) {
    bool result = false;

    if (m_ok_button)
        result = m_ok_button->PtInBtn(x, y);

    return result;
}

bool
MessageBoxWindow::OnKeyPress(int keyCode) {
    if (keyCode == ALLEGRO_KEY_ENTER) {
        Event e(EVENT_CLOSE);
        Game::modeMgr->BroadcastEvent(&e);
        return true;
    }
    return false;
}

void
MessageBoxWindow::Update() {}

void
MessageBoxWindow::Draw() {

    ALLEGRO_BITMAP *backBuffer = g_game->GetBackBuffer();

    int left;
    int top;

    if (centered) {
        left = x - width / 2;
        top = y - height / 2;
    } else {
        left = x;
        top = y;
    }

    ALLEGRO_BITMAP *temp = al_create_bitmap(width, height);
    al_set_target_bitmap(temp);
    al_draw_scaled_bitmap(m_resources[I_TRANS_BG],
                          0,
                          0,
                          al_get_bitmap_width(m_resources[I_TRANS_BG]),
                          al_get_bitmap_height(m_resources[I_TRANS_BG]),
                          0,
                          0,
                          width,
                          height,
                          0);
    al_set_target_bitmap(backBuffer);
    al_draw_bitmap(temp, left, top, 0);

    if (m_ok_button)
        m_ok_button->Run(backBuffer);

    al_set_target_bitmap(backBuffer);
    al_draw_scaled_bitmap(m_resources[I_MESSAGEBOX_BAR],
                          0,
                          0,
                          al_get_bitmap_width(m_resources[I_MESSAGEBOX_BAR]),
                          al_get_bitmap_height(m_resources[I_MESSAGEBOX_BAR]),
                          left,
                          top,
                          al_get_bitmap_width(temp),
                          al_get_bitmap_height(m_resources[I_MESSAGEBOX_BAR]),
                          0);
    al_draw_scaled_bitmap(
        m_resources[I_MESSAGEBOX_BAR],
        0,
        0,
        al_get_bitmap_width(m_resources[I_MESSAGEBOX_BAR]),
        al_get_bitmap_height(m_resources[I_MESSAGEBOX_BAR]),
        left,
        top + al_get_bitmap_height(temp) -
            al_get_bitmap_height(m_resources[I_MESSAGEBOX_BAR]),
        al_get_bitmap_width(temp),
        al_get_bitmap_height(m_resources[I_MESSAGEBOX_BAR]),
        0);

    al_destroy_bitmap(temp);

    if (labelHeading)
        labelHeading->Draw(backBuffer);
    if (labelText)
        labelText->Draw(backBuffer);
}
