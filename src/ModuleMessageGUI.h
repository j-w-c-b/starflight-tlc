#ifndef ModuleMessageGUI_H
#define ModuleMessageGUI_H

#include <memory>

#include "Bitmap.h"
#include "Label.h"
#include "Module.h"
#include "Officer.h"
#include "RichTextLabel.h"
#include "ScrolledModule.h"

class ModuleMessageGUI : public Module {
  public:
    virtual bool on_init() override;
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;
    virtual bool on_close() override;
    void printout(
        const std::string &text,
        ALLEGRO_COLOR color = WHITE,
        long delay = 0);
    void clear_printout();

  private:
    void printout(const RichText &text, long delay);

    std::shared_ptr<Bitmap> m_messagewindow;
    std::shared_ptr<Bitmap> m_gui_socket;
    std::shared_ptr<Label> m_stardate;
    std::shared_ptr<ScrolledModule<RichTextLabel>> m_text;

    struct TimedText {
        RichText text;
        long delay;
    };
    std::vector<TimedText> m_messages;
};
#endif
// vi: ft=cpp
