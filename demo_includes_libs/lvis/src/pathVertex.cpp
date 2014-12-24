#include "../include/pathVertex.hpp"

PathVertex::PathVertex() {
	pos=glm::vec3(((GLfloat)(rand()%100-50))/20.0f,((GLfloat)(rand()%100-50))/20.0f,((GLfloat)(rand()%100-50))/20.0f);
}

PathVertex::PathVertex (GLfloat x,GLfloat y,GLfloat z) {
	pos=glm::vec3(x,y,z);
}

PathVertex::PathVertex (glm::vec3 _pos) {
	pos=_pos;
}

glm::vec3 PathVertex::getPos() {
	return pos;
}

std::string PathVertex::getInfo() {
	std::stringstream ss;
	ss.precision(4);
	ss << "Vertex - x: " << pos.x << " y: " << pos.y << " z: " << pos.z << "\n";
	return ss.str(); 
}