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
    static constexpr int NUMBER_OF_DOORS = 8;
    static constexpr int SCREEN_EDGE_PADDING = 24;
    static constexpr int AVATAR_INSIDE_DOOR_Y = 350;
    static constexpr int HORIZONTAL_MOVE_DISTANCE = 10;
    static constexpr int LAST_FRAME_OF_MOVE_RIGHT_ANIMATION = 7;
    static constexpr int STAND_RIGHT_FRAME = 0;
    static constexpr int STAND_LEFT_FRAME = 0;
    static constexpr int MINIMUN_DISTANCE_TO_DOOR = 10;
    static constexpr int ENTER_DOOR_SPEED = 10;
    static constexpr int DOOR_SPEED = 5;

  private:
    std::unique_ptr<Sprite> door;
    std::unique_ptr<Sprite> avatar;
    int playerx;
    int playery;
    int destinationDoor;
    int doorDistance;
    enum
    {
        MOVE_LEFT,
        STOP_LEFT,
        STOP_RIGHT,
        MOVE_RIGHT
    } movement;
    bool enteringDoor;
    bool openingDoor;
    bool closingDoor;
    bool insideDoor;
    bool m_bNotified;
    bool flag_showWelcome;
    std::shared_ptr<ALLEGRO_BITMAP> m_background_image;
    bool testDoors();
    void move_player(int distanceInPixels);
    void enterDoor();
    void drawDoors(ALLEGRO_BITMAP *target);

  public:
    ModuleStarport();

    virtual bool on_init() override;
    virtual bool on_draw(ALLEGRO_BITMAP *target) override;
    virtual bool on_close() override;
    virtual bool on_event(ALLEGRO_EVENT *event) override;
    virtual bool on_key_down(ALLEGRO_KEYBOARD_EVENT *event) override;
    virtual bool on_key_up(ALLEGRO_KEYBOARD_EVENT *event) override;
};

#endif
