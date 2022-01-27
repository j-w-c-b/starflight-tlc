/*
        STARFLIGHT - THE LOST COLONY
        ModuleAuxiliaryDisplay.cpp
        Author: J.Harbour
        Date: Jan 2008

        Engineer & Doctor: Keith Patch

        General status displayed for all crew:
                * DATE
                * DAMAGE
                * CARGO
                * ENERGY
                * SHIELDS
                * WEAPONS
                * CREW NAME

        Additional status info for each crew:

        Captain:
                * SHIP NAME
                * SHIP TYPE
                * CREDITS

        Science:

        Navigation:
                * COORD
                * REGION
                * SPEED
                * MINIMAP

        Tactical:
                * SHIELD CLASS
                * LASER CLASS
                * MISSILE CLASS

        Engineering:
                * ENGINE CLASS

        Communications:

        Medical:


*/

#include <sstream>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "AudioSystem.h"
#include "Button.h"
#include "DataMgr.h"
#include "Events.h"
#include "Flux.h"
#include "Game.h"
#include "GameState.h"
#include "ModeMgr.h"
#include "ModuleAuxiliaryDisplay.h"
#include "Script.h"
#include "Sprite.h"
#include "Util.h"
#include "auxiliary_resources.h"

using namespace std;
using namespace auxiliary_resources;

ALLEGRO_DEBUG_CHANNEL("ModuleAuxiliaryDisplay")

ModuleAuxiliaryDisplay::ModuleAuxiliaryDisplay()
    : Module(
        static_cast<int>(g_game->getGlobalNumber("AUX_SCREEN_X")),
        static_cast<int>(g_game->getGlobalNumber("AUX_SCREEN_Y")),
        static_cast<int>(g_game->getGlobalNumber("AUX_SCREEN_WIDTH")),
        static_cast<int>(g_game->getGlobalNumber("AUX_SCREEN_HEIGHT"))),
      resources(AUXILIARY_IMAGES) {}
ModuleAuxiliaryDisplay::~ModuleAuxiliaryDisplay() {}

bool
ModuleAuxiliaryDisplay::on_event(ALLEGRO_EVENT *event) {
    switch (event->type) {
    case EVENT_CARGO_UPDATE:
        updateCargoFillPercent();
        break;
    }
    return true;
}

bool
ModuleAuxiliaryDisplay::on_close() {
    delete scroller;

    resources.unload();
    return true;
}

bool
ModuleAuxiliaryDisplay::on_init() {
    ALLEGRO_DEBUG("  ModuleAuxiliaryDisplay Initialize\n");

    // load the datafile
    if (!resources.load()) {
        g_game->message("Auxiliary: Error loading resources");
        return false;
    }

    // create a new color
    HEADING_COLOR = al_map_rgb(0, 168, 168);

    canvas = g_game->GetBackBuffer();

    // get aux display location
    gax = (int)g_game->getGlobalNumber("GUI_AUX_POS_X");
    gay = (int)g_game->getGlobalNumber("GUI_AUX_POS_Y");

    // get position of screen within gui
    asx = (int)g_game->getGlobalNumber("AUX_SCREEN_X");
    asy = (int)g_game->getGlobalNumber("AUX_SCREEN_Y");
    asw = (int)g_game->getGlobalNumber("AUX_SCREEN_WIDTH");
    ash = (int)g_game->getGlobalNumber("AUX_SCREEN_HEIGHT");

    // load the aux gui
    img_aux = resources[I_GUI_AUX];
    if (!img_aux) {
        g_game->message("Aux: Error loading gui_aux");
        return false;
    }

    // load ship status icon
    ship_icon_image = NULL;
    switch (g_game->gameState->getProfession()) {
    case PROFESSION_FREELANCE:
        ship_icon_image = resources[I_AUX_ICON_FREELANCE];
        break;
    case PROFESSION_MILITARY:
        ship_icon_image = resources[I_AUX_ICON_MILITARY];
        break;
    case PROFESSION_SCIENTIFIC:
        ship_icon_image = resources[I_AUX_ICON_SCIENCE];
        break;

    default:
        ALLEGRO_ASSERT(0);
    }
    if (!ship_icon_image) {
        g_game->message("Aux: error loading ship image");
        return false;
    }

    // create ship status icon sprite
    ship_icon_sprite = new Sprite();
    ship_icon_sprite->setImage(ship_icon_image);

    init_nav();

    updateCargoFillPercent();

    return true;
}

