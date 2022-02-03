/*
        STARFLIGHT - THE LOST COLONY
        ModuleStarport.cpp - Handles Starport activity
        Author: Matt Klausmeier
        Date: October 6, 2007

        This module is the common starport area, where the user gains access to
   the areas where he can create his character, customize his ship, hire a crew,
        go to the bank, trade goods and receive orders.
*/

#include <sstream>

#include "AudioSystem.h"
#include "Events.h"
#include "Game.h"
#include "GameState.h"
#include "ModeMgr.h"
#include "ModuleStarport.h"
#include "PauseMenu.h"
#include "QuestMgr.h"
#include "ScrollBox.h"
#include "Sprite.h"
#include "starport_resources.h"

using namespace std;
using namespace starport_resources;

ALLEGRO_DEBUG_CHANNEL("ModuleStarport")

//***********************************************
// Constructor
//***********************************************
ModuleStarport::ModuleStarport(void) : resources(STARPORT_IMAGES) {
    // load the starport background
    if (!resources.load()) {
        g_game->message("Starport: Error loading datafile");
    }

    // enable the Pause Menu
    g_game->enable_pause_menu(true);
    flag_showWelcome = true;
}
//***********************************************
// Destructor
//***********************************************
ModuleStarport::~ModuleStarport(void) { resources.unload(); }

//***********************************************
// Private Funtions
//***********************************************
bool
ModuleStarport::testDoors() {
    int px = playerx + g_game->gameState->player->posStarport.x
             + (avatar->getFrameWidth() / 2);
    for (int a = 0; a < NUMBER_OF_DOORS; a++) {
        if (px >= doors[a].left && px <= doors[a].right) {
            if (a == 0 || a == 6 || a == 7) {
                // Science Profession - wrong doors
                if (g_game->gameState->getProfession() == PROFESSION_SCIENTIFIC
                    && a == 6) {
                    g_game->ShowMessageBoxWindow(
                        "",
                        "We don't serve your kind here! Go to the science lab "
                        "where you belong.",
                        400,
                        200,
                        WHITE,
                        312,
                        284,
                        false);
                    return false;
                }
                if (g_game->gameState->getProfession() == PROFESSION_SCIENTIFIC
                    && a == 7) {
                    g_game->ShowMessageBoxWindow(
                        "",
                        "Invalid Military Rank! Go to the science lab where "
                        "you belong.",
                        400,
                        200,
                        WHITE,
                        312,
                        284,
                        false);
                    return false;
                }

                // Freelance - wrong doors
                if (g_game->gameState->getProfession() == PROFESSION_FREELANCE
                    && a == 0) {
                    g_game->ShowMessageBoxWindow(
                        "",
                        "ACCESS DENIED! Wouldn't you feel more at home in the "
                        "Cantina?",
                        400,
                        200,
                        WHITE,
                        312,
                        284,
                        false);
                    return false;
                }
                if (g_game->gameState->getProfession() == PROFESSION_FREELANCE
                    && a == 7) {
                    g_game->ShowMessageBoxWindow(
                        "",
                        "Invalid Military Rank! Go check out the Cantina.",
                        400,
                        200,
                        WHITE,
                        312,
                        284,
                        false);
                    return false;
                }

                // Military - wrong doors
                if (g_game->gameState->getProfession() == PROFESSION_MILITARY
                    && a == 0) {
                    g_game->ShowMessageBoxWindow(
                        "",
                        "The military has no jurisdiction here! Go report to "
                        "your commander or whoever.",
                        400,
                        200,
                        WHITE,
                        312,
                        284,
                        false);
                    return false;
                }
                if (g_game->gameState->getProfession() == PROFESSION_MILITARY
                    && a == 6) {
                    g_game->ShowMessageBoxWindow(
                        "",
                        "Hey bub, aren't you late for "
                        "a briefing in the War Room?",
                        400,
                        200,
                        WHITE,
                        312,
                        284,
                        false);

                    return false;
                }
            }

            destinationDoor = a;

            return true;
        }
    }
    return false;
}

