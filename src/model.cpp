#include "model.h"
using namespace std;

Model::Model(const std::string &filename, const std::string &fragfile, const std::string &vertfile) : 
	filename(filename), fragfile(fragfile), vertfile(vertfile) {
	
	bool s = parseObject(filename, vertices, textures, normals, faces);
	if (!s) std::cout << "Failed to load obj: " << filename << std::endl;
	
	scale = glm::vec3(1, 1, 1); 
	pos = glm::vec3(0, 0, 0);
	rotaxis = glm::vec3(0, 1, 0);
	rotangle = 0;

	initModel();
}

void Model::initModel() { 
    //Create a vertex array object and bind it so that current vao is this one
    glGenVertexArrays(1, &vao); assert(vao > 0);
    glBindVertexArray(vao);

    //Enable attrib 0 (positions) and 1 (normals)
    glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

    glGenBuffers(1, &vbo); //Create vertex buffer object (store positions and colors)
    glGenBuffers(1, &ebo); //Create element buffer object (store vertex ids for faces)

    //Bind vbo and ebo so that current vbo and ebo are the one we created
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    int vertexDataSize = vertices.size() * 3 * sizeof(GLfloat);
    int normalDataSize = normals.size() * 3 * sizeof(GLfloat);
    int indexDataSize = faces.size() * 3 * sizeof(GLuint);
	normalDataOffset = vertexDataSize;

    GLfloat* vertexData = new GLfloat[vertices.size() * 3];
	GLfloat* normalData = new GLfloat[normals.size() * 3];
	GLuint* indexData = new GLuint[faces.size() * 3];

	float minX = 1e6, maxX = -1e6;
	float minY = 1e6, maxY = -1e6;
	float minZ = 1e6, maxZ = -1e6;

	for (int i = 0; i < vertices.size(); ++i) {
		vertexData[3 * i] = vertices[i].x;
		vertexData[3 * i + 1] = vertices[i].y;
		vertexData[3 * i + 2] = vertices[i].z;

		minX = std::min(minX, vertices[i].x);
		maxX = std::max(maxX, vertices[i].x);
		minY = std::min(minY, vertices[i].y);
		maxY = std::max(maxY, vertices[i].y);
		minZ = std::min(minZ, vertices[i].z);
		maxZ = std::max(maxZ, vertices[i].z);
	}

    for (int i = 0; i < normals.size(); ++i) {
		normalData[3 * i] = normals[i].x;
		normalData[3 * i + 1] = normals[i].y;
		normalData[3 * i + 2] = normals[i].z;
	}

    for (int i = 0; i < faces.size(); ++i) {
		indexData[3 * i] = faces[i].vIndex[0];
		indexData[3 * i + 1] = faces[i].vIndex[1];
		indexData[3 * i + 2] = faces[i].vIndex[2];
	}

	dimensions_.x = maxX - minX; dimensions_.y = maxY - minY; dimensions_.z = maxZ - minZ;
	minpos_.x = minX; minpos_.y = minY; minpos_.z = minZ; minpos.w = 1.f;
	maxpos_.x = maxX; maxpos_.y = maxY; maxpos_.z = maxZ; maxpos.w = 1.f;
	center_ = (maxpos_ + minpos_) * 0.5f; center_.w = 1.f;

	glm::mat4 matT = glm::translate(glm::mat4(1.0), pos);
	glm::mat4 matS = glm::scale(glm::mat4(1.0), scale);
	glm::mat4 matST = glm::translate(glm::mat4(1.0), -glm::vec3(center_));
	glm::mat4 matR = glm::rotate<float>(glm::mat4(1.0), rotangle, rotaxis);
	modelingMatrix = matT * matS * matR * matST;

	dimensions.x = dimensions_.x * scale.x; 
	dimensions.y = dimensions_.y * scale.y; 
	dimensions.z = dimensions_.y * scale.z;

	center = modelingMatrix * center_;
	minpos = center - dimensions * 0.5f;
	maxpos = center + dimensions * 0.5f;

    //Fill vbo buffer (positions, normals) and ebo (face indices)
    glBufferData(GL_ARRAY_BUFFER, vertexDataSize + normalDataSize, 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertexDataSize, vertexData);
	glBufferSubData(GL_ARRAY_BUFFER, vertexDataSize, normalDataSize, normalData);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSize, indexData, GL_STATIC_DRAW);

	// done copying to GPU memory; can free now from CPU memory
	delete[] vertexData;
	delete[] normalData;
	delete[] indexData;

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertexDataSize));

	//Init shaders
	program = glCreateProgram();

	GLuint vs1 = createVertexShader(vertfile.c_str());
	GLuint fs1 = createFragmentShader(fragfile.c_str());

	glAttachShader(program, vs1);
	glAttachShader(program, fs1);

	glLinkProgram(program);
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);

	if (status != GL_TRUE) {
		cout << "Program link failed" << endl;
		exit(-1);
	}

	modelingMatrixLoc = glGetUniformLocation(program, "modelingMatrix");
	viewingMatrixLoc = glGetUniformLocation(program, "viewingMatrix");
	projectionMatrixLoc = glGetUniformLocation(program, "projectionMatrix");
	eyePosLoc = glGetUniformLocation(program, "eyePos");

	assert(glGetError() == GL_NONE);
}

