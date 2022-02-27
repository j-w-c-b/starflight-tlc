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
#include "MessageBoxWindow.h"
#include "ModeMgr.h"
#include "ModuleStarport.h"
#include "PauseMenu.h"
#include "QuestMgr.h"
#include "Sprite.h"
#include "starport_resources.h"

using namespace std;
using namespace starport;

namespace {
ALLEGRO_DEBUG_CHANNEL("ModuleStarport")

struct doorArea {
    static constexpr int DOOR_WIDTH = 228;
    explicit doorArea(int left_, string_view module_name_)
        : left(left_), right(left_ + DOOR_WIDTH), middle(left + DOOR_WIDTH / 2),
          module_name(string(module_name_)) {}
    bool within_door(int point) const {
        return (point >= left && point <= right);
    }
    int left;
    int right;
    int middle;
    string module_name;
};

static const std::vector<doorArea> doors = {
    doorArea(558, MODULE_RESEARCHLAB),
    doorArea(961, MODULE_CREWBUY),
    doorArea(1299, MODULE_CAPTAINSLOUNGE),
    doorArea(1958, MODULE_SHIPCONFIG),
    doorArea(2326, MODULE_TRADEDEPOT),
    doorArea(2807, MODULE_BANK),
    doorArea(3194, MODULE_CANTINA),
    doorArea(3615, MODULE_MILITARYOPS),
};
}; // namespace

//***********************************************
// Constructor
//***********************************************
ModuleStarport::ModuleStarport() : Module() {
    // enable the Pause Menu
    flag_showWelcome = true;
}

//***********************************************
// Private Funtions
//***********************************************
bool
ModuleStarport::testDoors() {
    int px = playerx + g_game->gameState->player.posStarport.x
             + (avatar->getFrameWidth() / 2);
    ProfessionType profession = g_game->gameState->getProfession();
    string message;

    for (int a = 0; a < NUMBER_OF_DOORS; a++) {
        if (doors[a].within_door(px)) {
            if (a == 0 || a == 6 || a == 7) {
                // Science Profession - wrong doors
                if (profession == PROFESSION_SCIENTIFIC && a == 6) {
                    message = "We don't serve your kind here! Go to the "
                              "science lab where you belong.";
                    break;
                }
                if (profession == PROFESSION_SCIENTIFIC && a == 7) {
                    message = "Invalid Military Rank! Go to the science lab "
                              "where you belong.";
                    break;
                }

                // Freelance - wrong doors
                if (profession == PROFESSION_FREELANCE && a == 0) {
                    message = "ACCESS DENIED! Wouldn't you feel more at home "
                              "in the Cantina?";
                    break;
                }
                if (profession == PROFESSION_FREELANCE && a == 7) {
                    message =
                        "Invalid Military Rank! Go check out the Cantina.";

                    break;
                }

                // Military - wrong doors
                if (profession == PROFESSION_MILITARY && a == 0) {
                    message = "The military has no jurisdiction here! Go "
                              "report to your commander or whoever.";
                    break;
                }
                if (profession == PROFESSION_MILITARY && a == 6) {
                    message = "Hey bub, aren't you late for a briefing in the "
                              "War Room?";
                    break;
                }
            }

            destinationDoor = a;

            return true;
        }
    }

    if (message != "") {
        set_modal_child(make_shared<MessageBoxWindow>(
            "", message, 400, 200, 312, 284, WHITE, false));
        return false;
    }
    return false;
}

