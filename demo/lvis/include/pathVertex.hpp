#include "common.hpp"

class PathVertex {
	public:
		PathVertex();
		PathVertex(GLfloat x,GLfloat y,GLfloat z);
		PathVertex (glm::vec3 _pos);
		glm::vec3 getPos();
		std::string getInfo();
	private:
		glm::vec3 pos;
};