void
ModuleStarport::movePlayerLeft(int distanceInPixels) {
    if (g_game->gameState->player->posStarport.x
        <= 0) // If we're scrolled atw left...
    {
        if (playerx - distanceInPixels < SCREEN_EDGE_PADDING)
            playerx = SCREEN_EDGE_PADDING;
        else
            playerx -= distanceInPixels;
    } else if (
        g_game->gameState->player->posStarport.x + SCREEN_WIDTH
        >= al_get_bitmap_width(starport)) // If we're scrolled atw right...
    {
        if (playerx - distanceInPixels > SCREEN_WIDTH / 2 - 237 / 2)
            playerx -= distanceInPixels;
        else {
            playerx = SCREEN_WIDTH / 2 - 237 / 2;
            g_game->gameState->player->posStarport.x -= distanceInPixels;
        }
    } else // If we're inbetween...
    {
        g_game->gameState->player->posStarport.x -= distanceInPixels;
        if (g_game->gameState->player->posStarport.x < 0)
            g_game->gameState->player->posStarport.x = 0;
    }
}
void
ModuleStarport::movePlayerRight(int distanceInPixels) {
    if (g_game->gameState->player->posStarport.x
        <= 0) // If we're scrolled atw left...
    {
        if (playerx + distanceInPixels < SCREEN_WIDTH / 2 - 237 / 2)
            playerx += distanceInPixels;
        else {
            playerx = SCREEN_WIDTH / 2 - 237 / 2;
            g_game->gameState->player->posStarport.x += distanceInPixels;
        }
    } else if (
        g_game->gameState->player->posStarport.x + SCREEN_WIDTH
        >= al_get_bitmap_width(starport)) // If we're scrolled atw right...
    {
        if (playerx + distanceInPixels + 237 + SCREEN_EDGE_PADDING
            > SCREEN_WIDTH)
            playerx = SCREEN_WIDTH - 237 - SCREEN_EDGE_PADDING;
        else
            playerx += distanceInPixels;
    } else // If we're inbetween...
    {
        g_game->gameState->player->posStarport.x += distanceInPixels;
        if (g_game->gameState->player->posStarport.x + SCREEN_WIDTH
            > al_get_bitmap_width(starport))
            g_game->gameState->player->posStarport.x =
                al_get_bitmap_width(starport) - SCREEN_WIDTH;
    }
}

//***********************************************
// Public Functions
//***********************************************
bool
ModuleStarport::on_close() {
    delete avatar;
    delete door;
    return true;
}

bool
ModuleStarport::on_init() {
    g_game->SetTimePaused(true); // game-time frozen in this module.

    ALLEGRO_DEBUG("  Starport Initialize\n");

    // enable the Pause Menu
    g_game->enable_pause_menu(true);

    // load the sound effects
    // NOTE: a missing wav file is a soft error for the time being
    if (!g_game->audioSystem->SampleExists("dooropen.wav")) {
        if (!g_game->audioSystem->Load(
                "data/starport/dooropen.wav", "dooropen")) {
            ALLEGRO_DEBUG(
                "Starport: Error loading data/starport/dooropen.wav\n");
        }
    }

    // load the starport background
    starport = resources[I_STARPORT];

    // load door
    door = new Sprite();
    door->setImage(resources[I_STARPORT_DOOR]);
    door->setAnimColumns(2);
    door->setFrameWidth(180);
    door->setFrameHeight(237);
    door->setTotalFrames(2);

    // load avatar
    avatar = new Sprite();
    avatar->setImage(resources[I_STARPORT_AVATAR]);
    avatar->setAnimColumns(8);
    avatar->setTotalFrames(16);
    avatar->setFrameHeight(237);
    avatar->setFrameWidth(237);

    ALLEGRO_DEBUG("    Positioning avatar\n");
    playerx = SCREEN_WIDTH / 2 - 237 / 2;
    playery = g_game->getGlobalNumber("PLAYER_STARPORT_START_Y");

    // set avatar starting position in starport (first time only)
    if (g_game->gameState->player->posStarport.x == -1) {
        g_game->gameState->player->posStarport.x =
            g_game->getGlobalNumber("PLAYER_STARPORT_START_X");
    }

    avatar->setPos(playerx, playery);
    avatar->setFrameDelay(3);

    movement = 0;
    enteringDoor = false;
    openingDoor = false;
    closingDoor = false;
    insideDoor = false;
    m_bNotified = false;
    doorDistance = 0;
    destinationDoor = 0;

    // create doors
    doors[0].left = DOOR_0_X;
    doors[1].left = DOOR_1_X;
    doors[2].left = DOOR_2_X;
    doors[3].left = DOOR_3_X;
    doors[4].left = DOOR_4_X;
    doors[5].left = DOOR_5_X;
    doors[6].left = DOOR_6_X;
    doors[7].left = DOOR_7_X;

    for (int a = 0; a < NUMBER_OF_DOORS; ++a) {
        doors[a].right = doors[a].left + DOOR_WIDTH;
        doors[a].middle =
            ((doors[a].right - doors[a].left) / 2) + doors[a].left;
    }

    return true;
}

