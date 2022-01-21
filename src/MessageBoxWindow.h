#pragma once
#include <allegro5/allegro.h>
#include <string>

class Button;
class Label;

class MessageBoxWindow {
  public:
    // ctors
    MessageBoxWindow(const std::string &heading,
                     const std::string &initText,
                     int initX,
                     int initY,
                     int initWidth,
                     int initHeight,
                     ALLEGRO_COLOR initTextColor,
                     bool initCentered);

    ~MessageBoxWindow();

    // accessors
    int GetX() const;
    int GetY() const;
    int GetWidth() const;
    int GetHeight() const;
    bool IsVisible() const;

    // mutators
    void SetText(const std::string &initText);
    void SetX(int initX);
    void SetY(int initY);
    void SetTextColor(ALLEGRO_COLOR initTextColor);
    void SetVisible(bool visibility);

    // other funcs
    bool OnMouseMove(int x, int y);
    bool OnMouseReleased(int button, int x, int y);
    bool OnMouseClick(int button, int x, int y);
    bool OnMousePressed(int button, int x, int y);
    bool OnKeyPress(int keyCode);

    void Update();
    void Draw();

    static Button *button1;
    static Button *button2;
    static ALLEGRO_BITMAP *bg;
    static ALLEGRO_BITMAP *bar;

  private:
    std::string heading;
    std::string text;
    int x;
    int y;
    int width;
    int height;
    ALLEGRO_COLOR textColor;
    bool centered;
    bool visible;
    Label *labelText;
    Label *labelHeading;
};
