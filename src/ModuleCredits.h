/*
        STARFLIGHT - THE LOST COLONY
        ModuleCredits.h - ?
        Author: Justin Sargent
        Date: 9/21/07
*/

#ifndef MODULECREDITS_H
#define MODULECREDITS_H

#include "GameState.h"
#include "Module.h"
#include "ResourceManager.h"

class ModuleCredits : public Module {
  public:
    ModuleCredits();
    virtual ~ModuleCredits();
    virtual bool Init() override;
    virtual void Update() override;
    virtual void Draw() override;
    virtual void OnKeyReleased(int keyCode) override;
    virtual void OnMouseReleased(int button, int x, int y) override;
    virtual void Close() override;

  private:
    ALLEGRO_BITMAP *background;
    ResourceManager<ALLEGRO_BITMAP> resources;
};

#endif
