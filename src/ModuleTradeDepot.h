#ifndef MODULETRADEDEPOT_H
#define MODULETRADEDEPOT_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include "Bitmap.h"
#include "Button.h"
#include "GameState.h"
#include "ItemStackButtonList.h"
#include "Label.h"
#include "Module.h"
#include "ScrolledModule.h"
#include "TextEntry.h"

class TradeDepotAmountChooser : public Module {
  public:
    TradeDepotAmountChooser(int unit_price, int max_units);
    virtual ~TradeDepotAmountChooser() {}

    virtual bool on_event(ALLEGRO_EVENT *event) override;
    virtual bool on_key_pressed(ALLEGRO_KEYBOARD_EVENT *event) override;
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;

    int get_amount() const;
    void set_amount(int amount);

    void set_unit_price(int unit_price) {
        if (unit_price != m_unit_price) {
            m_unit_price = unit_price;
            m_update_total = true;
        }
    }

    void set_max_units(int max_units);

  private:
    std::shared_ptr<Bitmap> m_prompt_background;
    std::shared_ptr<NumericTextEntry> m_text_entry;
    std::shared_ptr<Label> m_price_label;

    std::shared_ptr<Button> m_spin_up_button;
    std::shared_ptr<Button> m_spin_down_button;
    std::shared_ptr<TextButton> m_all_button;
    std::shared_ptr<TextButton> m_ok_button;
    std::shared_ptr<TextButton> m_cancel_button;
    int m_unit_price;
    int m_max_units;

    bool m_update_total;
};

using ScrolledTradeDepotItemList = ScrolledModule<ItemStackButtonList>;

class ModuleTradeDepot : public Module {
  public:
    ModuleTradeDepot();
    virtual ~ModuleTradeDepot();
    virtual bool on_init() override;
    virtual bool on_update() override;
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;
    virtual bool on_close() override;

  private:
    ItemType item_to_display;

    std::map<ItemType, std::shared_ptr<ALLEGRO_BITMAP>> m_item_portraits;
    std::string portrait_string;

    typedef enum
    {
        PT_INVALID = 0,
        PT_SELL,
        PT_BUY
    } PromptType;
    PromptType m_promptType;
    const Item *m_promptItem;

    bool m_clearListSelOnUpdate;

    void DoBuySell(int amount);
    void DoFinalizeTransaction();

    std::shared_ptr<Bitmap> m_background;

    std::shared_ptr<ScrolledTradeDepotItemList> m_player_items;
    std::shared_ptr<ScrolledTradeDepotItemList> m_sell_items;
    std::shared_ptr<ScrolledTradeDepotItemList> m_depot_items;
    std::shared_ptr<ScrolledTradeDepotItemList> m_buy_items;

    std::shared_ptr<TextButton> m_buy_sell_button;
    std::shared_ptr<TextButton> m_exit_button;
    std::shared_ptr<TextButton> m_confirm_button;
    std::shared_ptr<TextButton> m_clear_button;

    std::map<ItemType, std::shared_ptr<TextButton>> m_filter_buttons;

    std::shared_ptr<TradeDepotAmountChooser> m_amount_chooser;
    bool exitToStarportCommons;

    ItemType m_filterType;
    Items m_depotItems;
    Items m_sellItems;
    Items m_buyItems;

    void UpdateButtonStates();
    void UpdateLists();

    int m_sellTotal;
    int m_buyTotal;
};

#endif
// vi: ft=cpp
