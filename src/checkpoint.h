#pragma once

#include "global.h"
#include "entity.h"
#include "model.h"

#define INITIAL_VELZ 0.0f;
#define OFFSET_Y 0.95f

class Checkpoint : public GameEntity {

public:
    Checkpoint(bool hostile);

    void update();
    void render();

    bool isActive() { return active; }
    bool isHostile() { return hostile; }

private:

    float velZ = INITIAL_VELZ;
    bool active = true;
    bool hostile;
};