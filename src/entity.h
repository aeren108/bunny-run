#pragma once

#include "model.h"

class GameEntity {
public:
    virtual void update() = 0;
    virtual void render() = 0;

    glm::vec4 getMaxPos() { return model.maxpos; }
    glm::vec4 getMinPos() { return model.minpos; }
    glm::vec4 getDimensions() { return model.dimensions; }
    glm::vec4 getCenter() { return model.center; }

    glm::vec3 position;
    glm::vec3 velocity;
    
protected:
    Model model;
};