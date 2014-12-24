#ifndef SCENELOADER_H
#define SCENELOADER_H
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <il.h>
#include <ilu.h>
#include "mesh.h"

class meshLoader{
	std::vector<Mesh*> meshes;
	const char* text_folder;
	void recursiveProcess(aiNode* node,const aiScene* scene);
	void processMesh(aiMesh* Mesh,const aiScene* scene);
	unsigned int loadTexture(const char* filename);
	public:
		meshLoader(const char* filename,const char* _text_folder);
		meshLoader(const char* filename);
		~meshLoader();
		void draw(unsigned int programId, bool wireframe);
		std::vector<Mesh*>& getMeshes();
};

#endif
