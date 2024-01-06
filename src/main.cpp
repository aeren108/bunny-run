#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h> // The GLFW header
#include <glm/glm.hpp> // GL Math library header
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/gl.h>   // The GL Header File
#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>

#include "global.h"
#include "model.h"
#include "bunny.h"
#include "checkpoint.h"

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

using namespace std;

int gWidth, gHeight;
float groundLeft, groundRight;

glm::mat4 projectionMatrix;
glm::mat4 viewingMatrix;
glm::vec3 eyePos(0, 0, 0);

Model ground;
Bunny* bunny;
Checkpoint* checkpoints[3];

bool lefthold = false;
bool righthold = false;

int score = 0;

GLuint text_program, gTextVBO;

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    GLuint TextureID;   // ID handle of the glyph texture
    glm::ivec2 Size;    // Size of glyph
    glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
    GLuint Advance;    // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;



bool ReadDataFromFile(
    const string& fileName, ///< [in]  Name of the shader file
    string&       data)     ///< [out] The contents of the file
{
    fstream myfile;

    // Open the input 
    myfile.open(fileName.c_str(), std::ios::in);

    if (myfile.is_open())
    {
        string curLine;

        while (getline(myfile, curLine))
        {
            data += curLine;
            if (!myfile.eof())
            {
                data += "\n";
            }
        }

        myfile.close();
    }
    else
    {
        return false;
    }

    return true;
}



void createVS(GLuint& program, const string& filename)
{
    string shaderSource;

    if (!ReadDataFromFile(filename, shaderSource))
    {
        cout << "Cannot find file name: " + filename << endl;
        exit(-1);
    }

    GLint length = shaderSource.length();
    const GLchar* shader = (const GLchar*) shaderSource.c_str();

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &shader, &length);
    glCompileShader(vs);

    char output[1024] = {0};
    glGetShaderInfoLog(vs, 1024, &length, output);
    printf("VS compile log: %s\n", output);

    glAttachShader(program, vs);
}

void createFS(GLuint& program, const string& filename)
{
    string shaderSource;

    if (!ReadDataFromFile(filename, shaderSource))
    {
        cout << "Cannot find file name: " + filename << endl;
        exit(-1);
    }

    GLint length = shaderSource.length();
    const GLchar* shader = (const GLchar*) shaderSource.c_str();

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &shader, &length);
    glCompileShader(fs);

    char output[1024] = {0};
    glGetShaderInfoLog(fs, 1024, &length, output);
    printf("FS compile log: %s\n", output);

    glAttachShader(program, fs);
}



/*
GLuint text_program;
createVS(text_program, "vert_text.glsl");
createFS(text_program, "frag_text.glsl");
glBindAttribLocation(text_program, 2, "vertex");
glLinkProgram(text_program);
*/



void initFonts(int windowWidth, int windowHeight)
{
    // Set OpenGL options
    //glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(windowWidth), 0.0f, static_cast<GLfloat>(windowHeight));
    glUseProgram(text_program);
    glUniformMatrix4fv(glGetUniformLocation(text_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // FreeType
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    }

    // Load font as face
    FT_Face face;
    if (FT_New_Face(ft, "/usr/share/fonts/truetype/liberation/LiberationSerif-Italic.ttf", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    }

    // Set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 48);

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

    // Load first 128 characters of ASCII set
    for (GLubyte c = 0; c < 128; c++)
    {
        // Load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
                );
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    // Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    //
    // Configure VBO for texture quads
    //
    glGenBuffers(1, &gTextVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gTextVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}



void renderText(const std::string& text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
    // Activate corresponding render state	
    glUseProgram(text_program);
    glUniform3f(glGetUniformLocation(text_program, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) 
    {
        Character ch = Characters[*c];

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;

        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },            
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },

            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }           
        };

        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);

        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, gTextVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

        //glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)

        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}














void init() {
    glEnable(GL_DEPTH_TEST);
	
	bunny = new Bunny();
	bunny->velocity.z = -0.01f;


	ground = Model("assets/quad.obj", "shaders/ground_frag.glsl", "shaders/ground_vert.glsl");
	ground.pos = glm::vec3(0.f, -1.f, -50.f);
	ground.scale = glm::vec3(2.f, 1.f, 50.f);
	ground.rotaxis = glm::vec3(1.f, 0.f, 0.f);
	ground.rotangle = - M_PI / 2;


	groundLeft = ground.minpos.x;
	groundRight = ground.maxpos.x;
	
	for (int i = 0; i < 3; ++i) {
		int pi = 1 - i;

		checkpoints[i] = new Checkpoint(false);
		checkpoints[i]->position.x = 0.f + pi * 1.25f;
		checkpoints[i]->position.z = -CP_BUNNY_DIST;
	}

	
	createVS(text_program, "shaders/vert_text.glsl");
	createFS(text_program, "shaders/frag_text.glsl");
	glBindAttribLocation(text_program, 2, "vertex");
	glLinkProgram(text_program);

	initFonts(gWidth, gHeight);

}

void reset() {

}

void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
        lefthold = true;
	else if (key == GLFW_KEY_D && action == GLFW_PRESS)
        righthold = true;
	else if (key == GLFW_KEY_A && action == GLFW_RELEASE)
        lefthold = false;
	else if (key == GLFW_KEY_D && action == GLFW_RELEASE)
        righthold = false;
	else if (key == GLFW_KEY_R && action == GLFW_PRESS)
        reset();
}

