#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#include "AudioSystem.h"
#include "Button.h"
#include "Events.h"
#include "Game.h"
#include "GameState.h"
#include "MessageBoxWindow.h"
#include "ModeMgr.h"
#include "ModuleTradeDepot.h"
#include "QuestMgr.h"
#include "ScrolledModule.h"
#include "Util.h"
#include "tradedepot_resources.h"

using namespace std;
using namespace tradedepot;

#define PLAYERLIST_X 10
#define PLAYERLIST_Y 240
#define PLAYERLIST_WIDTH 421
#define PLAYERLIST_HEIGHT 166

#define SELLLIST_X 10
#define SELLLIST_Y 580
#define SELLLIST_WIDTH 421
#define SELLLIST_HEIGHT 97

#define DEPOTLIST_X 572
#define DEPOTLIST_Y 240
#define DEPOTLIST_WIDTH 421
#define DEPOTLIST_HEIGHT 166

#define BUYLIST_X 580
#define BUYLIST_Y 580
#define BUYLIST_WIDTH 421
#define BUYLIST_HEIGHT 97

#define PLAYER_BALANCE_X 435
#define PLAYER_BALANCE_Y 188
#define PLAYER_BALANCE_TEXTHEIGHT 48
#define PLAYER_BALANCE_TEXTCOL GREEN

#define SELLTOTAL_X 430
#define SELLTOTAL_Y 524
#define SELLTOTAL_TEXTCOL GREEN

#define BUYTOTAL_X 593
#define BUYTOTAL_Y 524
#define BUYTOTAL_TEXTCOL RED

#define EXITBTN_X 20
#define EXITBTN_Y 698

#define BUTTONS_X 460
#define SELLBUYBTN_Y 330
#define CHECKOUTBTN_Y 386
#define CLEARBTN_Y 623
#define BTNTEXTCOLOR BLACK
#define BTN_TEXTHEIGHT 30

#define FILTERBTN_Y 719
#define FILTERBTN_START_X 304
#define FILTERBTN_DELTA_X 147
#define FILTERBTN_TEXTHEIGHT 20

#define PROMPTBG_X 303
#define PROMPTBG_Y 69
#define SPINUPBTN_X 369
#define SPINUPBTN_Y 22
#define SPINDOWNBTN_X 369
#define SPINDOWNBTN_Y 42
#define ALLBTN_X 20
#define ALLBTN_Y 71
#define OKBTN_X 270
#define OKBTN_Y 71
#define CANCELBTN_X 270
#define CANCELBTN_Y 117
#define QTYTEXT_X 198
#define QTYTEXT_Y 25
#define PRICE_X 20
#define PRICE_Y 137

#define PROMPTBTN_TEXT_COLOR BLACK
#define PROMPT_TEXT_COLOR ORANGE

#define PROMPT_MAX_CHARS 8
#define PROMPT_VAL_TEXTHEIGHT 30
#define CURSOR_DELAY 10
#define CURSOR_Y QTYTEXT_Y

ALLEGRO_DEBUG_CHANNEL("ModuleTradeDepot")

const int ITEM_ENDURIUM = 54;