void
ModuleAuxiliaryDisplay::updateCargoFillPercent() {
    double totalSpace = g_game->gameState->m_ship.getTotalSpace();
    double occupiedSpace = g_game->gameState->m_ship.getOccupiedSpace();
    cargoFillPercent = (int)(occupiedSpace / totalSpace * 100.0);
}

// The mini-scroller displayed in the navigator's aux display
void
ModuleAuxiliaryDisplay::init_nav() {
    TileSet ts(resources[I_IS_TILES_SMALL], 16, 16, 5, 2);
    // center of the viewport minus 1/2 the tile width
    Point2D tile_offset((80.0 - 8) / 2, (75.0 - 8) / 2);
    scroller = new TileScroller(ts, 250, 220, 80, 75, tile_offset);
    scroller->set_scroll_position(
        g_game->gameState->getHyperspaceCoordinates());

    // set specific tiles in the scrolling tilemap with star data from DataMgr
    for (int i = 0; i < g_game->dataMgr->GetNumStars(); i++) {
        const Star *star = g_game->dataMgr->GetStar(i);
        scroller->set_tile(star->x, star->y, star->spectralClass);
    }
}

void
ModuleAuxiliaryDisplay::updateAll() {
    string label;
    Ship ship = g_game->gameState->getShip();
    Stardate date = g_game->gameState->stardate;
    int x = asx, y = asy;

    if (g_game->gameState->getCurrentSelectedOfficer() != OFFICER_MEDICAL
        && g_game->gameState->getCurrentSelectedOfficer() != OFFICER_ENGINEER) {
        int vx;
        // stardate
        al_draw_text(g_game->font18, HEADING_COLOR, x, y, 0, "DATE: ");
        vx = x + al_get_text_width(g_game->font18, "DATE: ");

        al_draw_text(
            g_game->font18,
            SKYBLUE,
            vx,
            y,
            0,
            date.GetFullDateString().c_str());

        // damage status
        y += 20;
        al_draw_text(g_game->font12, HEADING_COLOR, x, y, 0, "DAMAGE: ");
        vx = x + al_get_text_width(g_game->font12, "DAMAGE: ");

        // FIXME: This is always 0
        int damage = 0;
        ALLEGRO_COLOR damage_color = SKYBLUE;
        if (damage > 66) {
            label = "HEAVY";
            damage_color = al_map_rgb(240, 0, 0); // red
        } else if (damage > 33) {
            label = "MODERATE";
            damage_color = al_map_rgb(240, 240, 0); // yellow
        } else if (damage > 0) {
            label = "LIGHT";
            damage_color = al_map_rgb(0, 200, 0); // green
        } else {
            label = "NONE";
        }
        al_draw_text(g_game->font12, damage_color, vx, y, 0, label.c_str());

        // cargo status
        x += 75;
        y += 20;
        al_draw_text(g_game->font18, HEADING_COLOR, x, y, 0, "CARGO: ");
        vx = x + al_get_text_width(g_game->font18, "CARGO: ");
        al_draw_textf(
            g_game->font18, SKYBLUE, vx, y, 0, "%d%%", cargoFillPercent);

        // fuel status
        y += 20;
        al_draw_text(g_game->font18, HEADING_COLOR, x, y, 0, "ENERGY: ");
        vx = x + al_get_text_width(g_game->font18, "ENERGY: ");
        int fuel = g_game->gameState->m_ship.getEnduriumOnBoard();
        al_draw_textf(g_game->font18, SKYBLUE, vx, y, 0, "%d", fuel);

        // shield status
        y += 20;
        al_draw_text(g_game->font18, HEADING_COLOR, x, y, 0, "SHLDS: ");
        vx = x + al_get_text_width(g_game->font18, "SHLDS: ");
        if (g_game->gameState->getShieldStatus())
            label = "RAISED";
        else
            label = "LOWERED";
        al_draw_text(g_game->font12, SKYBLUE, vx, y + 6, 0, label.c_str());

        // weapon status
        y += 20;
        al_draw_text(g_game->font18, HEADING_COLOR, x, y, 0, "WPNS: ");
        vx = x + al_get_text_width(g_game->font18, "WPNS: ");
        if (g_game->gameState->getWeaponStatus())
            label = "ARMED";
        else
            label = "UNARMED";
        al_draw_text(g_game->font12, SKYBLUE, vx, y + 6, 0, label.c_str());

        // ship icon image
        ship_icon_sprite->setPos(asx + 18, asy + 45);
        ship_icon_sprite->drawframe(g_game->GetBackBuffer());

        // shield bar is 48 pixels tall
        int shield = ship.getShieldClass();
        al_draw_filled_rectangle(
            asx + 2, asy + 95, asx + 12, asy + 95 - shield * 8, RED);
        al_draw_rectangle(asx + 2, asy + 95, asx + 12, asy + 95 - 48, STEEL, 1);
        g_game->Print18(canvas, asx + 2, asy + 96, "S", STEEL);

        // armor bar is 48 pixels tall
        int armor = ship.getArmorClass();
        al_draw_filled_rectangle(
            asx + 56, asy + 95, asx + 66, asy + 95 - armor * 8, YELLOW);
        al_draw_rectangle(
            asx + 56, asy + 95, asx + 66, asy + 95 - 48, STEEL, 1);
        g_game->Print18(canvas, asx + 56, asy + 96, "A", STEEL);
    }
}