void
ModuleStarport::move_player(int distanceInPixels) {
    int width = al_get_bitmap_width(m_background_image.get());
    int player_width = avatar->getFrameWidth();

    if (g_game->gameState->player.posStarport.x
        <= 0) // If we're scrolled atw left...
    {
        if (playerx + distanceInPixels < SCREEN_EDGE_PADDING) {
            playerx = SCREEN_EDGE_PADDING;
        } else if (
            playerx + distanceInPixels < (SCREEN_WIDTH - player_width) / 2) {
            playerx += distanceInPixels;
        } else {
            playerx = SCREEN_WIDTH / 2 - player_width / 2;
            g_game->gameState->player.posStarport.x += distanceInPixels;
        }
    } else if (
        g_game->gameState->player.posStarport.x + SCREEN_WIDTH >= width) {
        // If we're scrolled atw right...
        if (playerx + distanceInPixels + player_width + SCREEN_EDGE_PADDING
            > SCREEN_WIDTH) {
            playerx = SCREEN_WIDTH - player_width - SCREEN_EDGE_PADDING;
        } else if (
            playerx + distanceInPixels > (SCREEN_WIDTH - player_width) / 2) {
            playerx += distanceInPixels;
        } else {
            playerx = (SCREEN_WIDTH - player_width) / 2;
            g_game->gameState->player.posStarport.x += distanceInPixels;
        }
    } else { // If we're inbetween...
        g_game->gameState->player.posStarport.x += distanceInPixels;
        if (g_game->gameState->player.posStarport.x < 0) {
            g_game->gameState->player.posStarport.x = 0;
        } else if (
            g_game->gameState->player.posStarport.x + SCREEN_WIDTH > width) {
            g_game->gameState->player.posStarport.x = width - SCREEN_WIDTH;
        }
    }
}

//***********************************************
// Public Functions
//***********************************************
bool
ModuleStarport::on_close() {
    avatar.reset();
    door.reset();
    return true;
}

bool
ModuleStarport::on_init() {
    g_game->SetTimePaused(true); // game-time frozen in this module.

    ALLEGRO_DEBUG("  Starport Initialize\n");

    m_background_image = images[I_STARPORT];
    // enable the Pause Menu
    g_game->enable_pause_menu(true);

    // load door
    door = make_unique<Sprite>();
    door->setImage(images[I_STARPORT_DOOR]);
    door->setAnimColumns(2);
    door->setFrameWidth(180);
    door->setFrameHeight(237);
    door->setTotalFrames(2);

    // load avatar
    avatar = make_unique<Sprite>();
    avatar->setImage(images[I_STARPORT_AVATAR]);
    avatar->setAnimColumns(8);
    avatar->setTotalFrames(16);
    avatar->setFrameHeight(237);
    avatar->setFrameWidth(237);

    ALLEGRO_DEBUG("    Positioning avatar\n");
    playerx = SCREEN_WIDTH / 2 - 237 / 2;
    playery = g_game->getGlobalNumber("PLAYER_STARPORT_START_Y");

    // set avatar starting position in starport (first time only)
    if (g_game->gameState->player.posStarport.x == -1) {
        g_game->gameState->player.posStarport.x =
            g_game->getGlobalNumber("PLAYER_STARPORT_START_X");
    }

    avatar->setPos(playerx, playery);
    avatar->setFrameDelay(3);

    movement = STOP_RIGHT;
    enteringDoor = false;
    openingDoor = false;
    closingDoor = false;
    insideDoor = false;
    m_bNotified = false;
    doorDistance = 0;
    destinationDoor = 0;

    return true;
}

bool
ModuleStarport::on_event(ALLEGRO_EVENT *event) {
    std::string escape;

    // check for general events
    switch (event->type) {
    case EVENT_CLOSE:
        set_modal_child(nullptr);
        return true;
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
        movement = MOVE_RIGHT;
        break;

    case ALLEGRO_KEY_A:
    case ALLEGRO_KEY_LEFT:
        // turn left
        movement = MOVE_LEFT;
        break;

    case ALLEGRO_KEY_W:
    case ALLEGRO_KEY_UP:
        // thrust/forward
        if (testDoors()) {
            enteringDoor = true;
            openingDoor = true;
            g_game->audioSystem->Play(samples[S_DOOROPEN]);
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
        movement = STOP_RIGHT;
        break;

        // turn left
    case ALLEGRO_KEY_A:
    case ALLEGRO_KEY_LEFT:
        movement = STOP_LEFT;
        break;

    case ALLEGRO_KEY_ESCAPE:
        break;
    }
    return true;
}

void
ModuleStarport::drawDoors(ALLEGRO_BITMAP *target) {
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
                    doors[a].left - g_game->gameState->player.posStarport.x
                        - 42,
                    348);
                door->drawframe(target);

                // draw right door frame
                door->setCurrFrame(1);
                door->setPos(
                    doors[a].right - 180
                        - g_game->gameState->player.posStarport.x,
                    348);
                door->drawframe(target);
            }
        } else {
            // draw this door if it's in view
            if (doors[a].right > g_game->gameState->player.posStarport.x
                && doors[a].left < g_game->gameState->player.posStarport.x
                                       + SCREEN_WIDTH) {
                // draw left door frame
                door->setCurrFrame(0);
                door->setPos(
                    doors[a].left - g_game->gameState->player.posStarport.x
                        - 42,
                    348);
                door->drawframe(target);

                // draw right door frame
                door->setCurrFrame(1);
                door->setPos(
                    doors[a].right - 180
                        - g_game->gameState->player.posStarport.x,
                    348);
                door->drawframe(target);
            }
        }
    }
}

