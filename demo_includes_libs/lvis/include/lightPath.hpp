#include "common.hpp"
#include "pathVertex.hpp"

class LightPath {
	public:
		LightPath();
		~LightPath();
		void setStrength(GLfloat _s);
		void finalize();
		void pushVertex(PathVertex* _v);
		void clear();
		void draw(GLuint program,glm::mat4 VP,bool colorless,bool selected);
		void drawDiff(LightPath* previous,GLuint program,glm::mat4 VP);
		std::string getInfo();
		std::vector<PathVertex*>* getVertices();
		void setIndex(GLuint index);
		GLuint getIndex() const;
		glm::vec3 getPickColor();
		static GLuint getIndexByColor(GLuint r,GLuint g,GLuint b);
	private:
		std::vector<PathVertex*> vertices;
		bool vboExists;
		GLfloat strength;
		GLuint vbo,vboLength,r,g,b;
		std::stringstream info;
};