void
ModuleAuxiliaryDisplay::updateCap() {
    ALLEGRO_COLOR HEADING_COLOR = al_map_rgb(0, 168, 168);
    Ship ship = g_game->gameState->getShip();
    string label;
    ProfessionType profession;
    int x = asx, y = asy + 130;
    int dx;

    // captain's name
    al_draw_textf(
        g_game->font18,
        SKYBLUE,
        x,
        y,
        0,
        "Captain %s",
        g_game->gameState->officerCap->getLastName().c_str());

    // ship name
    y += 32;
    g_game->Print18(canvas, x, y, "SHIP:", HEADING_COLOR);
    dx = al_get_text_width(g_game->font18, "SHIP: ");
    g_game->Print18(canvas, x + dx, y, "MSS " + ship.getName(), SKYBLUE);

    // ship type
    y += 20;
    g_game->Print18(canvas, x, y, "TYPE:", HEADING_COLOR);
    dx = al_get_text_width(g_game->font18, "TYPE: ");
    profession = g_game->gameState->getProfession();
    switch (profession) {
    case PROFESSION_SCIENTIFIC:
        label = "SCIENTIFIC";
        break;
    case PROFESSION_FREELANCE:
        label = "FREELANCE";
        break;
    case PROFESSION_MILITARY:
        label = "MILITARY";
        break;
    default:
        label = "UNKNOWN";
        break;
    }
    g_game->Print18(canvas, x + dx, y, label, SKYBLUE);

    // credits
    y += 20;
    g_game->Print18(canvas, x, y, "CREDITS: ", HEADING_COLOR);
    dx = al_get_text_width(g_game->font18, "CREDITS: ");
    al_draw_textf(
        g_game->font18,
        SKYBLUE,
        x + dx,
        y,
        0,
        "%d",
        g_game->gameState->getCredits());
}

void
ModuleAuxiliaryDisplay::updateSci() {
    std::ostringstream os;
    int x = asx, y = asy + 130;

    os << "Sci Off. " << g_game->gameState->officerSci->getLastName();
    g_game->Print18(canvas, x, y, os.str(), SKYBLUE);
}

void
ModuleAuxiliaryDisplay::updateNav() {
    ALLEGRO_COLOR HEADING_COLOR = al_map_rgb(0, 168, 168);
    int x = asx, y = asy + 130;
    int dx;

    Point2D position = g_game->gameState->getHyperspaceCoordinates();

    // officer name
    al_draw_textf(
        g_game->font18,
        SKYBLUE,
        x,
        y,
        0,
        "Nav Off. %s",
        g_game->gameState->officerNav->getLastName().c_str());

    // galactic location
    y += 40;
    g_game->Print12(canvas, x, y, "COORD:", HEADING_COLOR);
    dx = al_get_text_width(g_game->font12, "COORD: ");
    al_draw_textf(
        g_game->font12,
        SKYBLUE,
        x + dx,
        y,
        0,
        "%0.f %0.f",
        position.x,
        position.y);

    // speed status
    y += 12;
    g_game->Print12(canvas, x, y, "SPEED:", HEADING_COLOR);
    dx = al_get_text_width(g_game->font12, "SPEED: ");
    al_draw_textf(
        g_game->font12,
        SKYBLUE,
        x + dx,
        y,
        0,
        "%.1f",
        g_game->gameState->player->getCurrentSpeed());

    // galactic region (alien space)
    AlienRaces race = g_game->gameState->player->getGalacticRegion();
    string race_str = g_game->gameState->player->getAlienRaceName(race);
    if (race != ALIEN_NONE) {
        y += 12;
        g_game->Print12(canvas, x, y, "REGION: ", HEADING_COLOR);
        dx = al_get_text_width(g_game->font12, "REGION: ");
        g_game->Print12(canvas, x + dx, y, race_str, SKYBLUE);
    }

    scroller->set_scroll_position(
        g_game->gameState->getHyperspaceCoordinates());
    scroller->draw_scroll_window(
        g_game->GetBackBuffer(), asx + 145, asy + 150, 80, 75);
}