bool
ModuleStarport::on_event(ALLEGRO_EVENT *event) {
    std::string escape;

    // check for general events
    switch (event->type) {
    case EVENT_SAVE_GAME:
        g_game->gameState->AutoSave();
        return true;
    case EVENT_LOAD_GAME:
        g_game->gameState->AutoLoad();
        return true;
    case EVENT_QUIT_GAME:
        escape = g_game->getGlobalString("ESCAPEMODULE");
        g_game->LoadModule(escape);
        return false;
    }
    return true;
}

bool
ModuleStarport::on_key_down(ALLEGRO_KEYBOARD_EVENT *event) {
    switch (event->keycode) {
    case ALLEGRO_KEY_D:
    case ALLEGRO_KEY_RIGHT:
        // turn right
        movement = 1;
        break;

    case ALLEGRO_KEY_A:
    case ALLEGRO_KEY_LEFT:
        // turn left
        movement = -1;
        break;

    case ALLEGRO_KEY_W:
    case ALLEGRO_KEY_UP:
        // thrust/forward
        if (testDoors()) {
            enteringDoor = true;
            openingDoor = true;
            if (g_game->audioSystem->SampleExists("dooropen"))
                g_game->audioSystem->Play("dooropen");
        }
        break;
    }
    return true;
}

bool
ModuleStarport::on_key_up(ALLEGRO_KEYBOARD_EVENT *event) {
    switch (event->keycode) {
        // turn right
    case ALLEGRO_KEY_D:
    case ALLEGRO_KEY_RIGHT:
        movement = 2; // to indicate stopped in this direction
        break;

        // turn left
    case ALLEGRO_KEY_A:
    case ALLEGRO_KEY_LEFT:
        movement = -2; // to indicate stopped in this direction
        break;

    case ALLEGRO_KEY_ESCAPE:
        break;
    }
    return true;
}

void
ModuleStarport::drawDoors() {
    for (int a = 0; a < NUMBER_OF_DOORS; a++) {
        if (enteringDoor) {
            if (openingDoor)
                doorDistance += DOOR_SPEED;
            if (doorDistance > 100) {
                openingDoor = false;
                closingDoor = true;
            }
            if (destinationDoor != a) {
                // draw left door frame
                door->setCurrFrame(0);
                door->setPos(
                    doors[a].left - g_game->gameState->player->posStarport.x
                        - 42,
                    348);
                door->drawframe(g_game->GetBackBuffer());

                // draw right door frame
                door->setCurrFrame(1);
                door->setPos(
                    doors[a].right - 180
                        - g_game->gameState->player->posStarport.x,
                    348);
                door->drawframe(g_game->GetBackBuffer());
            }
        } else {
            // draw this door if it's in view
            if (doors[a].right > g_game->gameState->player->posStarport.x
                && doors[a].left < g_game->gameState->player->posStarport.x
                                       + SCREEN_WIDTH) {
                // draw left door frame
                door->setCurrFrame(0);
                door->setPos(
                    doors[a].left - g_game->gameState->player->posStarport.x
                        - 42,
                    348);
                door->drawframe(g_game->GetBackBuffer());

                // draw right door frame
                door->setCurrFrame(1);
                door->setPos(
                    doors[a].right - 180
                        - g_game->gameState->player->posStarport.x,
                    348);
                door->drawframe(g_game->GetBackBuffer());
            }
        }
    }
}

void
ModuleStarport::enterDoor() {
    int px = playerx + g_game->gameState->player->posStarport.x
             + (avatar->getFrameWidth() / 2);
    if (px < doors[destinationDoor].middle
        && doors[destinationDoor].middle - px > HORIZONTAL_MOVE_DISTANCE) {
        movePlayerRight(HORIZONTAL_MOVE_DISTANCE);
        if (px > doors[destinationDoor].middle)
            playerx = doors[destinationDoor].middle
                      - g_game->gameState->player->posStarport.x
                      - (avatar->getFrameWidth() / 2);
    } else if (
        px > doors[destinationDoor].middle
        && px - doors[destinationDoor].middle > HORIZONTAL_MOVE_DISTANCE) {
        movePlayerLeft(HORIZONTAL_MOVE_DISTANCE);
        if (px < doors[destinationDoor].middle)
            playerx = doors[destinationDoor].middle
                      - g_game->gameState->player->posStarport.x
                      - (avatar->getFrameWidth() / 2);
    } else {
        // OPEN DOOR CODE HERE
        if (playery > AVATAR_INSIDE_DOOR_Y)
            playery -= ENTER_DOOR_SPEED;
        else if (closingDoor) {
            insideDoor = true;
            if (doorDistance <= 0) {
                closingDoor = false;
                enteringDoor = false;
                switch (destinationDoor) {
                case 0:
                    g_game->LoadModule(MODULE_RESEARCHLAB);
                    return;
                    break;
                case 1:
                    g_game->LoadModule(MODULE_CREWBUY);
                    return;
                    break;
                case 2:
                    g_game->LoadModule(MODULE_CAPTAINSLOUNGE);
                    return;
                    break;
                case 3:
                    g_game->LoadModule(MODULE_SHIPCONFIG);
                    return;
                    break;
                case 4:
                    g_game->LoadModule(MODULE_TRADEDEPOT);
                    return;
                    break;
                case 5:
                    g_game->LoadModule(MODULE_BANK);
                    return;
                    break;
                case 6:
                    g_game->LoadModule(MODULE_CANTINA);
                    return;
                    break;
                case 7:
                    g_game->LoadModule(MODULE_MILITARYOPS);
                    return;
                    break;
                }
                return;
            } else
                doorDistance -= DOOR_SPEED;
        }
    }
}

