#include "model.h"

class Checkpoint {

public:
    Checkpoint(bool hostile) : hostile(hostile) {
        //TODO define model based on hostile parameter
    }

    void update();
    void render();

    bool isActive();
    bool isHostile();

private:

    const Model model;

    bool active = true;
    bool hostile;

    glm::vec3 position;
    glm::vec3 velocity;
    
    glm::vec4 boundingBox; //TODO: set bounding box
}