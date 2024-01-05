#include "bunny.h"

Bunny::Bunny() {
    model = Model("assets/bunny.obj", "shaders/frag.glsl", "shaders/vert.glsl");

    position = glm::vec3(0.f, -0.95f, -1.35f);
    velocity = glm::vec3(0.f, 0.f, 0.f);

    model.pos = position;
	model.scale = glm::vec3(0.175, 0.175, 0.175);
	model.rotangle = - M_PI / 2;
}

void Bunny::update() {
    position += velocity;

    if (state == Bunny::IDLE) {
        if (idleUp) {
            velocity.y = hopVel;
            if (position.y + velocity.y >= MAX_IDLE_Y) {
                velocity.y = -hopVel;
                idleUp = false;
            }
        } else {
            velocity.y = -hopVel;
            if (position.y + velocity.y <= MIN_IDLE_Y) {
                velocity.y = hopVel;
                idleUp = true;
            }
        }
    } else if (state == Bunny::HAPPY) {
        model.rotangle += happyAngVel;

        totalRotation += happyAngVel;
        if (totalRotation >= 2 * M_PI) {
            setState(Bunny::IDLE);
            model.rotangle = INITIAL_ROTANGLE;
            totalRotation = 0;
        }
    } else if (state == Bunny::DEAD) {
        model.rotaxis = glm::vec3(1.f, 0.f, 1.f);
        model.rotangle = - M_PI / 2;
    }

    model.pos = this->position;
}

void Bunny::render() {
    model.render();
}

void Bunny::setState(int state) {
    if (state != Bunny::IDLE && state != Bunny::HAPPY && state != Bunny::DEAD) 
        throw -1;

    this->state = state;
}