TradeDepotAmountChooser::TradeDepotAmountChooser(int unit_price, int max_units)
    : Module(
        PROMPTBG_X,
        PROMPTBG_Y,
        al_get_bitmap_width(images[I_TRADEDEPOT_QUANTITY_PROMPT].get()),
        al_get_bitmap_height(images[I_TRADEDEPOT_QUANTITY_PROMPT].get())),
      m_unit_price(unit_price), m_max_units(max_units), m_update_total(true) {
    m_prompt_background = make_shared<Bitmap>(
        images[I_TRADEDEPOT_QUANTITY_PROMPT], PROMPTBG_X, PROMPTBG_Y);
    add_child_module(m_prompt_background);

    m_text_entry = make_shared<NumericTextEntry>(
        g_game->font24,
        PROMPT_TEXT_COLOR,
        "",
        5,
        max_units,
        QTYTEXT_X + PROMPTBG_X,
        QTYTEXT_Y + PROMPTBG_Y,
        190,
        40,
        images[I_TRADEDEPOT_CURSOR0],
        0.4,
        samples[S_BUTTONCLICK],
        samples[S_ERROR]);

    add_child_module(m_text_entry);

    m_price_label = make_shared<Label>(
        "",
        PRICE_X + PROMPTBG_X,
        PRICE_Y + PROMPTBG_Y,
        220,
        al_get_font_line_height(g_game->font24.get()),
        false,
        ALLEGRO_ALIGN_LEFT,
        g_game->font24,
        PROMPTBTN_TEXT_COLOR);
    add_child_module(m_price_label);

    // create spinup buttons
    m_spin_up_button = make_shared<Button>(
        SPINUPBTN_X + PROMPTBG_X,
        SPINUPBTN_Y + PROMPTBG_Y,
        EVENT_NONE,
        EVENT_TRADE_SPIN_UP,
        images[I_TRADEDEPOT_SPINUPBTN],
        images[I_TRADEDEPOT_SPINUPBTN_MO]);
    add_child_module(m_spin_up_button);

    // create spindown buttons
    m_spin_down_button = make_shared<Button>(
        SPINDOWNBTN_X + PROMPTBG_X,
        SPINDOWNBTN_Y + PROMPTBG_Y,
        EVENT_NONE,
        EVENT_TRADE_SPIN_DOWN,
        images[I_TRADEDEPOT_SPINDOWNBTN],
        images[I_TRADEDEPOT_SPINDOWNBTN_MO]);

    add_child_module(m_spin_down_button);

    // create all button
    m_all_button = make_shared<TextButton>(
        "All",
        g_game->font24,
        PROMPTBTN_TEXT_COLOR,
        ALLEGRO_ALIGN_CENTER,
        ALLBTN_X + PROMPTBG_X,
        ALLBTN_Y + PROMPTBG_Y,
        EVENT_NONE,
        EVENT_TRADE_ALL,
        images[I_TRADEDEPOT_PROMPTBTN],
        images[I_TRADEDEPOT_PROMPTBTN_MO]);

    add_child_module(m_all_button);

    // create ok button
    m_ok_button = make_shared<TextButton>(
        "OK",
        g_game->font24,
        PROMPTBTN_TEXT_COLOR,
        ALLEGRO_ALIGN_CENTER,
        OKBTN_X + PROMPTBG_X,
        OKBTN_Y + PROMPTBG_Y,
        EVENT_NONE,
        EVENT_TRADE_OK,
        images[I_TRADEDEPOT_PROMPTBTN],
        images[I_TRADEDEPOT_PROMPTBTN_MO]);

    add_child_module(m_ok_button);

    // create cancel button
    m_cancel_button = make_shared<TextButton>(
        "Cancel",
        g_game->font24,
        PROMPTBTN_TEXT_COLOR,
        ALLEGRO_ALIGN_CENTER,
        CANCELBTN_X + PROMPTBG_X,
        CANCELBTN_Y + PROMPTBG_Y,
        EVENT_NONE,
        EVENT_TRADE_CANCEL,
        images[I_TRADEDEPOT_PROMPTBTN],
        images[I_TRADEDEPOT_PROMPTBTN_MO]);

    add_child_module(m_cancel_button);
}

bool
TradeDepotAmountChooser::on_event(ALLEGRO_EVENT *event) {
    bool play_button_click = false;
    bool play_button_error = false;
    int value = m_text_entry->get_value();

    switch (event->type) {
    case EVENT_TRADE_SPIN_UP:
        if (m_text_entry->set_value(value + 1)) {
            play_button_click = true;
            m_update_total = true;
        } else {
            play_button_error = true;
        }
        break;
    case EVENT_TRADE_SPIN_DOWN:

        if (m_text_entry->set_value(value - 1)) {
            play_button_click = true;
            m_update_total = true;
        } else {
            play_button_error = true;
        }
        break;
    case EVENT_TRADE_ALL:
        if (value < m_max_units) {
            m_text_entry->set_value(m_max_units);
            play_button_click = true;
            m_update_total = true;
        } else {
            play_button_error = true;
        }
        break;
    }
    if (play_button_click) {
        g_game->audioSystem->Play(samples[S_BUTTONCLICK]);
    } else if (play_button_error) {
        g_game->audioSystem->Play(samples[S_ERROR]);
    }
    return !(play_button_click || play_button_error);
}

bool
TradeDepotAmountChooser::on_key_pressed(ALLEGRO_KEYBOARD_EVENT * /*event*/) {
    m_update_total = true;

    return true;
}

