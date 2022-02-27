/*
        STARFLIGHT - THE LOST COLONY
        ModuleCredits.cpp -
        Author:
        Date:
*/
#include <sstream>

#include "DataMgr.h"
#include "Game.h"
#include "ModeMgr.h"
#include "ModuleCredits.h"
#include "credits_resources.h"

using namespace std;
using namespace credits;

ALLEGRO_DEBUG_CHANNEL("ModuleCredits")
/*
  This is not elegant or data driven but it meets our needs. The credit list for
  this game is not large so we can update this as needed without exposing a data
  file to end-user manipulation. Roles ended up being printed on left, with
  names of the right; but i left the array as is.
 */
static vector<pair<string, string>> s_credits = {
    {"ARTWORK", "Ronald Conley"},
    {"", "Andrew Chason"},
    {"MUSIC", "Chris Hurn"},
    {"PROGRAMMING", "David Calkins"},
    {"", "Jon Harbour"},
    {"", "Steven Wirsz"},
    {"GAME DESIGN", "Jon Harbour"},
    {"", "David Calkins"},
    {"STORY WRITING", "Steven Wirsz"},
    {"", "Jon Harbour"},
    {"", ""},
    {"INSPIRATION", "Rod McConnell"},
    {"", "Alec Kercso"},
    {"", "Greg Johnson"},
    {"", "T.C. Lee"},
    {"", "Bob Gonsalves"},
    {"", "Evan Robinson"},
    {"", ""},
    {"SPECIAL THANKS", ""},
    {"Steve Heyer", "Justin Sargent"},
    {"Nick Busby", "Jonathan Ray"},
    {"Jonathan Verrier", "Matthew Klausmeier"},
    {"David Guardalabene", "Keith Patch"},
    {"Jakob Medlin", "Scott Idler"},
    {"Vincent Cappe", "Steven Kottke"},
    {"Michael Drotar", "Wascal Wabbit"},
    {"Donnie Jason", "Jeff Price"},
    {"Michael Madrio", "William Sherwin"},
    {"Nathan Wright", "Vince Converse"},
    {"Ed Wolinski", "Sophia Wolinski"},
};

bool
ModuleCredits::on_key_pressed(ALLEGRO_KEYBOARD_EVENT * /*event*/) {
    g_game->LoadModule(MODULE_TITLESCREEN);
    return false;
}

bool
ModuleCredits::on_mouse_button_click(ALLEGRO_MOUSE_EVENT * /*event*/) {
    g_game->LoadModule(MODULE_TITLESCREEN);
    return false;
}

bool
ModuleCredits::on_init() {
    ALLEGRO_DEBUG("  ModuleCredits Initialize\n");
    m_background = make_shared<Bitmap>(images[I_BACKGROUND]);
    add_child_module(m_background);

    m_contributors = make_shared<Label>(
        "CONTRIBUTORS",
        390,
        10,
        al_get_text_width(g_game->font32.get(), "CONTRIBUTORS") + 1,
        al_get_font_line_height(g_game->font32.get()) + 1,
        false,
        ALLEGRO_ALIGN_LEFT,
        g_game->font32,
        STEEL,
        nullptr,
        al_map_rgba(0, 0, 0, 0),
        true);
    add_child_module(m_contributors);

    ostringstream os;
    for (auto &i : s_credits) {
        os << i.first << "\n";
    }
    m_credits_left = make_shared<Label>(
        os.str(),
        250,
        70,
        330,
        SCREEN_WIDTH - 70,
        true,
        ALLEGRO_ALIGN_LEFT,
        g_game->font18,
        ORANGE,
        nullptr,
        al_map_rgba(0, 0, 0, 0),
        true);
    add_child_module(m_credits_left);

    os.str("");
    for (auto &i : s_credits) {
        os << i.second << "\n";
    }
    m_credits_right = make_shared<Label>(
        os.str(),
        580,
        70,
        SCREEN_WIDTH - 580,
        SCREEN_WIDTH - 70,
        true,
        ALLEGRO_ALIGN_LEFT,
        g_game->font18,
        ORANGE,
        nullptr,
        al_map_rgba(0, 0, 0, 0),
        true);
    add_child_module(m_credits_right);

    return true;
}

bool
ModuleCredits::on_close() {
    ALLEGRO_DEBUG("Credits Close\n");

    remove_child_module(m_background);
    m_background = nullptr;

    remove_child_module(m_contributors);
    m_contributors = nullptr;

    remove_child_module(m_credits_left);
    m_credits_left = nullptr;

    remove_child_module(m_credits_right);
    m_credits_right = nullptr;

    return true;
}
