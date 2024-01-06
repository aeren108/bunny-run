#pragma once

#include "global.h"
#include "entity.h"
#include "model.h"

#define OFFSET_Y 0.95f

class Checkpoint : public GameEntity {

public:
    Checkpoint(bool hostile);

    void update();
    void render();

    void setActive(bool active) { this->active = active; }
    bool isActive() { return active; }
    bool isHostile() { return hostile; }

private:

    float velZ = 0;
    bool active = true;
    bool hostile;
};