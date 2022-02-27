/*
        STARFLIGHT - THE LOST COLONY
        ModuleMessageGUI.cpp
        Author:
        Date:
*/

#ifndef ModuleMessageGUI_H
#define ModuleMessageGUI_H

#include "Module.h"

class ModuleMessageGUI : public Module {
  public:
    ModuleMessageGUI();
    virtual ~ModuleMessageGUI();
    virtual bool on_init() override;
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;
    virtual bool on_close() override;
};

#endif
// vi: ft=cpp
