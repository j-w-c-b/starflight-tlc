#ifndef _CARGO_WINDOW_H
#define _CARGO_WINDOW_H

#include "AudioSystem.h"
#include "DataMgr.h"
#include "Module.h"
#include "ResourceManager.h"

class Items;
class Button;
class Label;

namespace ScrollBox {
class ScrollBox;
};

/**
 * cargo window
 */
class ModuleCargoWindow : public Module {
  public:
    ModuleCargoWindow();
    virtual ~ModuleCargoWindow();

    virtual bool Init() override;
    virtual void Update() override;
    void UpdateLists();
    virtual void Draw() override;
    virtual void OnMouseMove(int x, int y) override;
    virtual void OnMouseClick(int button, int x, int y) override;
    virtual void OnMousePressed(int button, int x, int y) override;
    virtual void OnMouseReleased(int button, int x, int y) override;
    virtual void OnEvent(Event *event) override;
    virtual void Close() override;
    bool
    isVisible(void) {
        return (m_x > gui_viewer_left && initialized);
    }

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

    Label *spaceStatus;
    int maxSpace;
    ResourceManager<ALLEGRO_BITMAP> m_resources;
};

#endif /* _CARGO_WINDOW_H */
