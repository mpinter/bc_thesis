#include "../include/sphereSelect.hpp"

SphereSelect::SphereSelect(GLuint _index,GLuint _type,meshLoader* _sphere,meshLoader* _arrow,glm::vec3 pos) {	//index zabudnuty a ziskany len z rgb, ak nieco nepojde tak koli tomuto
	index=_index;	
	r = index&0xFF;
    g = (index>>8)&0xFF;
	b = (index>>16)&0xFF;
	type=_type;
	sphere=_sphere;
	arrow=_arrow;
	scale=1.0f;
	tempScale=0.0f;
	transVector=pos;
	tempTrans=glm::vec3(0.0f,0.0f,0.0f);
}

void SphereSelect::setType(GLuint _type) {
	type=_type;
}

GLuint SphereSelect::getType() {
	return type;
}

glm::vec3 SphereSelect::getPickColor() {
   return glm::vec3(GLfloat(r)/255.0f, GLfloat(g)/255.0f, GLfloat(b)/255.0f);
}

bool SphereSelect::pointInside(glm::vec3 point) {
	if (glm::distance(point,transVector)<=scale) return true;
	return false;
}

void SphereSelect::translate(GLfloat x,GLfloat y,GLfloat z) {
	tempTrans=glm::vec3(x,y,z);	
}

void SphereSelect::translate(glm::vec3 v) {
	tempTrans=v;	
}

void SphereSelect::finishTranslate() {
	transVector+=tempTrans;
	tempTrans=glm::vec3(0.0f,0.0f,0.0f);
}

void SphereSelect::scaling(GLfloat _scale) {
	tempScale=_scale;
}

void SphereSelect::finishScaling() {
	scale+=tempScale;
	tempScale=0.0f;
}

void SphereSelect::draw(GLuint program,glm::mat4 VP,bool wireframe) {
	glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), transVector+tempTrans);
	glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.f),glm::vec3(scale+tempScale));
    GLint mvp = glGetUniformLocation(program, "uniMVP");
    GLint uniColor= glGetUniformLocation(program, "uniColor");
    glUniformMatrix4fv(mvp, 1, GL_FALSE, glm::value_ptr(VP*translateMatrix*scaleMatrix));
    if (wireframe) {
	    switch (type) {
	    	case 1:
	    		glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(0.0f,1.0f,0.0f)));
	    		break;
	    	case 2:
	    		glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(1.0f,0.0f,0.0f)));
	    		break;
	    	case 3:
	    		glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(1.0f,1.0f,0.0f)));
	    		break;
	    	default:
	    		glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(1.0f,1.0f,1.0f)));
	    		break;
	    }
	    sphere->draw(program,true);
	} else {
		glUniform3fv(uniColor, 1, glm::value_ptr(getPickColor()));
		sphere->draw(program,false);
	}
    
}

void SphereSelect::drawAxes(GLuint program,glm::mat4 VP,bool scaling, glm::vec3 camPos) {
	glm::vec3 finalVec=transVector+tempTrans;
	glm::mat4 translateMatrix = glm::translate(glm::mat4(1.f), finalVec);
	glm::mat4 rotateMatrix,rotTrans,scaleMatrix;
	GLint sigX=-1*sgn(finalVec.x-camPos.x);
	GLint sigY=-1*sgn(finalVec.y-camPos.y);
	GLint sigZ=-1*sgn(finalVec.z-camPos.z);

	GLint positionAttrib = glGetAttribLocation(program,"vertex");
	GLint uniColor= glGetUniformLocation(program, "uniColor");
	GLint mvp =glGetUniformLocation(program, "uniMVP");

	if (scaling) {

		scaleMatrix = glm::scale(glm::mat4(1.f),glm::vec3(scale+tempScale));
		rotTrans=glm::translate(glm::mat4(scale), glm::vec3(0.0f,scale+tempScale,0.0f));
		glUniformMatrix4fv(mvp, 1, GL_FALSE, glm::value_ptr(VP*translateMatrix*rotTrans*scaleMatrix));
	    glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(1.0f,1.0f,0.0f)));
	    arrow->draw(program,false);

	} else {

		scaleMatrix = glm::scale(glm::mat4(1.f),glm::vec3(scale));		

		rotateMatrix=glm::rotate(glm::mat4(1.f),90.0f,glm::vec3(0.0f,0.0f,sigX*-1.0f));
		rotTrans=glm::translate(glm::mat4(1.f), glm::vec3(sigX*scale,0.0f,0.0));
		glUniformMatrix4fv(mvp, 1, GL_FALSE, glm::value_ptr(VP*translateMatrix*rotTrans*rotateMatrix*scaleMatrix));
	    glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(1.0f,0.0f,0.0f)));
	    arrow->draw(program,false);
		
	    rotateMatrix=glm::rotate(glm::mat4(1.f),0.0f,glm::vec3(0.0f,0.0f,1.0f));
		rotTrans=glm::translate(glm::mat4(1.f), glm::vec3(0.0f,scale,0.0));
		glUniformMatrix4fv(mvp, 1, GL_FALSE, glm::value_ptr(VP*translateMatrix*rotTrans*rotateMatrix*scaleMatrix));
	    glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(0.0f,1.0f,0.0f)));
	    arrow->draw(program,false);

	    rotateMatrix=glm::rotate(glm::mat4(1.f),90.0f,glm::vec3(sigZ*1.0f,0.0f,0.0f));
		rotTrans=glm::translate(glm::mat4(1.f), glm::vec3(0.0f,0.0f,sigZ*scale));
		glUniformMatrix4fv(mvp, 1, GL_FALSE, glm::value_ptr(VP*translateMatrix*rotTrans*rotateMatrix*scaleMatrix));
	    glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(0.0f,0.0f,0.9f)));
	    arrow->draw(program,false);

	}
}

GLuint SphereSelect::getIndex() const {
	return (r)|(g<<8)|(b<<16); 
}

GLuint SphereSelect::getIndexByColor(GLuint _r,GLuint _g,GLuint _b) {
	return (_r)|(_g<<8)|(_b<<16); 
}