#ifndef MODULE_CARGO_WINDOW_H
#define MODULE_CARGO_WINDOW_H

#include "AudioSystem.h"
#include "Button.h"
#include "DataMgr.h"
#include "ItemStackButtonList.h"
#include "Label.h"
#include "Module.h"
#include "ScrolledModule.h"
#include "SlidingModule.h"

/**
 * cargo window
 */
class ModuleCargoWindow : public Module {
  public:
    ModuleCargoWindow();
    virtual ~ModuleCargoWindow();

    virtual bool on_init() override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;
    virtual bool on_update() override;
    virtual bool on_close() override;

  private:
    using ScrolledItemStackButtonList = ScrolledModule<ItemStackButtonList>;

    void update_inventory();

    std::shared_ptr<SlidingModule<Bitmap>> m_viewer;
    std::shared_ptr<ScrolledItemStackButtonList> m_player_items;
    std::shared_ptr<TextButton> m_jettisonButton;
    std::shared_ptr<Label> m_space_status;
    bool m_is_active;
};
#endif /* MODULE_CARGO_WINDOW_H */
// vi: ft=cpp
