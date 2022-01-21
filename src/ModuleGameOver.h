/*
        STARFLIGHT - THE LOST COLONY
        ModuleGameOver.h
        Date: October, 2007
*/

#ifndef GAMEOVER_H
#define GAMEOVER_H

#include "Module.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

class ModuleGameOver : public Module {
  private:
  public:
    ModuleGameOver(void);
    virtual ~ModuleGameOver(void);
    virtual void Update() override;
    virtual void Draw() override;
    virtual void OnKeyReleased(int keyCode) override;

    bool bQuickShutdown;
};

#endif
