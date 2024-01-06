#version 330 core

// All of the following variables could be defined in the OpenGL
// program and passed to this shader as uniform variables. This
// would be necessary if their values could change during runtim.
// However, we will not change them and therefore we define them 
// here for simplicity.

uniform vec3 eyePos;

in vec4 fragWorldPos;
in vec3 fragWorldNor;

out vec4 fragColor;

void main(void)
{
    float scale = 1;
    float offset = 2;

    bool x = bool(int(((fragWorldPos.x + offset) * scale)) % 2);
    bool y = bool(int(((fragWorldPos.y + offset) * scale)) % 2);
    bool z = bool(int(((fragWorldPos.z + offset) * scale)) % 2);

    bool xorXY = x != y;
    //if (xorXY < 0) xorXY = -xorXY;

    if (xorXY != z){
        fragColor = vec4(0, 0, 0, 1);
    }

    else {
        fragColor = vec4(1, 1, 1, 1);
    }

}