void
ModuleAuxiliaryDisplay::PrintSystemStatus(
    int x,
    int y,
    const string &title,
    int value) {
    ALLEGRO_COLOR color;
    string status;

    al_draw_text(g_game->font12, SKYBLUE, x, y, 0, title.c_str());

    if (value == -1) {
        color = GRAY1;
        status = "NONE";
    } else if (value == 0) {
        color = RED;
        status = "DESTROYED";
    } else if (value < 25) {
        color = RED;
        status = "CRITICAL";
    } else if (value < 50) {
        color = YELLOW2;
        status = "DAMAGED";
    } else {
        color = GREEN2;
        status = "FUNCTIONAL";
    }
    al_draw_text(
        g_game->font12,
        color,
        x + (asw - asx),
        y,
        ALLEGRO_ALIGN_RIGHT,
        status.c_str());
}

void
ModuleAuxiliaryDisplay::updateEng() {
    int x = asx, y = asy;

    Ship ship = g_game->gameState->getShip();

    // officer name
    al_draw_textf(
        g_game->font20,
        SKYBLUE,
        x,
        y,
        0,
        "Eng Off. %s",
        g_game->gameState->officerEng->getLastName().c_str());
    y += 40;

    PrintSystemStatus(x, y, "HULL", ship.getHullIntegrity());
    y += 20;

    PrintSystemStatus(
        x,
        y,
        "ENGINE " + ship.getEngineClassString(),
        ship.getEngineIntegrity());
    y += 20;

    PrintSystemStatus(
        x,
        y,
        "ARMOR" + ship.getArmorClassString(),
        ship.getArmorIntegrity() / ship.getMaxArmorIntegrity() * 100.0);
    y += 20;

    if (ship.getShieldClass() != 0) {
        PrintSystemStatus(
            x,
            y,
            "SHIELD " + ship.getShieldClassString(),
            ship.getShieldIntegrity());
    } else {
        PrintSystemStatus(x, y, "SHIELD", -1);
    }
    y += 20;

    if (ship.getLaserClass() != 0) {
        PrintSystemStatus(
            x,
            y,
            "LASER " + ship.getLaserClassString(),
            ship.getLaserIntegrity());
    } else {
        PrintSystemStatus(x, y, "LASER", -1);
    }
    y += 20;

    if (ship.getMissileLauncherClass() != 0) {
        PrintSystemStatus(
            x,
            y,
            "MISSILE " + ship.getMissileLauncherClassString(),
            ship.getMissileLauncherIntegrity());
    } else {
        PrintSystemStatus(x, y, "MISSILE", -1);
    }
}

void
ModuleAuxiliaryDisplay::updateCom() {
    std::ostringstream os;
    int x = asx, y = asy + 130;

    // officer name
    os << "Comm Off. " << g_game->gameState->officerCom->getLastName();
    g_game->Print20(canvas, x, y, os.str(), SKYBLUE);
}

void
ModuleAuxiliaryDisplay::updateTac() {
    std::ostringstream os;
    Ship ship = g_game->gameState->getShip();
    int x = asx, y = asy + 130;
    int dx;

    // officer name
    al_draw_textf(
        g_game->font20,
        SKYBLUE,
        x,
        y,
        0,
        "Tac Off. %s",
        g_game->gameState->officerTac->getLastName().c_str());

    dx = asx + asw / 2;

    y += 20;
    al_draw_text(g_game->font18, HEADING_COLOR, x, y, 0, "ARMOR: ");
    al_draw_text(
        g_game->font18,
        SKYBLUE,
        x + dx,
        y,
        0,
        ship.getArmorClassString().c_str());

    y += 18;
    al_draw_text(g_game->font18, HEADING_COLOR, x, y, 0, "SHIELD: ");
    al_draw_text(
        g_game->font18,
        SKYBLUE,
        x + dx,
        y,
        0,
        ship.getShieldClassString().c_str());

    y += 18;
    g_game->Print18(canvas, x, y, "LASER: ", HEADING_COLOR);
    g_game->Print18(canvas, x + dx, y, ship.getLaserClassString(), SKYBLUE);

    y += 18;
    g_game->Print18(canvas, x, y, "MISSILE: ", HEADING_COLOR);
    g_game->Print18(
        canvas, x + dx, y, ship.getMissileLauncherClassString(), SKYBLUE);
}

