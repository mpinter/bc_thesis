#ifndef MESHDATA_H
#define MESHDATA_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


struct vertexData{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec3 color;
	float U,V;
};

struct textureData{
	unsigned int id;
	unsigned int type;
};

#endif