bool
TradeDepotAmountChooser::on_draw(ALLEGRO_BITMAP * /*target*/) {
    if (m_update_total) {
        string label_text =
            "Price: " + to_string(m_unit_price * m_text_entry->get_value());
        m_price_label->set_text(label_text);
    }
    return true;
}

int
TradeDepotAmountChooser::get_amount() const {
    return m_text_entry->get_value();
}

void
TradeDepotAmountChooser::set_amount(int amount) {
    m_text_entry->set_value(amount);
    m_update_total = true;
}

void
TradeDepotAmountChooser::set_max_units(int max_units) {
    m_max_units = max_units;
    m_text_entry->set_max_value(max_units);
    m_update_total = true;
}

ModuleTradeDepot::ModuleTradeDepot()
    : m_clearListSelOnUpdate(false), exitToStarportCommons(false),
      m_filterType(IT_INVALID) {
    set_draw_after_children(true);
}

bool
ModuleTradeDepot::on_init() {
    m_background = make_shared<Bitmap>(images[I_TRADEDEPOT_BACKGROUND]);
    add_child_module(m_background);

    // create exit button
    m_exit_button = make_shared<TextButton>(
        "Exit",
        g_game->font24,
        BLACK,
        ALLEGRO_ALIGN_CENTER,
        EXITBTN_X,
        EXITBTN_Y,
        EVENT_NONE,
        EVENT_TRADE_EXIT,
        images[I_GENERIC_EXIT_BTN_NORM],
        images[I_GENERIC_EXIT_BTN_OVER]);

    add_child_module(m_exit_button);

    // create buy/sell button
    m_buy_sell_button = make_shared<TextButton>(
        "",
        g_game->font24,
        BTNTEXTCOLOR,
        ALLEGRO_ALIGN_CENTER,
        BUTTONS_X,
        SELLBUYBTN_Y,
        EVENT_NONE,
        EVENT_TRADE_SELL_BUY,
        images[I_TRADEDEPOT_BTN],
        images[I_TRADEDEPOT_BTN_MO]);
    add_child_module(m_buy_sell_button);
    m_buy_sell_button->set_active(false);

    // create confirm button
    m_confirm_button = make_shared<TextButton>(
        "Confirm",
        g_game->font22,
        BTNTEXTCOLOR,
        ALLEGRO_ALIGN_CENTER,
        BUTTONS_X,
        CHECKOUTBTN_Y,
        EVENT_NONE,
        EVENT_TRADE_CHECKOUT,
        images[I_TRADEDEPOT_BTN],
        images[I_TRADEDEPOT_BTN_MO]);
    add_child_module(m_confirm_button);

    m_confirm_button->set_active(false);

    // create clear button
    m_clear_button = make_shared<TextButton>(
        "Clear",
        g_game->font24,
        BTNTEXTCOLOR,
        ALLEGRO_ALIGN_CENTER,
        BUTTONS_X,
        CLEARBTN_Y,
        EVENT_NONE,
        EVENT_TRADE_CLEAR,
        images[I_TRADEDEPOT_BTN],
        images[I_TRADEDEPOT_BTN_MO]);

    add_child_module(m_clear_button);
    m_clear_button->set_active(false);

    // create all button
    int x = FILTERBTN_START_X;
    auto filter_all_button = make_shared<TextButton>(
        "All",
        g_game->font18,
        BTNTEXTCOLOR,
        ALLEGRO_ALIGN_CENTER,
        x,
        FILTERBTN_Y,
        EVENT_NONE,
        EVENT_TRADE_FILTER_ALL,
        images[I_TRADEDEPOT_FILTERBTN],
        images[I_TRADEDEPOT_FILTERBTN_MO]);

    m_filter_buttons[IT_INVALID] = filter_all_button;
    add_child_module(filter_all_button);

    // create artifacts button
    x += FILTERBTN_DELTA_X;
    auto filter_artifacts_button = make_shared<TextButton>(
        "Artifacts",
        g_game->font18,
        BTNTEXTCOLOR,
        ALLEGRO_ALIGN_CENTER,
        x,
        FILTERBTN_Y,
        EVENT_NONE,
        EVENT_TRADE_FILTER_ARTIFACT,
        images[I_TRADEDEPOT_FILTERBTN],
        images[I_TRADEDEPOT_FILTERBTN_MO]);

    m_filter_buttons[IT_ARTIFACT] = filter_artifacts_button;
    add_child_module(filter_artifacts_button);

    // create minerals button
    x += FILTERBTN_DELTA_X;
    auto filter_minerals_button = make_shared<TextButton>(
        "Minerals",
        g_game->font18,
        BTNTEXTCOLOR,
        ALLEGRO_ALIGN_CENTER,
        x,
        FILTERBTN_Y,
        EVENT_NONE,
        EVENT_TRADE_FILTER_MINERAL,
        images[I_TRADEDEPOT_FILTERBTN],
        images[I_TRADEDEPOT_FILTERBTN_MO]);

    m_filter_buttons[IT_MINERAL] = filter_minerals_button;
    add_child_module(filter_minerals_button);

    // create lifeforms button
    x += FILTERBTN_DELTA_X;
    auto filter_life_forms_button = make_shared<TextButton>(
        "Lifeforms",
        g_game->font18,
        BTNTEXTCOLOR,
        ALLEGRO_ALIGN_CENTER,
        x,
        FILTERBTN_Y,
        EVENT_NONE,
        EVENT_TRADE_FILTER_LIFEFORM,
        images[I_TRADEDEPOT_FILTERBTN],
        images[I_TRADEDEPOT_FILTERBTN_MO]);

    m_filter_buttons[IT_LIFEFORM] = filter_life_forms_button;
    add_child_module(filter_life_forms_button);

    // create trade items button
    x += FILTERBTN_DELTA_X;
    auto filter_trade_item_button = make_shared<TextButton>(
        "Trade Items",
        g_game->font18,
        BTNTEXTCOLOR,
        ALLEGRO_ALIGN_CENTER,
        x,
        FILTERBTN_Y,
        EVENT_NONE,
        EVENT_TRADE_FILTER_TRADE_ITEM,
        images[I_TRADEDEPOT_FILTERBTN],
        images[I_TRADEDEPOT_FILTERBTN_MO]);

    m_filter_buttons[IT_TRADEITEM] = filter_trade_item_button;
    add_child_module(filter_trade_item_button);

    m_amount_chooser = make_shared<TradeDepotAmountChooser>(0, 0);

    item_to_display = IT_INVALID;
    portrait_string = "";

    m_item_portraits[IT_ARTIFACT] = images[I_T_ALIENARTIFACT];
    m_item_portraits[IT_MINERAL] = images[I_T_GEMS];
    m_item_portraits[IT_LIFEFORM] = images[I_T_RABID_VERTRUK];
    m_item_portraits[IT_TRADEITEM] = images[I_T_SEEDS];

    exitToStarportCommons = false;

    g_game->gameState->m_credits = g_game->gameState->getCredits();

    // empty the trade depot
    m_depotItems.Reset();

    // there must always be a supply of endurium!
    m_depotItems.SetItemCount(ITEM_ENDURIUM, Util::Random(1, 6) + 6);

    // add ship repair metals
    m_depotItems.SetItemCount(32, Util::Random(1, 6) + 6); // cobalt
    m_depotItems.SetItemCount(36, Util::Random(1, 6) + 6); // molybdenum
    m_depotItems.SetItemCount(39, Util::Random(1, 6) + 6); // aluminum
    m_depotItems.SetItemCount(40, Util::Random(1, 6) + 6); // titanium
    m_depotItems.SetItemCount(44, Util::Random(1, 6) + 6); // silica

    m_player_items = make_shared<ScrolledTradeDepotItemList>(
        PLAYERLIST_X,
        PLAYERLIST_Y,
        PLAYERLIST_WIDTH,
        PLAYERLIST_HEIGHT,
        al_get_font_line_height(g_game->font22.get()),
        al_map_rgb(64, 64, 64),
        al_map_rgb(32, 32, 32),
        al_map_rgb(48, 48, 128),
        al_map_rgb(96, 96, 128),
        al_map_rgb(32, 32, 32),
        g_game->gameState->m_items,
        m_filterType,
        EVENT_TRADE_PLAYER_LIST,
        g_game->font22,
        WHITE,
        al_map_rgb(32, 32, 32),
        al_map_rgb(48, 48, 128),
        al_map_rgb(96, 96, 128));
    add_child_module(m_player_items);

    m_sell_items = make_shared<ScrolledTradeDepotItemList>(
        SELLLIST_X,
        SELLLIST_Y,
        SELLLIST_WIDTH,
        SELLLIST_HEIGHT,
        al_get_font_line_height(g_game->font22.get()),
        al_map_rgb(64, 64, 64),
        al_map_rgb(32, 32, 32),
        al_map_rgb(48, 48, 128),
        al_map_rgb(96, 96, 128),
        al_map_rgb(32, 32, 32),
        m_sellItems,
        IT_INVALID,
        EVENT_TRADE_SELL_LIST,
        g_game->font22,
        WHITE,
        al_map_rgb(32, 32, 32),
        al_map_rgb(48, 48, 128),
        al_map_rgb(96, 96, 128));
    add_child_module(m_sell_items);

    m_depot_items = make_shared<ScrolledTradeDepotItemList>(
        DEPOTLIST_X,
        DEPOTLIST_Y,
        DEPOTLIST_WIDTH,
        DEPOTLIST_HEIGHT,
        al_get_font_line_height(g_game->font22.get()),
        al_map_rgb(64, 64, 64),
        al_map_rgb(32, 32, 32),
        al_map_rgb(48, 48, 128),
        al_map_rgb(96, 96, 128),
        al_map_rgb(32, 32, 32),
        m_depotItems,
        m_filterType,
        EVENT_TRADE_DEPOT_LIST,
        g_game->font22,
        WHITE,
        al_map_rgb(32, 32, 32),
        al_map_rgb(48, 48, 128),
        al_map_rgb(96, 96, 128));

    add_child_module(m_depot_items);

    m_buy_items = make_shared<ScrolledTradeDepotItemList>(
        BUYLIST_X,
        BUYLIST_Y,
        BUYLIST_WIDTH,
        BUYLIST_HEIGHT,
        al_get_font_line_height(g_game->font22.get()),
        al_map_rgb(64, 64, 64),
        al_map_rgb(32, 32, 32),
        al_map_rgb(48, 48, 128),
        al_map_rgb(96, 96, 128),
        al_map_rgb(32, 32, 32),
        m_buyItems,
        m_filterType,
        EVENT_TRADE_BUY_LIST,
        g_game->font22,
        WHITE,
        al_map_rgb(32, 32, 32),
        al_map_rgb(48, 48, 128),
        al_map_rgb(96, 96, 128));

    add_child_module(m_buy_items);

    m_sellTotal = 0;
    m_buyTotal = 0;

    // reset transactions
    m_sellItems.Reset();
    m_buyItems.Reset();

    // refresh lists
    UpdateLists();
    UpdateButtonStates();

    // tell questmgr that this module has been visited
    g_game->questMgr->raiseEvent(20);

    return true;
}

