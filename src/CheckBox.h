#ifndef CHECKBOX_H
#define CHECKBOX_H
#include <string>

#include <allegro5/allegro.h>

#include "Button.h"
#include "Events.h"
#include "Module.h"

class CheckBox : public Module {
  public:
    CheckBox(
        int x,
        int y,
        int width,
        int height,
        const std::string &text,
        std::shared_ptr<ALLEGRO_FONT> font,
        ALLEGRO_COLOR color,
        bool checked = false);
    CheckBox(
        int x,
        int y,
        int width,
        const std::string &text,
        std::shared_ptr<ALLEGRO_FONT> font,
        ALLEGRO_COLOR color,
        bool checked = false);
    ~CheckBox() {}

    bool get_checked() const { return m_checked; }
    void set_checked(bool checked);
    virtual bool on_event(ALLEGRO_EVENT *event) override;

  private:
    bool m_checked;
    std::shared_ptr<Module> m_label;
    std::shared_ptr<TextButton> m_button;
};
#endif // CHECKBOX_H
       // vi: ft=cpp
