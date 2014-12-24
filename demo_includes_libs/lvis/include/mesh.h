#ifndef MESH_H
#define MESH_H
#include <iostream>
#include "meshData.h"
#include <vector>
#include <string>
#ifndef OGL_INCLUDES
#define OGL_INCLUDES
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

class Mesh{
	std::vector<vertexData> data;
	std::vector<textureData> textures;
	std::vector<unsigned int> indices;
	GLuint VBO;
	GLuint IND;
	public:
		Mesh(std::vector<vertexData>* vd,std::vector<unsigned int>* id,std::vector<textureData>* td=NULL);
		~Mesh();
		void draw(unsigned int programId, bool wireframe);
		GLuint getVBO();
};
#endif
