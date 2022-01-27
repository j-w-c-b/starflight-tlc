/*
        STARFLIGHT - THE LOST COLONY
        ModuleCredits.cpp -
        Author:
        Date:
*/

#include "ModuleCredits.h"
#include "DataMgr.h"
#include "Game.h"
#include "ModeMgr.h"
#include "credits_resources.h"

using namespace std;
using namespace credits_resources;

ALLEGRO_DEBUG_CHANNEL("ModuleCredits")
/*
  This is not elegant or data driven but it meets our needs. The credit list for
  this game is not large so we can update this as needed without exposing a data
  file to end-user manipulation. Roles ended up being printed on left, with
  names of the right; but i left the array as is.
 */
const int numcredits = 36;
string credits[numcredits][2] = {
    {"ARTWORK", "Ronald Conley"},
    {"", "Andrew Chason"},
    {"", ""},
    {"MUSIC", "Chris Hurn"},
    {"", ""},
    {"PROGRAMMING", "David Calkins"},
    {"", "Jon Harbour"},
    {"", "Steven Wirsz"},
    {"", ""},
    {"GAME DESIGN", "Jon Harbour"},
    {"", "David Calkins"},
    {"", ""},
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
    {"", ""},
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

ModuleCredits::ModuleCredits()
    : Module(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT), resources(CREDITS_IMAGES) {
    background = NULL;
}
ModuleCredits::~ModuleCredits() {}

bool
ModuleCredits::on_key_pressed(ALLEGRO_KEYBOARD_EVENT * /*event*/) {
    g_game->LoadModule(MODULE_TITLESCREEN);
    return false;
}

bool
ModuleCredits::on_mouse_button_up(ALLEGRO_MOUSE_EVENT * /*event*/) {
    g_game->LoadModule(MODULE_TITLESCREEN);
    return false;
}

bool
ModuleCredits::on_close() {
    ALLEGRO_DEBUG("Credits Close\n");

    // unload the data file
    resources.unload();
    return true;
}

bool
ModuleCredits::on_init() {
    ALLEGRO_DEBUG("  ModuleCredits Initialize\n");

    // load the datafile
    if (!resources.load()) {
        g_game->message("Credits: Error loading resources");
        return false;
    }

    // Load background
    background = resources[I_BACKGROUND];

    g_game->Print32(background, 390, 10, "CONTRIBUTORS", STEEL, true);

    // print credit lines onto background
    for (int n = 0; n < numcredits; n++) {
        g_game->Print24(
            background, 250, 70 + n * 19, credits[n][0], ORANGE, true);
        g_game->Print24(
            background, 580, 70 + n * 19, credits[n][1], ORANGE, true);
    }

    return true;
}

bool
ModuleCredits::on_draw(ALLEGRO_BITMAP *target) {
    // draw background
    al_set_target_bitmap(target);
    al_draw_bitmap(background, 0, 0, 0);
    return true;
}
