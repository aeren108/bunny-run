#pragma once

#include <glm/glm.hpp> 

#define GROUND_Y -1.f
#define ENTITY_Y -0.97f

#define CAMERA_BUNNY_DIST 1.35f
#define CP_BUNNY_DIST 3.f
#define BUNNY_LEFT_VEL -0.05f
#define BUNNY_RIGHT_VEL -BUNNY_LEFT_VEL

extern int gWidth, gHeight;
extern glm::mat4 projectionMatrix;
extern glm::mat4 viewingMatrix;
extern glm::vec3 eyePos;

extern float groundLeft;
extern float groundRight;