bool
ModuleTradeDepot::on_update() {
    if (m_clearListSelOnUpdate) {
        m_player_items->clear_selected();
        m_depot_items->clear_selected();
        m_sell_items->clear_selected();
        m_buy_items->clear_selected();

        m_clearListSelOnUpdate = false;
    }

    if (exitToStarportCommons) {
        if (g_game->audioSystem->IsPlaying(samples[S_BUTTONCLICK])
            == false) { // switched to named sound: see debug log.
            g_game->LoadModule(MODULE_STARPORT);
            return false;
        }
    }
    return true;
}

bool
ModuleTradeDepot::on_draw(ALLEGRO_BITMAP *target) {
    al_set_target_bitmap(target);

    ostringstream balStr;
    balStr << g_game->gameState->getCredits();
    al_draw_text(
        g_game->font48.get(),
        PLAYER_BALANCE_TEXTCOL,
        PLAYER_BALANCE_X,
        PLAYER_BALANCE_Y,
        ALLEGRO_ALIGN_RIGHT,
        balStr.str().c_str());

    if (m_sellTotal > 0) {
        ostringstream sellStr;
        sellStr << m_sellTotal;
        al_draw_text(
            g_game->font48.get(),
            SELLTOTAL_TEXTCOL,
            SELLTOTAL_X,
            SELLTOTAL_Y,
            ALLEGRO_ALIGN_RIGHT,
            to_string(m_sellTotal).c_str());
    }

    if (m_buyTotal > 0) {
        al_draw_text(
            g_game->font48.get(),
            BUYTOTAL_TEXTCOL,
            BUYTOTAL_X,
            BUYTOTAL_Y,
            0,
            to_string(m_buyTotal).c_str());
    }

    // draw portrait
    // case logic
    // 447,443
    if (portrait_string == "") {
        std::shared_ptr<ALLEGRO_BITMAP> portrait =
            m_item_portraits[item_to_display];
        if (portrait) {
            al_draw_bitmap(portrait.get(), 447, 443, 0);
        }
    } else {
        ALLEGRO_BITMAP *temp_bmp;
        std::string temp_string =
            Util::resource_path("data/tradedepot/" + portrait_string);
        temp_bmp = al_load_bitmap(temp_string.c_str());
        if (temp_bmp) {
            al_draw_bitmap(temp_bmp, 447, 443, 0);
            al_destroy_bitmap(temp_bmp);
        } else {
            string s = "TradeDepot: ERROR! " + temp_string + " not found!";
            ALLEGRO_DEBUG("%s\n", s.c_str());
        }
    }
    return true;
}