bool
ModuleStarport::on_draw(ALLEGRO_BITMAP *target) {
    // move avatar
    if (!enteringDoor) {
        switch (movement) {
        case 1: // Move Right
            movePlayerRight(HORIZONTAL_MOVE_DISTANCE);
            avatar->animate(0, 7);
            break;
        case 2: // stop right
            avatar->setCurrFrame(0);
            break;
        case -1: // move left
            movePlayerLeft(HORIZONTAL_MOVE_DISTANCE);
            avatar->animate(8, 15);
            break;
        case -2: // stop left
            avatar->setCurrFrame(8);
            break;
        }
    }

    // clear background
    al_set_target_bitmap(target);
    al_clear_to_color(BLACK);

    // update and draw doors
    drawDoors();

    // draw starport top section
    al_draw_bitmap_region(
        starport,
        g_game->gameState->player->posStarport.x,
        0,
        SCREEN_WIDTH,
        348,
        0,
        0,
        0);

    // draw starport floor section
    al_draw_bitmap_region(
        starport,
        g_game->gameState->player->posStarport.x,
        585,
        SCREEN_WIDTH,
        183,
        0,
        585,
        0);

    // draw starport middle section
    al_draw_bitmap_region(
        starport,
        g_game->gameState->player->posStarport.x,
        348,
        SCREEN_WIDTH,
        237,
        0,
        348,
        0);

    // draw avatar
    avatar->setPos(playerx, playery);
    avatar->drawframe(target);

    // draw door over top of avatar if closing
    if (enteringDoor || closingDoor) {
        // draw left door panel
        door->setCurrFrame(0);
        door->setPos(
            doors[destinationDoor].left
                - g_game->gameState->player->posStarport.x - doorDistance - 42,
            348);
        door->drawframe(target);

        // draw right door panel
        door->setCurrFrame(1);
        door->setPos(
            doors[destinationDoor].right - 180
                - g_game->gameState->player->posStarport.x + doorDistance,
            348);
        door->drawframe(target);

        // draw starport center section
        al_draw_bitmap_region(
            starport,
            g_game->gameState->player->posStarport.x,
            348,
            SCREEN_WIDTH,
            237,
            0,
            348,
            0);
    }

    if (g_game->gameState->player->hasOverdueLoan()
        && g_game->gameState->player->hasHyperspacePermit()) {
        if (!m_bNotified) {
            g_game->ShowMessageBoxWindow(
                "",
                " The bank has disabled your hyperspace engine due to overdue "
                "loans! ",
                400,
                200);
            m_bNotified = true;
            g_game->gameState->player->set_HyperspacePermit(false);
        }
    }

    // launch new module when door entered (must be last!)
    if (enteringDoor) {
        enterDoor();
        return true;
    }

    // show welcome message for first-time visitor
    if (g_game->gameState->getActiveQuest() == 1 && flag_showWelcome) {
        flag_showWelcome = false;
        string message = "Welcome to the Starport, captain! If you haven't "
                         "already, please head over to the ";
        switch (g_game->gameState->getProfession()) {
        case PROFESSION_FREELANCE:
            message += "Cantina";
            break;
        case PROFESSION_MILITARY:
            message += "War Room";
            break;
        case PROFESSION_SCIENTIFIC:
            message += "Research Lab";
            break;
        default:
            ALLEGRO_ASSERT(0);
        }
        message +=
            " for your first assignment. Use the cursor keys to move "
            "left/right, use the up key to enter a room. You can change the "
            "default keys from the Settings option on the Title Screen.";
        g_game->ShowMessageBoxWindow(
            "", message, 400, 350, WHITE, 600, 400, false);
    }
    return true;
}
