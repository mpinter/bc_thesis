#include "../include/virtCam.hpp"

VirtualCamera::VirtualCamera(meshLoader* _cam) {
	default_facing=glm::vec3(0.0f,0.0f,1.0f);
	from=glm::vec3(0.0f,0.0f,0.0f);
	to=glm::vec3(0.0f,0.0f,0.0f);
	cam=_cam;
}

void VirtualCamera::draw(GLuint program,glm::mat4 VP) {
	glm::vec3 direction=to-from;
	glm::mat4 fixRotate;
	glm::vec4 tmpUp=faceVector(default_facing,direction)*up;
	fixRotate=faceVector(glm::vec3(tmpUp),glm::vec3(0.0f,1.0f,0.0f));
	glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), from);
	glm::mat4 translateToLens = glm::translate(glm::mat4(1.f), glm::vec3(0.0f,-0.15f,-0.2f));
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.f),glm::vec3(0.01f));
	GLint mvp = glGetUniformLocation(program, "uniMVP");
	glUniformMatrix4fv(mvp, 1, GL_FALSE, glm::value_ptr(VP*translateMatrix*fixRotate*faceVector(default_facing,direction)*translateToLens*scaleMatrix));
	cam->draw(program,false);
}

void VirtualCamera::lookAt(glm::vec3 _from, glm::vec3 _to, glm::vec3 _up) {
	from=_from;
	to=_to;
	up=glm::vec4(_up,0.0f);
}

glm::mat4 VirtualCamera::faceVector(glm::vec3 previous_facing,glm::vec3 vec) {
	vec=glm::normalize(vec);
	glm::normalize(previous_facing);
	GLfloat cosa = glm::dot(previous_facing,vec);
	glm::vec3 axis = glm::cross(previous_facing,vec);
	GLfloat angle = glm::degrees(glm::acos(cosa)); 
	return glm::rotate(glm::mat4(1.0f),angle,axis);
}

glm::vec3 VirtualCamera::getPos() {
	return from;
}