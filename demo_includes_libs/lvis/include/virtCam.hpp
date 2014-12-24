#include "common.hpp"
#include "sceneLoader.h"

class VirtualCamera {
public:
	VirtualCamera(meshLoader* _cam);
	void lookAt(glm::vec3 _from, glm::vec3 _to, glm::vec3 _up);
	void draw(GLuint program,glm::mat4 VP);
	glm::vec3 getPos();
private:
	glm::mat4 faceVector(glm::vec3 previous_facing,glm::vec3 vec);
	glm::mat4 model;
	glm::vec3 default_facing;
	glm::vec3 from,to;
	glm::vec4 up;
	meshLoader* cam;
};