void Model::render() {
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(normalDataOffset));

	glm::mat4 matT = glm::translate(glm::mat4(1.0), pos);
	glm::mat4 matS = glm::scale(glm::mat4(1.0), scale);
	glm::mat4 matST = glm::translate(glm::mat4(1.0),  -glm::vec3(center_));
	glm::mat4 matR = glm::rotate<float>(glm::mat4(1.0), rotangle, rotaxis);
	modelingMatrix = matT * matS * matR * matST; // starting from right side, rotate around Y to turn back, then rotate around Z some more at each frame, then translate.

	dimensions.x = dimensions_.x * scale.x; 
	dimensions.y = dimensions_.y * scale.y; 
	dimensions.z = dimensions_.y * scale.z;
	
	center = modelingMatrix * center_;
	minpos = center - dimensions * 0.5f;
	maxpos = center + dimensions * 0.5f;
	
	std::cout << filename << " dim"<< ": " << dimensions.x << ","  << dimensions.y << ", " << dimensions.z << std::endl;
	std::cout << filename << " cneter"<< ": " << center.x << ","  << center.y << ", " << center.z << std::endl;

	glUseProgram(program);
	glUniformMatrix4fv(projectionMatrixLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(viewingMatrixLoc, 1, GL_FALSE, glm::value_ptr(viewingMatrix));
	glUniformMatrix4fv(modelingMatrixLoc, 1, GL_FALSE, glm::value_ptr(modelingMatrix));
	glUniform3fv(eyePosLoc, 1, glm::value_ptr(eyePos));
	
	glDrawElements(GL_TRIANGLES, faces.size() * 3, GL_UNSIGNED_INT, 0);
}

bool parseObject(const string& fileName, vector<Vertex> &gVertices, vector<Texture> &gTextures, vector<Normal> &gNormals, vector<Face> &gFaces) {
	fstream myfile;

	// Open the input 
	myfile.open(fileName.c_str(), std::ios::in);

	if (myfile.is_open()) {
		string curLine;

		while (getline(myfile, curLine)) {
			stringstream str(curLine);
			GLfloat c1, c2, c3;
			GLuint index[9];
			string tmp;

			if (curLine.length() >= 2) {
				if (curLine[0] == 'v') {
					if (curLine[1] == 't') { // texture
						str >> tmp; // consume "vt"
						str >> c1 >> c2;
						gTextures.push_back(Texture(c1, c2));
					} else if (curLine[1] == 'n') { // normal
						str >> tmp; // consume "vn"
						str >> c1 >> c2 >> c3;
						gNormals.push_back(Normal(c1, c2, c3));
					} else {// vertex
						str >> tmp; // consume "v"
						str >> c1 >> c2 >> c3;
						gVertices.push_back(Vertex(c1, c2, c3));
					}
				} else if (curLine[0] == 'f') { // face
					str >> tmp; // consume "f"
					char c;
					int vIndex[3], nIndex[3], tIndex[3];
					str >> vIndex[0]; str >> c >> c; // consume "//"
					str >> nIndex[0];
					str >> vIndex[1]; str >> c >> c; // consume "//"
					str >> nIndex[1];
					str >> vIndex[2]; str >> c >> c; // consume "//"
					str >> nIndex[2];

					assert(vIndex[0] == nIndex[0] &&
						vIndex[1] == nIndex[1] &&
						vIndex[2] == nIndex[2]); // a limitation for now

					// make indices start from 0
					for (int c = 0; c < 3; ++c) {
						vIndex[c] -= 1;
						nIndex[c] -= 1;
						tIndex[c] -= 1;
					}

					gFaces.push_back(Face(vIndex, tIndex, nIndex));
				} else {
					cout << "Ignoring unidentified line in obj file: " << curLine << endl;
				}
			}

			//data += curLine;
			if (!myfile.eof()) {
				//data += "\n";
			}
		}

		myfile.close();
	} else {
		return false;
	}

	assert(gVertices.size() == gNormals.size());

	return true;
}

bool readDataFromFile( const string& fileName, string& data) {
	fstream myfile;

	// Open the input 
	myfile.open(fileName.c_str(), std::ios::in);

	if (myfile.is_open()) {
		string curLine;

		while (getline(myfile, curLine)){
			data += curLine;
			if (!myfile.eof()) {
				data += "\n";
			}
		}

		myfile.close();
	} else {
		return false;
	}

	return true;
}

GLuint createVertexShader(const char* shaderName) {
	string shaderSource;

	string filename(shaderName);
	if (!readDataFromFile(filename, shaderSource)) {
		cout << "Cannot find file name: " + filename << endl;
		exit(-1);
	}

	GLint length = shaderSource.length();
	const GLchar* shader = (const GLchar*)shaderSource.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &shader, &length);
	glCompileShader(vs);

	char output[1024] = { 0 };
	glGetShaderInfoLog(vs, 1024, &length, output);
	printf("VS compile log: %s\n", output);

	return vs;
}

GLuint createFragmentShader(const char* shaderName) {
	string shaderSource;

	string filename(shaderName);
	if (!readDataFromFile(filename, shaderSource)) {
		cout << "Cannot find file name: " + filename << endl;
		exit(-1);
	}

	GLint length = shaderSource.length();
	const GLchar* shader = (const GLchar*)shaderSource.c_str();

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &shader, &length);
	glCompileShader(fs);

	char output[1024] = { 0 };
	glGetShaderInfoLog(fs, 1024, &length, output);
	printf("FS compile log: %s\n", output);

	return fs;
}

