#include "global.h"
#include "bunny.h"

Bunny::Bunny() {
    model = Model("assets/bunny.obj", "shaders/frag2.glsl", "shaders/vert2.glsl");

    position = glm::vec3(0.f, MIN_IDLE_Y, -CAMERA_BUNNY_DIST);
    velocity = glm::vec3(0.f, 0.f, 0.f);

	model.scale = glm::vec3(0.175, 0.175, 0.175);
	model.rotangle = - M_PI / 2;
    model.pos = position;
}

void Bunny::update() {
    if (state != DEAD) {
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
    }

    if (state == Bunny::HAPPY) {
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

    //Check boundary collisions
    if (position.x + velocity.x > groundLeft - 0.5f && position.x + velocity.x < groundRight + 0.5f)
        position.x += velocity.x;

    position.y += velocity.y;
    position.z += velocity.z;
    model.pos = position;
}

void Bunny::render() {
    model.render();
}

void Bunny::setState(int state) {
    if (state != Bunny::IDLE && state != Bunny::HAPPY && state != Bunny::DEAD) 
        throw -1;

    this->state = state;
}

int Bunny::getState() { return state; }
