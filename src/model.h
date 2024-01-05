#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h> // The GLFW header
#include <glm/glm.hpp> // GL Math library header
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

#include "global.h"

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

struct Vertex {
	Vertex(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) { }
	GLfloat x, y, z;
};

struct Texture {
	Texture(GLfloat inU, GLfloat inV) : u(inU), v(inV) { }
	GLfloat u, v;
};

struct Normal {
	Normal(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) { }
	GLfloat x, y, z;
};

struct Face {
	Face(int v[], int t[], int n[]) {
		vIndex[0] = v[0];
		vIndex[1] = v[1];
		vIndex[2] = v[2];
		tIndex[0] = t[0];
		tIndex[1] = t[1];
		tIndex[2] = t[2];
		nIndex[0] = n[0];
		nIndex[1] = n[1];
		nIndex[2] = n[2];
	}
    
	GLuint vIndex[3], tIndex[3], nIndex[3];
};

bool parseObject(const std::string& fileName, std::vector<Vertex> &gVertices, std::vector<Texture> &gTextures, std::vector<Normal> &gNormals, std::vector<Face> &gFaces);
GLuint createVertexShader(const char* shaderName);
GLuint createFragmentShader(const char* shaderName);

class Model {
public:
	Model() {}
	Model(const std::string &filename, const std::string &fragfile, const std::string &vertfile) : 
		filename(filename), fragfile(fragfile), vertfile(vertfile) {
		bool s = parseObject(filename, vertices, textures, normals, faces);
		if (!s) std::cout << "Failed to load obj: " << filename << std::endl;
		
		scale = glm::vec3(1, 1, 1); 
		pos = glm::vec3(0, 0, -2);
		rotaxis = glm::vec3(0, 1, 0);
		rotangle = 0;
		
		initModel();
	}

	void render();

	glm::vec3 scale;
	glm::vec3 pos;
	glm::vec3 rotaxis; //TODO handle rotation
	float rotangle;

private:
	void initModel();

	std::vector<Vertex> vertices;
	std::vector<Texture> textures;
	std::vector<Normal> normals;
	std::vector<Face> faces;

	std::string filename;
	std::string fragfile, vertfile;
	
	GLuint vao, vbo, ebo;
	GLuint program;

	GLint modelingMatrixLoc;
	GLint viewingMatrixLoc;
	GLint projectionMatrixLoc;
	GLint eyePosLoc;

	glm::mat4 modelingMatrix;

	int normalDataOffset;
};
