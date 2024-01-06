#pragma once

#include "model.h"

class GameEntity {
public:
    virtual void update() = 0;
    virtual void render() = 0;

    glm::vec3 getMaxPos() { return model.maxpos; }
    glm::vec3 getMinPos() { return model.minpos; }
    glm::vec3 getCenter() { return model.center; }

    glm::vec3 position;
    glm::vec3 velocity;
    
protected:
    Model model;
};