void
ModuleAuxiliaryDisplay::updateMed() {
    int y = 525, x = 15;

    // officer name
    string doctor = "Med Off. " + g_game->gameState->officerDoc->getLastName();
    g_game->Print18(canvas, x, y, doctor, SKYBLUE);

    y += 40;
    medical_display(g_game->gameState->officerCap, x, y, "CAP. ");

    y += 20;
    medical_display(g_game->gameState->officerSci, x, y, "SCI. ");

    y += 20;
    medical_display(g_game->gameState->officerNav, x, y, "NAV. ");

    y += 20;
    medical_display(g_game->gameState->officerTac, x, y, "TAC. ");

    y += 20;
    medical_display(g_game->gameState->officerCom, x, y, "COM. ");

    y += 20;
    medical_display(g_game->gameState->officerEng, x, y, "ENG. ");

    y += 20;
    medical_display(g_game->gameState->officerDoc, x, y, "MED. ");
}

void
ModuleAuxiliaryDisplay::medical_display(
    Officer *officer_data,
    int x,
    int y,
    const string &additional_data) {
    ALLEGRO_COLOR text_color = BLACK;
    std::string status = "";
    int x2 = 150;
    std::ostringstream os;
    if (officer_data->attributes.getVitality() <= 0) {
        text_color = BLACK;
        status = "DEAD";
    } else if (officer_data->attributes.getVitality() < 25) {
        text_color = RED2;
        status = "CRITICAL";
    } else if (officer_data->attributes.getVitality() < 50) {
        text_color = YELLOW2;
        status = "INJURED";
    } else {
        text_color = GREEN2;
        status = "HEALTHY";
    }

    // name and rank
    os << additional_data << officer_data->getLastName();
    g_game->Print12(canvas, x, y, os.str(), SKYBLUE);

    os.str(""); // clear

    // medical status
    g_game->Print12(canvas, x2, y, status, text_color);
}

void
ModuleAuxiliaryDisplay::updateCrew() {
    switch (g_game->gameState->getCurrentSelectedOfficer()) {
    case OFFICER_CAPTAIN:
        updateCap();
        break;
    case OFFICER_SCIENCE:
        updateSci();
        break;
    case OFFICER_NAVIGATION:
        updateNav();
        break;
    case OFFICER_ENGINEER:
        updateEng();
        break;
    case OFFICER_COMMUNICATION:
        updateCom();
        break;
    case OFFICER_MEDICAL:
        updateMed();
        break;
    case OFFICER_TACTICAL:
        updateTac();
        break;
    default:
        // this should never happen, so we want a fatal if it happens to
        // find the bug
        ALLEGRO_DEBUG("  [AuxiliaryDisplay] ERROR: No officer selected in "
                      "control panel.");
    }
}

void
ModuleAuxiliaryDisplay::DrawBackground() {
    // draw the aux gui
    al_draw_bitmap(img_aux, gax, gay, 0);
}

void
ModuleAuxiliaryDisplay::DrawContent() {
    // clear the "lcd" portion of the screen with darkgreen
    static ALLEGRO_COLOR lcdcolor = al_map_rgb(20, 40, 0);
    al_draw_filled_rectangle(asx, asy, asx + asw, asy + ash, lcdcolor);

    updateAll();
    updateCrew();
}

bool
ModuleAuxiliaryDisplay::on_draw(ALLEGRO_BITMAP *target) {
    al_set_target_bitmap(target);

    // draw the aux gui
    DrawBackground();

    // based on current module, fill aux display with content
    std::string module = g_game->gameState->getCurrentModule();
    if (module == MODULE_HYPERSPACE) {
        DrawContent();
    } else if (module == MODULE_INTERPLANETARY) {
    } else if (module == MODULE_ORBIT) {
    } else if (module == MODULE_SURFACE) {
    } else if (module == MODULE_ENCOUNTER) {
    }
    return true;
}
