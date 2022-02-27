/*
        STARFLIGHT - THE LOST COLONY
        ModuleCredits.h - ?
        Author: Justin Sargent
        Date: 9/21/07
*/

#ifndef MODULECREDITS_H
#define MODULECREDITS_H

#include <memory>

#include "Bitmap.h"
#include "Label.h"
#include "Module.h"

class ModuleCredits : public Module {
  public:
    virtual bool on_init() override;
    virtual bool on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) override;
    virtual bool on_mouse_button_click(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_close() override;

  private:
    std::shared_ptr<Bitmap> m_background;
    std::shared_ptr<Label> m_contributors;
    std::shared_ptr<Label> m_credits_left;
    std::shared_ptr<Label> m_credits_right;
};

#endif
