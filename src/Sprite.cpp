/*
 * GENERIC ALL-PURPOSE SPRITE CLASS
 *
 * Author: Jonathan Harbour
 * Date: Early 21st century
 *
 * If you don't like this class, make your own, but don't screw with this one.
 *
 */

#include <sstream>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "Game.h"
#include "Sprite.h"

#define PI 3.1415926535
#define PI_div_180 0.017453292519444

Sprite::Sprite() {
    image = NULL;
    frame = NULL;
    alive = true;
    direction = 0;
    animColumns = 1;
    animStartX = 0;
    animStartY = 0;
    x = 0.0;
    y = 0.0;
    width = 0;
    height = 0;
    delayX = 0;
    delayY = 0;
    countX = 0;
    countY = 0;
    velX = 0.0;
    velY = 0.0;
    speed = 0.0;
    currFrame = 0;
    totalFrames = 1;
    frameCount = 0;
    frameDelay = 0;
    frameWidth = 0;
    frameHeight = 0;
    animDir = 1;
    faceAngle = 0.0f;
    moveAngle = 0.0f;
    counter1 = 0;
    counter2 = 0;
    counter3 = 0;
    threshold1 = 0;
    threshold2 = 0;
    threshold3 = 0;
    bLoaded = false;
    DebugMode = false;
}

Sprite::~Sprite() {
    // prevent destroying ALLEGRO_BITMAP if it was passed as a pointer rather
    // than loaded
    if (bLoaded) {
        if (this->image != NULL) {
            al_destroy_bitmap(this->image);
            this->image = NULL;
        }
    }

    if (this->frame != NULL) {
        al_destroy_bitmap(this->frame);
        this->frame = NULL;
    }
}

bool
Sprite::load(const char *filename) {
    this->image = al_load_bitmap(filename);
    if (!this->image) {
        std::ostringstream s;
        s << "Error loading sprite file: " << filename;
        g_game->message(s.str().c_str());
        return false;
    }
    this->width = al_get_bitmap_width(image);
    this->height = al_get_bitmap_height(image);
    this->bLoaded = true;
    al_convert_mask_to_alpha(image, MASK_COLOR);

    // default frame size equals whole image size unless manually changed
    this->frameWidth = this->width;
    this->frameHeight = this->height;
    return true;
}

bool
Sprite::load(std::string filename) {
    return load(filename.c_str());
}

ALLEGRO_BITMAP *
Sprite::getImage() {
    if (this->image)
        return this->image;
    else
        return NULL;
}
bool
Sprite::setImage(ALLEGRO_BITMAP *source) {
    // if new source image is null, then abort
    if (!source)
        return false;

    // if old image exists, it must be freed first
    if (this->image && bLoaded) {
        al_destroy_bitmap(this->image);
        this->image = NULL;
    }

    this->image = source;
    this->width = al_get_bitmap_width(source);
    this->height = al_get_bitmap_height(source);
    this->frameWidth = al_get_bitmap_width(source);
    this->frameHeight = al_get_bitmap_height(source);
    this->bLoaded = false;

    return true;
}

void
Sprite::draw(ALLEGRO_BITMAP *dest) {
    al_set_target_bitmap(dest);
    if (this->image)
        al_draw_bitmap(this->image, (int)this->x, (int)this->y, 0);
}

// draw normally with optional alpha channel support
void
Sprite::drawframe(ALLEGRO_BITMAP *dest) {
    if (!image)
        return;

    int fx = animStartX + (currFrame % animColumns) * frameWidth;
    int fy = animStartY + (currFrame / animColumns) * frameHeight;

    al_set_target_bitmap(dest);

    al_draw_bitmap_region(
        this->image, fx, fy, frameWidth, frameHeight, (int)x, (int)y, 0);

    if (DebugMode) {
        al_draw_rectangle(
            (int)x, (int)y, (int)x + frameWidth, (int)y + frameHeight, BLUE, 1);
    }
}

// draw with rotation
void
Sprite::drawframe_rotate(ALLEGRO_BITMAP *dest, int angle) {
    if (!image)
        return;

    // create scratch frame if necessary
    if (!frame) {
        frame = al_create_bitmap(frameWidth, frameHeight);
    }

    // first, draw frame normally but send it to the scratch frame image
    int fx = animStartX + (currFrame % animColumns) * frameWidth;
    int fy = animStartY + (currFrame / animColumns) * frameHeight;
    al_set_target_bitmap(frame);
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));
    al_draw_bitmap_region(image, fx, fy, frameWidth, frameHeight, 0, 0, 0);

    al_set_target_bitmap(dest);
    al_draw_rotated_bitmap(frame,
                           al_get_bitmap_width(frame) / 2,
                           al_get_bitmap_height(frame) / 2,
                           x + al_get_bitmap_width(frame) / 2,
                           y + al_get_bitmap_height(frame) / 2,
                           angle * PI_div_180,
                           0);

    if (DebugMode) {
        al_draw_rectangle(
            (int)x, (int)y, (int)x + frameWidth, (int)y + frameHeight, BLUE, 1);
    }
}

void
Sprite::move() {
    // update x position
    if (++countX > delayX) {
        countX = 0;
        x += velX;
    }

    // update y position
    if (++countY > delayY) {
        countY = 0;
        y += velY;
    }
}

void
Sprite::animate() {
    animate(0, totalFrames - 1);
}

void
Sprite::animate(int low, int high) {
    // update frame based on animdir
    if (++frameCount > frameDelay) {
        frameCount = 0;
        currFrame += animDir;

        if (currFrame < low) {
            currFrame = high;
        }
        if (currFrame > high) {
            currFrame = low;
        }
    }
}

/*
 * Distance based collision detection
 */
bool
Sprite::collidedD(Sprite *other) {
    if (other == NULL)
        return false;

    // calculate radius 1
    double radius1 = this->getFrameWidth() * 0.4;

    // point = center of sprite 1
    double x1 = this->getX() + this->getFrameWidth() / 2;
    double y1 = this->getY() + this->getFrameHeight() / 2;

    // calculate radius 2
    double radius2 = other->getFrameWidth() * 0.4;

    // point = center of sprite 2
    double x2 = other->getX() + other->getFrameWidth() / 2;
    double y2 = other->getY() + other->getFrameHeight() / 2;

    // calculate distance
    double deltaX = (x2 - x1);
    double deltaY = (y2 - y1);
    double dist = sqrt(deltaX * deltaX + deltaY * deltaY);

    // return distance comparison
    return (dist < radius1 + radius2);
}

double
Sprite::calcAngleMoveX(int angle) {
    // calculate X movement value based on direction angle
    return (double)cos(angle * PI_div_180);
}

// calculate Y movement value based on direction angle
double
Sprite::calcAngleMoveY(int angle) {
    return (double)sin(angle * PI_div_180);
}
