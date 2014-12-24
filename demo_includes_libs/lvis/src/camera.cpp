#include "../include/camera.hpp"

Camera::Camera() {
	xi=0.7f; //vertical angle
	fi=0.7f; //horizontal angle (both in radians)
	dist=10.0f;
	freeMode=false;
	pos=glm::vec3(0.0f,0.0f,0.0f);
	proj=glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 1000.0f);
	setProjection(45.0f,4.0f,3.0f,0.1f,1000.0f);
	setView();
}

void Camera::setProjection(GLfloat _fov,GLfloat _aspX,GLfloat _aspY,GLfloat _rangeMin, GLfloat _rangeMax) {
	fov=_fov;
	aspX=_aspX;
	aspY=_aspY;
	rangeMin=_rangeMin;
	rangeMax=_rangeMax;
	proj=glm::perspective(fov, aspX / aspY, rangeMin, rangeMax);	
}

void Camera::setProjection(GLfloat _aspX,GLfloat _aspY) {
	aspX=_aspX;
	aspY=_aspY;
	proj=glm::perspective(fov, aspX / aspY, rangeMin, rangeMax);
}

void Camera::setViewSpherical(GLfloat _xi,GLfloat _fi) {
	xi=_xi;
	fi=_fi;
	setView();
}

void Camera::setDeltaSpherical(GLfloat _xi,GLfloat _fi) {
	xi+=_xi;
	fi+=_fi;
	setView();
}

void Camera::memorizeSpherical() {
	modFi=fi;
	modXi=xi;
}

void Camera::setMemSpherical(GLfloat _xi,GLfloat _fi) {
	xi=modXi+_xi;
	fi=modFi+_fi;
	setView();	
}

void Camera::move(glm::vec3 vec) {
	if (freeMode) {
		glm::vec3 direction=glm::normalize(glm::vec3(
	    	cos(xi) * sin(fi),
	    	sin(xi),
	    	cos(xi) * cos(fi)
		));
		glm::vec3 right = glm::normalize(glm::vec3(
	    	sin(fi - 3.14f/2.0f),
	    	0,
	    	cos(fi - 3.14f/2.0f)
		));
		glm::vec3 up = glm::normalize(glm::cross( right, direction ));
		pos+=vec.x*direction+vec.y*up+vec.z*right;
		setView();
	}
}

void Camera::setDeltaDistance(GLfloat _dist) {
	dist+=_dist;
	setView();
}

void Camera::setView() {
	glm::vec3 direction(
    	cos(xi) * sin(fi),
    	sin(xi),
    	cos(xi) * cos(fi)
	);
	glm::vec3 right = glm::vec3(
    	sin(fi - 3.14f/2.0f),
    	0,
    	cos(fi - 3.14f/2.0f)
	);
	glm::vec3 up = glm::cross( right, direction );
	if (freeMode) {
		view = glm::lookAt(
    		pos,           
    		pos+direction, 
    		up                  
		);
	} else {
		view = glm::lookAt(
    		dist*direction,           
    		glm::vec3(0.0f,0.0f,0.0f), 
    		up                  
		);
	}
}

glm::vec3 Camera::getPos() {
	if (freeMode) return pos;
	glm::vec3 position(
    	cos(xi) * sin(fi),
    	sin(xi),
    	cos(xi) * cos(fi)
	);
	return dist*position;
}

glm::mat4 Camera::getVP() {
	return proj*view;
}

glm::mat4 Camera::getView() {
	return view;
}

bool Camera::isFree() {
	return freeMode;
}

void Camera::switchFree() {
	freeMode=!freeMode;
	setView();
}

GLfloat Camera::getDistance() {
	return dist;
}