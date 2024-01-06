#include "checkpoint.h"

Checkpoint::Checkpoint(bool hostile) : hostile(hostile) {
    /*if (hostile) model = Model("assets/cube.obj", "shaders/hcube_frag.glsl", "shaders/hcube_vert.glsl");
    else model = Model("assets/cube.obj", "shaders/cube_frag.glsl", "shaders/cube_vert.glsl");*/

    if (hostile) model = Model("assets/cube.obj", "shaders/frag.glsl", "shaders/vert.glsl");
    else model = Model("assets/cube.obj", "shaders/frag.glsl", "shaders/vert.glsl");
    
    position = glm::vec3(0.f, ENTITY_Y + 0.25f, -1.35f);
    model.scale = glm::vec3(0.2f, 0.5f, 0.2f);
    model.pos = position;
}

void Checkpoint::update() { 
    if (active)
        velocity = glm::vec3(0.f, 0.f, velZ);

    position += velocity;
    model.pos = position;
}

void Checkpoint::render() {
    if (active)
        model.render();
}
