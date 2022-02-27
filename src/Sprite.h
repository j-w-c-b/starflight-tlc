/*
 * GENERIC ALL-PURPOSE SPRITE CLASS
 *
 * Author: Jonathan Harbour
 * Date: Early 21st century
 *
 */

#ifndef SPRITE_H_
#define SPRITE_H_ 1

#include <memory>
#include <string>

#include <allegro5/allegro.h>

#include "Point2D.h"

class Sprite {
  protected:
    std::shared_ptr<ALLEGRO_BITMAP> frame;
    std::shared_ptr<ALLEGRO_BITMAP> image;
    double x;
    double y;
    double speed;
    double velX;
    double velY;
    float faceAngle;
    float moveAngle;
    bool alive;
    int state;
    int objectType;
    int direction;
    int width;
    int height;
    int delayX;
    int delayY;
    int countX;
    int countY;
    int currFrame;
    int totalFrames;
    int animDir;
    int frameCount;
    int frameDelay;
    int frameWidth;
    int frameHeight;
    int animColumns;
    int animStartX;
    int animStartY;
    int counter1;
    int counter2;
    int counter3;
    int threshold1;
    int threshold2;
    int threshold3;

  public:
    bool DebugMode;

    // accessors
    double getX() { return x; }
    double getY() { return y; }
    double getSpeed() { return speed; }
    double getVelX() { return velX; }
    double getVelY() { return velY; }
    float getFaceAngle() { return faceAngle; }
    float getMoveAngle() { return moveAngle; }
    int isAlive() { return alive; }
    int getState() { return state; }
    int getObjectType() { return objectType; }
    int getCurrFrame() { return currFrame; }
    int getTotalFrames() { return totalFrames; }
    int getFrameWidth() { return frameWidth; }
    int getFrameHeight() { return frameHeight; }
    int getCounter1() { return counter1; }
    int getCounter2() { return counter2; }
    int getCounter3() { return counter3; }
    int getThreshold1() { return threshold1; }
    int getThreshold2() { return threshold2; }
    int getThreshold3() { return threshold3; }

    // mutators
    void setX(double value) { x = value; }
    void setX(int value) { x = (double)value; }
    void setY(double value) { y = value; }
    void setY(int value) { y = (double)value; }
    void setPos(double x, double y) {
        setX(x);
        setY(y);
    }
    void setPos(Point2D p) {
        x = p.x;
        y = p.y;
    }
    void setSpeed(double value) { speed = value; }
    void setVelX(double value) { velX = value; }
    void setVelY(double value) { velY = value; }
    void setFaceAngle(float value) { faceAngle = value; }
    void setMoveAngle(float value) { moveAngle = value; }
    void setAlive(bool value) { alive = value; }
    void setState(int value) { state = value; }
    void setObjectType(int value) { objectType = value; }
    void setWidth(int value) { width = value; }
    void setHeight(int value) { height = value; }
    void setCurrFrame(int value) { currFrame = value; }
    void setTotalFrames(int value) { totalFrames = value; }
    void setFrameDelay(int value) { frameDelay = value; }
    void setFrameWidth(int value) { frameWidth = value; }
    void setFrameHeight(int value) { frameHeight = value; }
    void setAnimColumns(int value) { animColumns = value; }
    void setCounter1(int value) { counter1 = value; }
    void setCounter2(int value) { counter2 = value; }
    void setCounter3(int value) { counter3 = value; }
    void setThreshold1(int value) { threshold1 = value; }
    void setThreshold2(int value) { threshold2 = value; }
    void setThreshold3(int value) { threshold3 = value; }

    // METHODS
  public:
    Sprite();
    std::shared_ptr<ALLEGRO_BITMAP> getImage();
    bool setImage(std::shared_ptr<ALLEGRO_BITMAP> source);
    void move();
    void animate();
    void animate(int low, int high);
    bool collidedD(Sprite *other = nullptr); // distance based collision
    static double calcAngleMoveX(int angle);
    static double calcAngleMoveY(int angle);
    void draw(ALLEGRO_BITMAP *dest);
    void drawframe(ALLEGRO_BITMAP *dest);
    void drawframe_rotate(ALLEGRO_BITMAP *dest, int angle);
};

#endif
// vi: ft=cpp
