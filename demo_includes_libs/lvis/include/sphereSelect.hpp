#include "common.hpp"
#include "sceneLoader.h"

class SphereSelect {
	public:
		SphereSelect(GLuint _index,GLuint _type,meshLoader* _sphere,meshLoader* _arrow,glm::vec3 pos);
		void setType(GLuint _type);
		GLuint getType();
		bool pointInside(glm::vec3 point);
		glm::vec3 getPickColor();
		void translate(GLfloat x,GLfloat y,GLfloat z);
		void translate(glm::vec3 v);
		void scaling(GLfloat _scale);
		void finishScaling();
		void finishTranslate();
		void draw(GLuint program,glm::mat4 VP,bool wireframe);
		void drawAxes(GLuint program,glm::mat4 VP,bool scaling, glm::vec3 camPos);
		GLuint getIndex() const;
		static GLuint getIndexByColor(GLuint r,GLuint g,GLuint b);
	private:
		GLint r,g,b;
		GLfloat scale,tempScale;
		GLuint type,index;
		glm::vec3 transVector,tempTrans;
		meshLoader* sphere,*arrow;

		template <typename T> inline GLint sgn(T val) {
    		return (T(0) < val) - (val < T(0));
		}
};