#pragma once
#include "model.h"

#define MAX_IDLE_Y -0.87f
#define MIN_IDLE_Y -0.97f

#define INITIAL_HOP_VEL 0.005f
#define INITIAL_HAPPY_ANGVEL 0.15f
#define INITIAL_ROTANGLE - M_PI / 2
class Bunny {
public:
    Bunny();

    void update();
    void render();
    void setState(int state);
    int getState() { return state; }

    static const int IDLE = 0;
    static const int HAPPY = 1;
    static const int DEAD = 2;

private:

    Model model;

    int state = Bunny::IDLE;
    bool idleUp = true;

    glm::vec3 position;
    glm::vec3 velocity;

    glm::vec4 boundingBox; //TODO: set bounding box

    float hopVel = INITIAL_HOP_VEL;
    float happyAngVel = INITIAL_HAPPY_ANGVEL;
    float totalRotation = 0;
};