bool
ModuleTradeDepot::on_close() {
    remove_child_module(m_background);
    m_background = nullptr;
    remove_child_module(m_exit_button);
    m_exit_button = nullptr;
    remove_child_module(m_buy_sell_button);
    m_buy_sell_button = nullptr;
    remove_child_module(m_confirm_button);
    m_confirm_button = nullptr;
    remove_child_module(m_clear_button);
    m_clear_button = nullptr;

    for (auto &i : m_filter_buttons) {
        remove_child_module(i.second);
    }
    m_filter_buttons.clear();

    set_modal_child(nullptr);
    m_amount_chooser = nullptr;

    remove_child_module(m_player_items);
    m_player_items = nullptr;

    remove_child_module(m_depot_items);
    m_depot_items = nullptr;

    remove_child_module(m_sell_items);
    m_sell_items = nullptr;

    remove_child_module(m_buy_items);
    m_buy_items = nullptr;

    return true;
}

bool
ModuleTradeDepot::on_event(ALLEGRO_EVENT *event) {
    bool playBtnClick = false;
    int ret = true;
    const Item *item;

    switch (event->type) {
    case EVENT_TRADE_PLAYER_LIST:
        // clear other list selections
        m_depot_items->clear_selected();
        m_sell_items->clear_selected();
        m_buy_items->clear_selected();
        playBtnClick = true;

        // determine item type of selected item so that we can show the correct
        // portrait
        item = g_game->dataMgr->GetItemByID(m_player_items->get_selected());
        item_to_display = item->itemType;
        portrait_string = item->portrait;
        ret = false;
        break;
    case EVENT_TRADE_DEPOT_LIST:
        // clear other list selections
        m_player_items->clear_selected();
        m_sell_items->clear_selected();
        m_buy_items->clear_selected();
        playBtnClick = true;

        // determine item type of selected item so that we can show the correct
        // portrait
        item = g_game->dataMgr->GetItemByID(m_depot_items->get_selected());
        item_to_display = item->itemType;
        portrait_string = item->portrait;
        ret = false;
        break;
    case EVENT_TRADE_SELL_LIST:
        m_sell_items->clear_selected();
        ret = false;
        break;
    case EVENT_TRADE_BUY_LIST:
        m_buy_items->clear_selected();
        ret = false;
        break;
    case EVENT_TRADE_SELL_BUY:
        ID selected_item;

        if ((selected_item = m_player_items->get_selected()) >= 0) {
            int num_items = m_player_items->get_count(selected_item);
            item = g_game->dataMgr->GetItemByID(selected_item);

            m_promptItem = item;
            m_promptType = PT_SELL;
            m_amount_chooser->set_unit_price(item->value);
            m_amount_chooser->set_max_units(num_items);
            m_amount_chooser->set_amount(1);
            set_modal_child(m_amount_chooser);
        } else if ((selected_item = m_depot_items->get_selected()) >= 0) {
            int num_items = m_depot_items->get_count(selected_item);
            item = g_game->dataMgr->GetItemByID(selected_item);

            m_promptItem = item;
            m_promptType = PT_BUY;
            int available_credits =
                g_game->gameState->getCredits() + m_sellTotal - m_buyTotal;
            Item already_in_cart_item;
            int already_in_cart_amount = 0;

            m_buyItems.Get_Item_By_ID(
                item->id, already_in_cart_item, already_in_cart_amount);
            if (already_in_cart_amount) {
                available_credits +=
                    static_cast<int>(item->value * already_in_cart_amount);
            }
            int max_to_buy = available_credits / item->value;

            if (max_to_buy > num_items) {
                max_to_buy = num_items;
            }
            int available_space = g_game->gameState->m_ship.getAvailableSpace();
            if (max_to_buy > available_space) {
                max_to_buy = available_space;
            }

            m_amount_chooser->set_unit_price(item->value);
            m_amount_chooser->set_max_units(max_to_buy);
            m_amount_chooser->set_amount(0);
            set_modal_child(m_amount_chooser);
        }

        playBtnClick = true;
        ret = false;
        break;
    case EVENT_TRADE_EXIT:
        exitToStarportCommons = true;
        playBtnClick = true;
        ret = false;
        break;
    case EVENT_TRADE_CHECKOUT:
        DoFinalizeTransaction();
        playBtnClick = true;
        ret = false;
        break;
    case EVENT_TRADE_CLEAR:
        m_sellItems.Reset();
        m_buyItems.Reset();
        UpdateLists();

        playBtnClick = true;
        ret = false;
        break;
    case EVENT_TRADE_FILTER_ALL:
        m_filterType = IT_INVALID;
        playBtnClick = true;
        UpdateLists();
        m_player_items->set_filter(m_filterType);
        m_player_items->scroll_to_top();

        m_depot_items->set_filter(m_filterType);
        m_depot_items->scroll_to_top();
        ret = false;
        break;
    case EVENT_TRADE_FILTER_ARTIFACT:
        m_filterType = IT_ARTIFACT;
        playBtnClick = true;
        UpdateLists();
        m_player_items->set_filter(m_filterType);
        m_player_items->scroll_to_top();

        m_depot_items->set_filter(m_filterType);
        m_depot_items->scroll_to_top();
        ret = false;
        break;
    case EVENT_TRADE_FILTER_MINERAL:
        m_filterType = IT_MINERAL;
        playBtnClick = true;
        UpdateLists();
        m_player_items->set_filter(m_filterType);
        m_player_items->scroll_to_top();

        m_depot_items->set_filter(m_filterType);
        m_depot_items->scroll_to_top();
        ret = false;
        break;
    case EVENT_TRADE_FILTER_LIFEFORM:
        m_filterType = IT_LIFEFORM;
        playBtnClick = true;
        UpdateLists();
        m_player_items->set_filter(m_filterType);
        m_player_items->scroll_to_top();

        m_depot_items->set_filter(m_filterType);
        m_depot_items->scroll_to_top();
        ret = false;
        break;
    case EVENT_TRADE_FILTER_TRADE_ITEM:
        m_filterType = IT_TRADEITEM;
        playBtnClick = true;
        UpdateLists();
        m_player_items->set_filter(m_filterType);
        m_player_items->scroll_to_top();

        m_depot_items->set_filter(m_filterType);
        m_depot_items->scroll_to_top();
        ret = false;
        break;
    case EVENT_TRADE_OK:
        DoBuySell(m_amount_chooser->get_amount());
        playBtnClick = true;
        m_clearListSelOnUpdate = true;
        set_modal_child(nullptr);
        ret = false;
        break;
    case EVENT_TRADE_CANCEL:
        playBtnClick = true;
        m_clearListSelOnUpdate = true;
        set_modal_child(nullptr);
        ret = false;
        break;
    }

    if (playBtnClick) {
        g_game->audioSystem->Play(samples[S_BUTTONCLICK]);
    }

    UpdateButtonStates();
    return ret;
}

