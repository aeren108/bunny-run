#pragma once

#include "global.h"
#include "entity.h"
#include "model.h"

#define MIN_IDLE_Y ENTITY_Y + 0.21f
#define MAX_IDLE_Y MIN_IDLE_Y + 0.1f

#define INITIAL_VELZ -0.085f
#define INITIAL_HOP_VEL 0.0085f
#define INITIAL_HAPPY_ANGVEL 0.15f
#define INITIAL_ROTANGLE - M_PI / 2

class Bunny : public GameEntity {
public:
    Bunny();

    void update();
    void render();
    void reset();
    void setState(int state);
    int getState();

    static const int IDLE = 0;
    static const int HAPPY = 1;
    static const int DEAD = 2;

private:

    int state = Bunny::IDLE;
    bool idleUp = true;

    float hopVel = INITIAL_HOP_VEL;
    float happyAngVel = INITIAL_HAPPY_ANGVEL;
    float totalRotation = 0;
};