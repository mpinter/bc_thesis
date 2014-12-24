#include "common.hpp"

class Camera {
public:
	Camera();
	glm::mat4 getVP();
	glm::vec3 getPos();
	glm::mat4 getView();
	void zoom();
	void mouseRotate();
	void setProjection(GLfloat _fov,GLfloat _aspX,GLfloat _aspY,GLfloat _rangeMin, GLfloat _rangeMax);
	void setProjection(GLfloat _aspX,GLfloat _aspY);
	void setView();
	void setViewSpherical(GLfloat _xi,GLfloat _fi);
	void setDeltaSpherical(GLfloat _xi,GLfloat _fi);
	void memorizeSpherical();
	void setMemSpherical(GLfloat _xi,GLfloat _fi);
	void setDeltaDistance(GLfloat _dist);
	void move(glm::vec3 vec);
	bool isFree();
	void switchFree();
	GLfloat getDistance();
private:
	GLfloat xi,fi,dist,modXi,modFi,fov,aspX,aspY,rangeMin,rangeMax;
	glm::mat4 view, proj;
	glm::vec3 pos;
	bool freeMode;
};