#include "global.h"
#include "bunny.h"

Bunny::Bunny() {
    model = Model("assets/bunny.obj", "shaders/bunny_frag.glsl", "shaders/bunny_vert.glsl");

    position = glm::vec3(0.f, MIN_IDLE_Y, -CAMERA_BUNNY_DIST);
    velocity = glm::vec3(0.f, 0.f, INITIAL_VELZ);

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

        velocity.z -= 0.0002f;
        hopVel += 0.00001f;
        happyAngVel += 0.0001f;
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
        model.rotangle_alt = M_PI / 2;
        velocity = glm::vec3(0.f);
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

void Bunny::reset() {
    setState(Bunny::IDLE);
    model.rotaxis = glm::vec3(0.f, 1.f, 0.f);
    model.rotangle = - M_PI / 2;
    model.rotangle_alt = 0;

    velocity = glm::vec3(0.f, 0.f, INITIAL_VELZ);
    hopVel = INITIAL_HOP_VEL;
    happyAngVel = INITIAL_HAPPY_ANGVEL;
}

void Bunny::setState(int state) {
    if (state != Bunny::IDLE && state != Bunny::HAPPY && state != Bunny::DEAD) 
        throw -1;

    this->state = state;
}

int Bunny::getState() { return state; }
