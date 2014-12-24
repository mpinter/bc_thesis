#include "../include/lightPath.hpp"

LightPath::LightPath () {
	vboExists=false;
	strength=5.0f;
	vboLength=0;
	vbo=0;
}

LightPath::~LightPath() {
	for (int i=0;i<vertices.size();i++) {
		delete vertices[i];
	}
}

void LightPath::pushVertex(PathVertex* _v) {
	vertices.push_back(_v);
	vboExists=false;
}

void LightPath::clear() {
	for (int i=0;i<vertices.size();i++) {
		delete vertices[i];
	}
	vertices.clear();
}

void LightPath::setStrength(GLfloat _s) {
	strength=_s;
}

void LightPath::finalize() {
	if (vboExists) return;
	//generate vertex array
	GLfloat* arr=new GLfloat[vertices.size()*3];
	for (int i=0;i<vertices.size();i++) {
		glm::vec3 tmpVec=vertices[i]->getPos();
		arr[i*3]=tmpVec.x;
		arr[i*3+1]=tmpVec.y;
		arr[i*3+2]=tmpVec.z;
	}
	if (vbo!=0) glDeleteBuffers(1,&vbo);
	glGenBuffers(1,&vbo);
	glBindBuffer(GL_ARRAY_BUFFER,vbo);
	glBufferData(GL_ARRAY_BUFFER,vertices.size()*3*sizeof(GLfloat),&arr[0],GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	vboExists=true;
	vboLength=vertices.size();
	info.clear();
	info << std::endl;
	info << "Light path generated info log:\n";
	info << "Length: " << vertices.size() << "\n";
	for (int i=0;i<vertices.size();i++) {
		info << vertices[i]->getInfo();
	}
	
    delete [] arr;
}

void LightPath::draw(GLuint program,glm::mat4 VP,bool colorless,bool selected) {
	if (!vboExists) {
		std::cout << "Error! Drawing of a lightpath was skipped, because VBO for the path wasn't created. (Did you forget to call finalize() after pushing a vertex?)\n";
		return;
	}

	GLint positionAttrib = glGetAttribLocation(program,"vertex");
	GLint uniColor= glGetUniformLocation(program, "uniColor");
	GLint mvp =glGetUniformLocation(program, "uniMVP");

    glLineWidth(strength);
	glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glVertexAttribPointer(positionAttrib,3,GL_FLOAT,GL_FALSE,0,0);
    glEnableVertexAttribArray(positionAttrib);
    glUniformMatrix4fv(mvp, 1, GL_FALSE, glm::value_ptr(VP));
    if (colorless) {
    	if (selected) {
    		glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(1.0f,1.0f,0.7f)));
    	} else {
	    	glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(1.0f,1.0f,1.0f)));
    	}
    } else {
    	glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(getPickColor())));
    }
    glDrawArrays(GL_LINE_STRIP,0,vboLength);
    glDisableVertexAttribArray(positionAttrib);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glLineWidth(1.0f);

}

void LightPath::drawDiff(LightPath* previous,GLuint program,glm::mat4 VP) {
	if (!vboExists) {
		std::cout << "Error! Drawing of a lightpath was skipped, because VBO for the path wasn't created. (Did you forget to call finalize() after pushing a vertex?)\n";
		return;
	}
	GLint positionAttrib = glGetAttribLocation(program,"vertex");
	GLint uniColor= glGetUniformLocation(program, "uniColor");
	GLint mvp =glGetUniformLocation(program, "uniMVP");
	glLineWidth(strength);
	glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glVertexAttribPointer(positionAttrib,3,GL_FLOAT,GL_FALSE,0,0);
    glEnableVertexAttribArray(positionAttrib);
    glUniformMatrix4fv(mvp, 1, GL_FALSE, glm::value_ptr(VP));
	if (previous==NULL) {
	    glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(1.0f,1.0f,0.0f)));
	    glDrawArrays(GL_LINE_STRIP,0,vboLength);
	} else {
		int i=0;
		while (((i<vertices.size())&&(i<previous->getVertices()->size()))&&(vertices[i]->getPos()==previous->getVertices()->at(i)->getPos())) i++;
		glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(0.0f,1.0f,0.0f)));
	    if (i>0) glDrawArrays(GL_LINE_STRIP,0,i);
	    if (i==vertices.size()) {
	    	glDisableVertexAttribArray(positionAttrib);
		    glBindBuffer(GL_ARRAY_BUFFER,0);
		    glLineWidth(1.0f);
		    return;
	    }
	    int j=i-1; //the "changed" part should be closed interval
	    if (j<0) j=0;
	    while (((i<vertices.size())&&(i<previous->getVertices()->size()))&&(vertices[i]->getPos()!=previous->getVertices()->at(i)->getPos())) i++;
	    glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(1.0f,1.0f,0.0f)));
	    glDrawArrays(GL_LINE_STRIP,j,i-j+1);
	    j=i;
	    if (i==vertices.size()) {
	    	glDisableVertexAttribArray(positionAttrib);
		    glBindBuffer(GL_ARRAY_BUFFER,0);
		    glLineWidth(1.0f);
		    return;
	    }
	    while (((i<vertices.size())&&(i<previous->getVertices()->size()))&&(vertices[i]->getPos()==previous->getVertices()->at(i)->getPos())) i++;
	    glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(0.0f,1.0f,0.0f)));
	    glDrawArrays(GL_LINE_STRIP,j,i-j);
	}
    glDisableVertexAttribArray(positionAttrib);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glLineWidth(1.0f);
}

std::string LightPath::getInfo() {
	return info.str();
}

std::vector<PathVertex*>* LightPath::getVertices() {
	return &vertices;
}

glm::vec3 LightPath::getPickColor() {
	return glm::vec3(GLfloat(r)/255.0f, GLfloat(g)/255.0f, GLfloat(b)/255.0f);
}

void LightPath::setIndex(GLuint index) {
	r = index&0xFF;
    g = (index>>8)&0xFF;
	b = (index>>16)&0xFF;
}

GLuint LightPath::getIndex() const {
	return (r)|(g<<8)|(b<<16); 
}

GLuint LightPath::getIndexByColor(GLuint _r,GLuint _g,GLuint _b) {
	return (_r)|(_g<<8)|(_b<<16); 
}