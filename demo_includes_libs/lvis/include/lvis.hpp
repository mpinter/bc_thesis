#include "common.hpp"
#include "sceneLoader.h"
#include "camera.hpp"
#include "lightPath.hpp"
#include "sphereSelect.hpp"
#include "virtCam.hpp"

class Lvis {
	public:
		Lvis(char* _scene_file,char* _scene_textures) : scene_file(_scene_file), scene_textures_folder(_scene_textures) {};
		void run();
		void finish();
		void pushSinglePath(LightPath* _path);
		void pushPath(LightPath* _path);
		void clearSingle();
		void clearDrawnPaths();
		void clearAllSpheres();
		void setLimit(GLuint _limit);
		void setDrawLimit(GLuint _limit);
		void setThreshold(GLuint _thresh);
		void join(); //thread
		void wait();
		LightPath* getSelected();
		void setupVirtualCamera(glm::vec3 from, glm::vec3 to, glm::vec3 up);
		void setupLight(glm::vec3 lpos);
		bool isRunning();

		//flags
		bool flagSingle=false,flagSelected=false,flagPause=false;
	private:

		void testInit();
		void threadFunc();
		void initShaders(GLuint &program,const char* vsf,const char* fsf);
		void init();
		void draw();
		void handleEvents(sf::Window* win);
		bool filterPath(LightPath* path);
		GLint pickAxes(GLint x,GLint y);
		void pickSpheres(GLint x,GLint y);
		void pickPaths(GLint x,GLint y);
		void counterUp();
		void processPaths();
		void filterDrawn();
		void selectPath();

		const char* basic_vertex_shader_file = "./shaders/vertex.vs";
		const char* basic_fragment_shader_file = "./shaders/fragment.frag";
		const char* notext_fragment_shader_file = "./shaders/fragment_notext.frag";
		const char* textonly_fragment_shader_file = "./shaders/fragment_textonly.frag";
		const char* line_vertex_shader_file = "./shaders/line.vert";
		const char* line_fragment_shader_file = "./shaders/line.frag";
		const char* scene_file;
		const char* scene_textures_folder;
		const char* sphere_file="./models/icosphere.obj";
		const char* arrow_file="./models/arrow.obj";
		const char* camera_file="./models/camera/camera.obj";
		const char* camera_textures_folder="./models/camera/tex/";


		std::thread lvisThread;
		std::mutex mut;
		std::condition_variable cv;

		meshLoader* scene, *sphere, *arrow, *camModel;
		Camera* cam;
		SphereSelect* activeSphere=NULL;
		VirtualCamera* virtCam=NULL;

		glm::vec3 artificalLight;

		GLuint basicProgram,textOnlyProgram,noTextProgram,lineProgram,basicVao,lineVao,vertexBuffer,sphereCoordsVBO,sphereIndicesVBO,counter=1,limit=50,drawLimit=50,threshold=20,shaderType=0;
		GLint positionAttrib,colorAttrib,uniBMVP,uniLMVP,uniLcolor,uniNM;
		GLint mouseDownX,mouseDownY,window_w=800,window_h=600,dragModX=1,dragModY=1;
		bool mouseRotating=false,dragX=false,dragY=false,dragZ=false,countOverflow=false,scaleSwitch=false,scaleXYZ=false,showCamera=false,focus=true;
		bool flagRunning=false,flagWaiting=false;

		GLfloat clickDepth;
		glm::vec3 clickPos;

		struct compareSelect {
    		bool operator()(const SphereSelect* pX, const SphereSelect* pY) const {
        		return pX->getIndex() < pY->getIndex();
    		}
		};

		std::vector<LightPath*> inputPaths;
		std::map<GLuint,LightPath*> drawnPaths;
		LightPath* singlePath=NULL,*previousPath=NULL,*activePath=NULL,*selectedPath=NULL;
		std::map<GLuint,SphereSelect*> allSpheres;

		sf::Window* window;
		sf::ContextSettings* settings;

		template <typename T> inline GLint sgn(T val) {
    		return (T(0) < val) - (val < T(0));
		}
};