void
ModuleTradeDepot::DoBuySell(int qty) {
    if (qty <= 0)
        return;

    if (m_promptType == PT_SELL) {
        m_sellItems.SetItemCount(m_promptItem->id, qty);
        UpdateLists();
    } else if (m_promptType == PT_BUY) {
        // verify the ship has cargo pods
        if (g_game->gameState->m_ship.getCargoPodCount() == 0) {
            set_modal_child(make_shared<MessageBoxWindow>(
                "",
                "Your ship has no Cargo Pods! You must purchase one before "
                "taking on cargo."));
            return;
        }

        m_buyItems.SetItemCount(m_promptItem->id, qty);
        UpdateLists();
    }
}

void
ModuleTradeDepot::DoFinalizeTransaction() {
    Item item;
    int numItems;

    // we sell first since it will bring both credits and cargo slots
    for (int i = 0; i < m_sellItems.GetNumStacks(); i++) {
        m_sellItems.GetStack(i, item, numItems);
        g_game->gameState->m_credits += (int)(item.value * numItems);
        g_game->gameState->m_items.RemoveItems(item.id, numItems);
        m_depotItems.AddItems(item.id, numItems);
    }
    m_sellItems.Reset();
    m_player_items->scroll_to_top();
    UpdateLists();

    // verify credit balance & cargo space before buying
    int numstacks = m_buyItems.GetNumStacks();
    if (numstacks == 0)
        return;
    int buyValue = 0, neededSpace = 0;

    for (int i = 0; i < numstacks; i++) {
        m_buyItems.GetStack(i, item, numItems);
        buyValue += (int)(item.value * numItems);
        if (!item.IsArtifact())
            neededSpace += numItems;
    }

    if (buyValue > g_game->gameState->m_credits) {
        set_modal_child(
            make_shared<MessageBoxWindow>("", "You can't afford it!"));
        return;
    }

    if (neededSpace > g_game->gameState->m_ship.getAvailableSpace()) {
        set_modal_child(make_shared<MessageBoxWindow>(
            "", "You don't have enough free space in your cargo hold!"));
        return;
    }

    // ok, we have enough money and cargo space: we pay
    g_game->gameState->m_credits -= buyValue;

    // the transaction took place; clear the list
    for (int i = 0; i < numstacks; i++) {
        m_buyItems.GetStack(i, item, numItems);
        m_depotItems.RemoveItems(item.id, numItems);
        g_game->gameState->m_items.AddItems(item.id, numItems);
    }
    m_buyItems.Reset();
    m_depot_items->scroll_to_top();
    UpdateLists();
}

