#ifndef CARGO_WINDOW_H
#define CARGO_WINDOW_H

#include "AudioSystem.h"
#include "Button.h"
#include "DataMgr.h"
#include "Label.h"
#include "Module.h"
#include "ResourceManager.h"

/**
 * cargo window
 */
class ModuleCargoWindow : public Module {
  public:
    ModuleCargoWindow();
    virtual ~ModuleCargoWindow();

    virtual bool on_init() override;
    virtual bool on_update() override;
    void UpdateLists();
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;
    virtual bool on_mouse_move(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_mouse_button_down(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_mouse_button_up(ALLEGRO_MOUSE_EVENT *event) override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;
    virtual bool on_close() override;

    bool isVisible(void) { return (m_x > gui_viewer_left && initialized); }

    // Force the window into it's starting (hidden) state.
    void ResetViewer(void);

  private:
    void InitViewer(void);

    // true between end of Init() and start of Close(). false otherwise.
    bool initialized;

    // the window itself
    ALLEGRO_BITMAP *img_viewer;
    int gui_viewer_left;
    int gui_viewer_right;
    int gui_viewer_speed;
    bool sliding;
    int sliding_offset;

    // the window content
    Items *m_items;
    Items *m_playerItemsFiltered;
    ScrollBox::ScrollBox *m_playerList;
    ScrollBox::ScrollBox *m_playerListNumItems;
    ScrollBox::ScrollBox *m_playerListValue;

    Button *m_jettisonButton;
    std::shared_ptr<Sample> m_sndButtonClick;

    Label *m_space_status;
    int maxSpace;
    ResourceManager<ALLEGRO_BITMAP> m_resources;
};

// vi: ft=cpp
#endif /* CARGO_WINDOW_H */
