/*
        STARFLIGHT - THE LOST COLONY
        ModuleStarport.h - Handles Starport activity
        Author: Matt Klausmeier
        Date: October 6, 2007
*/

#ifndef _STARPORT_H
#define _STARPORT_H 1

#include <allegro5/allegro.h>

#include "Module.h"
#include "Sprite.h"

class ModuleStarport : public Module {
#define NUMBER_OF_DOORS 8
#define SCREEN_EDGE_PADDING 24
#define AVATAR_INSIDE_DOOR_Y 350
#define DOOR_0_X 558
#define DOOR_1_X 961
#define DOOR_2_X 1299
#define DOOR_3_X 1958
#define DOOR_4_X 2326
#define DOOR_5_X 2807
#define DOOR_6_X 3194
#define DOOR_7_X 3615

#define DOOR_WIDTH 228
#define HORIZONTAL_MOVE_DISTANCE 10
#define LAST_FRAME_OF_MOVE_RIGHT_ANIMATION 7
#define STAND_RIGHT_FRAME 0
#define STAND_LEFT_FRAME 0
#define MINIMUN_DISTANCE_TO_DOOR 10
#define ENTER_DOOR_SPEED 10
#define DOOR_SPEED 5

    struct doorArea {
        int left, right, middle;
    };

  private:
    Sprite *door;
    Sprite *avatar;
    int playerx;
    int playery;
    int destinationDoor;
    int doorDistance;
    int movement;
    bool enteringDoor;
    bool openingDoor;
    bool closingDoor;
    bool insideDoor;
    bool m_bNotified;
    doorArea doors[NUMBER_OF_DOORS];
    bool flag_showWelcome;

    bool testDoors();
    void movePlayerLeft(int distanceInPixels);
    void movePlayerRight(int distanceInPixels);
    void enterDoor();
    void drawDoors(ALLEGRO_BITMAP *target);

  public:
    ModuleStarport();
    virtual ~ModuleStarport();

    virtual bool on_init() override;
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;
    virtual bool on_close() override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;
    virtual bool on_key_down(ALLEGRO_KEYBOARD_EVENT *event) override;
    virtual bool on_key_up(ALLEGRO_KEYBOARD_EVENT *event) override;
};

#endif