void reshape(GLFWwindow* window, int w, int h) {
	w = w < 1 ? 1 : w;
	h = h < 1 ? 1 : h;

	gWidth = w;
	gHeight = h;

	glViewport(0, 0, w, h);

	// Use perspective projection
	float fovyRad = (float)(90.0 / 180.0) * M_PI;
	projectionMatrix = glm::perspective(fovyRad, w / (float) h, 0.1f, 200.0f);

	// Assume default camera position and orientation (camera is at
	// (0, 0, 0) with looking at -z direction and its up vector pointing
	// at +y direction)
	// 
	//viewingMatrix = glm::mat4(1);
	viewingMatrix = glm::lookAt(eyePos, eyePos + glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
}


void checkCollisions() {
	for (Checkpoint* cp : checkpoints) {
		glm::vec3 cpMax = cp->getMaxPos();
		glm::vec3 cpMin = cp->getMinPos();
		glm::vec3 cpCen = cp->getCenter();
		glm::vec3 bnMin = bunny->getMinPos();
		glm::vec3 bnMax = bunny->getMaxPos();
		glm::vec3 bnCen = bunny->getCenter();

		bool collisionX = cpMax.x >= bnMin.x && cpMin.x <= bnMax.x;
    	bool collisionY = cpMax.y >= bnMin.y && cpMin.y <= bnMax.y;
    	bool collisionZ = cpMax.z >= bnMin.z && cpMin.z <= bnMax.z;

		bool collision = collisionX && collisionY && collisionZ;

		if (!collision) continue;

		if (cp->isHostile()) {
			bunny->setState(Bunny::DEAD);
			std::cout << "CP Min: " << cpMin.x << ", " << cpMin.y << ", " << cpMin.z << std::endl;
			std::cout << "CP Max: " << cpMax.x << ", " << cpMax.y << ", " << cpMax.z << std::endl;
			std::cout << "CP Cen: " << cpCen.x << ", " << cpCen.y << ", " << cpCen.z << std::endl;
			std::cout << "CP Pos: " << cp->position.x << ", " << cp->position.y << ", " << cp->position.z << std::endl;
			std::cout << "BN Min: " << bnMin.x << ", " << bnMin.y << ", " << bnMin.z << std::endl;
			std::cout << "BN Max: " << bnMax.x << ", " << bnMax.y << ", " << bnMax.z << std::endl;
			std::cout << "BN Cen: " << bnCen.x << ", " << bnCen.y << ", " << bnCen.z << std::endl;
			std::cout << "BN Pos: " << bunny->position.x << ", " << bunny->position.y << ", " << bunny->position.z << std::endl;

			std::cout << "!!!!! COLLISION !!!!!" << std::endl;
		} else {
			bunny->setState(Bunny::HAPPY);
			score += 10;
		}
	}
}

void update() {
	
	if (lefthold) {
		bunny->velocity.x = BUNNY_LEFT_VEL;
	} else if (righthold) {
		bunny->velocity.x = BUNNY_RIGHT_VEL;
	} else {
		bunny->velocity.x = 0.f;
	}

	bunny->update();
	for (Checkpoint* cp : checkpoints) cp->update();
	
	checkCollisions();

	//update ground
	ground.pos.z += bunny->velocity.z;
	// std::cout << "checkpoint: " << checkpoints[1]->position.x << "," << checkpoints[1]->position.y << ", " << checkpoints[1]->position.z << std::endl;
	// std::cout << "bunny: " << bunny->position.x << ","  << bunny->position.y << ", " << bunny->position.z << std::endl;

	//update camera pos
	eyePos.z = bunny->position.z + CAMERA_BUNNY_DIST;
	viewingMatrix = glm::lookAt(eyePos, eyePos + glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
}

void render() {
    glClearColor(0, 0, 0, 1);
	glClearDepth(1.0f);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	ground.render();
    bunny->render();
	for (Checkpoint* cp : checkpoints) cp->render();
	renderText("AAAAAAAAAAAAA", 1, 0.5, -1, glm::vec3(0, 1, 1));
}

void gameLoop(GLFWwindow* window) {
    while (!glfwWindowShouldClose(window)) {
		render();
		update();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

int main(int argc, char** argv) {
	GLFWwindow* window;
	if (!glfwInit()) {
		exit(-1);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	int width = 1000, height = 800;
	window = glfwCreateWindow(width, height, "Simple Example", NULL, NULL);

	if (!window) {
		glfwTerminate();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit()) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	char rendererInfo[512] = { 0 };
	strcpy(rendererInfo, (const char*)glGetString(GL_RENDERER)); // Use strcpy_s on Windows, strcpy on Linux
	strcat(rendererInfo, " - "); // Use strcpy_s on Windows, strcpy on Linux
	strcat(rendererInfo, (const char*)glGetString(GL_VERSION)); // Use strcpy_s on Windows, strcpy on Linux
	glfwSetWindowTitle(window, rendererInfo);

	init();

	glfwSetKeyCallback(window, keyboard);
	glfwSetWindowSizeCallback(window, reshape);

	reshape(window, width, height); // need to call this once ourselves
	gameLoop(window); // this does not return unless the window is closed

	glfwDestroyWindow(window);
	glfwTerminate();

	delete bunny;
	for (Checkpoint* cp : checkpoints) delete cp;

	return 0;
}

