/*
        STARFLIGHT - THE LOST COLONY
        ModuleTitleScreen.h -
        Author: J.Harbour
        Date: Dec,2007
*/

#ifndef MODULETITLESCREEN_H
#define MODULETITLESCREEN_H

#include "Button.h"
#include "Module.h"
#include "ResourceManager.h"

class ModuleTitleScreen : public Module {
  public:
    ModuleTitleScreen();
    virtual ~ModuleTitleScreen();

    bool Init() override;
    void Update() override;
    void Draw() override;
    void OnKeyReleased(int keyCode) override;
    void OnMouseMove(int x, int y) override;
    void OnMouseReleased(int button, int x, int y) override;
    void OnEvent(Event *event) override;
    void Close() override;

  private:
    float m_rotationAngle;
    ResourceManager<ALLEGRO_BITMAP> m_resources;
    Button *btnTitle;
    Button *btnNewGame;
    Button *btnLoadGame;
    Button *btnSettings;
    Button *btnCredits;
    Button *btnQuit;
    int title_mode;
};

#endif