void
ModuleStarport::enterDoor() {
    int px = playerx + g_game->gameState->player.posStarport.x
             + (avatar->getFrameWidth() / 2);
    if (px < doors[destinationDoor].middle
        && doors[destinationDoor].middle - px > HORIZONTAL_MOVE_DISTANCE) {
        move_player(HORIZONTAL_MOVE_DISTANCE);
        if (px > doors[destinationDoor].middle)
            playerx = doors[destinationDoor].middle
                      - g_game->gameState->player.posStarport.x
                      - (avatar->getFrameWidth() / 2);
    } else if (
        px > doors[destinationDoor].middle
        && px - doors[destinationDoor].middle > HORIZONTAL_MOVE_DISTANCE) {
        move_player(-HORIZONTAL_MOVE_DISTANCE);
        if (px < doors[destinationDoor].middle)
            playerx = doors[destinationDoor].middle
                      - g_game->gameState->player.posStarport.x
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
                g_game->LoadModule(doors[destinationDoor].module_name);
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
        case MOVE_RIGHT: // Move Right
            move_player(HORIZONTAL_MOVE_DISTANCE);
            avatar->animate(0, 7);
            break;
        case STOP_RIGHT: // stop right
            avatar->setCurrFrame(0);
            break;
        case MOVE_LEFT: // move left
            move_player(-HORIZONTAL_MOVE_DISTANCE);
            avatar->animate(8, 15);
            break;
        case STOP_LEFT: // stop left
            avatar->setCurrFrame(8);
            break;
        }
    }

    // clear background
    al_set_target_bitmap(target);
    al_clear_to_color(BLACK);

    // update and draw doors
    drawDoors(target);

    // draw starport
    al_draw_bitmap_region(
        m_background_image.get(),
        g_game->gameState->player.posStarport.x,
        0,
        SCREEN_WIDTH,
        348 + 183 + 348,
        0,
        0,
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
                - g_game->gameState->player.posStarport.x - doorDistance - 42,
            348);
        door->drawframe(target);

        // draw right door panel
        door->setCurrFrame(1);
        door->setPos(
            doors[destinationDoor].right - 180
                - g_game->gameState->player.posStarport.x + doorDistance,
            348);
        door->drawframe(target);

        // draw starport center section
        al_draw_bitmap_region(
            m_background_image.get(),
            g_game->gameState->player.posStarport.x,
            348,
            SCREEN_WIDTH,
            237,
            0,
            348,
            0);
    }

    if (g_game->gameState->player.hasOverdueLoan()
        && g_game->gameState->player.hasHyperspacePermit()) {
        if (!m_bNotified) {
            set_modal_child(make_shared<MessageBoxWindow>(
                "",
                " The bank has disabled your hyperspace engine due to overdue "
                "loans! "));
            m_bNotified = true;
            g_game->gameState->player.set_HyperspacePermit(false);
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
        set_modal_child(make_shared<MessageBoxWindow>(
            "", message, 400, 350, 600, 400, WHITE, false));
    }
    return true;
}