void
ModuleTradeDepot::UpdateButtonStates() {
    m_confirm_button->set_active(false);
    m_buy_sell_button->set_active(false);
    m_clear_button->set_active(false);

    if (m_player_items->get_selected() >= 0) {
        m_buy_sell_button->set_text("Sell");
        m_buy_sell_button->set_active(true);
    }

    if (m_depot_items->get_selected() >= 0) {
        m_buy_sell_button->set_text("Buy");
        m_buy_sell_button->set_active(true);
    }

    if (m_sell_items->get_num_rows() > 0) {
        m_confirm_button->set_active(true);
        m_clear_button->set_active(true);
    }

    if (m_buy_items->get_num_rows() > 0) {
        m_confirm_button->set_active(true);
        m_clear_button->set_active(true);
    }

    m_filter_buttons[IT_INVALID]->set_highlight(false);
    m_filter_buttons[IT_ARTIFACT]->set_highlight(false);
    m_filter_buttons[IT_MINERAL]->set_highlight(false);
    m_filter_buttons[IT_LIFEFORM]->set_highlight(false);
    m_filter_buttons[IT_TRADEITEM]->set_highlight(false);

    m_filter_buttons[m_filterType]->set_highlight(true);
}

void
ModuleTradeDepot::UpdateLists() {
    // player items
    m_player_items->clear_selected();
    m_player_items->set_filter(m_filterType);
    m_player_items->update_items();

    // depot items
    m_depot_items->clear_selected();
    m_depot_items->set_filter(m_filterType);
    m_depot_items->update_items();

    // sell items
    int sellValue = 0;
    m_sell_items->update_items();

    for (auto &[id, count] : m_sellItems) {
        if (count > 0) {
            const Item *item = g_game->dataMgr->GetItemByID(id);
            sellValue += static_cast<int>(item->value * count);
        }
    }

    // buy items
    int buyValue = 0;
    m_buy_items->update_items();

    for (auto &[id, count] : m_buyItems) {
        if (count > 0) {
            const Item *item = g_game->dataMgr->GetItemByID(id);
            buyValue += static_cast<int>(item->value * count);
        }
    }

    // update net total values
    m_sellTotal = 0;
    m_buyTotal = 0;
    if (sellValue > buyValue) {
        m_sellTotal = sellValue - buyValue;
    } else {
        m_buyTotal = buyValue - sellValue;
    }
}
// vi: ft=cpp
