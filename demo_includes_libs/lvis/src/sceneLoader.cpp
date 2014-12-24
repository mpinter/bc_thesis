#include "../include/sceneLoader.h"
	
void meshLoader::recursiveProcess(aiNode* node,const aiScene* scene)
{
	//process
	for(int i=0;i<node->mNumMeshes;i++)
	{
		aiMesh* mesh=scene->mMeshes[node->mMeshes[i]];
		processMesh(mesh,scene);
	}
	
	//recursion
	for(int i=0;i<node->mNumChildren;i++)
	{
		recursiveProcess(node->mChildren[i],scene);
	}
}

void meshLoader::processMesh(aiMesh* mesh,const aiScene* scene)
{
	std::vector<vertexData> data;
	std::vector<unsigned int> indices;
	std::vector<textureData> textures; 
	aiColor4D col;
	aiMaterial* mat=scene->mMaterials[mesh->mMaterialIndex];
	aiGetMaterialColor(mat,AI_MATKEY_COLOR_DIFFUSE,&col);
	glm::vec3 defaultColor(col.r,col.g,col.b);
	
		
	for(int i=0;i<mesh->mNumVertices;i++)
	{
			vertexData tmp;
			glm::vec3 tmpVec;
			
			//position
			tmpVec.x=mesh->mVertices[i].x;
			tmpVec.y=mesh->mVertices[i].y;
			tmpVec.z=mesh->mVertices[i].z;
			tmp.position=tmpVec;
			
			//normals
			tmpVec.x=mesh->mNormals[i].x;
			tmpVec.y=mesh->mNormals[i].y;
			tmpVec.z=mesh->mNormals[i].z;
			tmp.normal=tmpVec;			
			
			
			//tangent
			if(mesh->mTangents)
			{
				tmpVec.x=mesh->mTangents[i].x;
				tmpVec.y=mesh->mTangents[i].y;
				tmpVec.z=mesh->mTangents[i].z;
			}else{
				tmpVec.x=1.0;
				tmpVec.y=tmpVec.z=0;
			}
			tmp.tangent=tmpVec;		
			
			
			//colors
			if(mesh->mColors[0])
			{
				//!= material color
				tmpVec.x=mesh->mColors[0][i].r;
				tmpVec.y=mesh->mColors[0][i].g;
				tmpVec.z=mesh->mColors[0][i].b;				
			}else{
				tmpVec=defaultColor;
			}
			tmp.color=tmpVec;
			
			//color
			if(mesh->mTextureCoords[0])
			{
				tmpVec.x=mesh->mTextureCoords[0][i].x;
				tmpVec.y=mesh->mTextureCoords[0][i].y;				
			}else{
				tmpVec.x=tmpVec.y=tmpVec.z=0.0;
			}
			tmp.U=tmpVec.x;
			tmp.V=tmpVec.y;
			data.push_back(tmp);
	}
	
	for(int i=0;i<mesh->mNumFaces;i++)
	{
		aiFace face=mesh->mFaces[i];
		for(int j=0;j<face.mNumIndices;j++) //0..2
		{
			indices.push_back(face.mIndices[j]);
		}
	}
	
	
	for(int i=0;i<mat->GetTextureCount(aiTextureType_DIFFUSE);i++)
	{
		aiString str;
		mat->GetTexture(aiTextureType_DIFFUSE,i,&str);
		textureData tmp;
		tmp.id=loadTexture(str.C_Str());
		tmp.type=0;
		textures.push_back(tmp);
	}
	meshes.push_back(new Mesh(&data,&indices,&textures));	
}

unsigned int meshLoader::loadTexture(const char* filename) {
	unsigned int num;
	glGenTextures(1,&num);
	ilInit();
	ILuint texIL;
    ilGenImages(1, &texIL);
    ilBindImage(texIL);
    char buffer[120];
    strcpy(buffer,text_folder);
    strcat(buffer,filename);
    bool result = ilLoadImage(buffer);
	if(!result) {
		ILenum devilError = ilGetError();
		std::cout << "Error loading texture " << buffer << std::endl;
		std::cout << "Devil Error: " << devilError << std::endl << std::flush;
		exit(1);
	}
	glBindTexture(GL_TEXTURE_2D,num);		
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 
                 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
    ilDeleteImages(1, &texIL);	
	return num;	
}


meshLoader::meshLoader(const char* filename) : meshLoader(filename,"") { //this is used only for flat-colored objects - we don't need light position
}

meshLoader::meshLoader(const char* filename,const char* _text_folder) : text_folder(_text_folder) {
	std::cout << "Mesh loader initializing - file: " << filename << " with textures in folder (optional):" << _text_folder << std::endl << std::flush;
	Assimp::Importer importer;
	const aiScene* scene=importer.ReadFile(filename, aiProcess_GenSmoothNormals | aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);
	if ((scene==NULL)||(scene->mFlags==AI_SCENE_FLAGS_INCOMPLETE || !(scene->mRootNode))) {
		std::cout << "Assimp loading of file " << filename << " failed with following error(s):"  << std::endl << std::flush;
		std::cout << importer.GetErrorString() << std::endl << std::flush;
		exit(1); 	
	}
	recursiveProcess(scene->mRootNode,scene);
	std::cout << "Done" << std::endl << std::flush;
}

meshLoader::~meshLoader()
{
	for(int i=0;i<meshes.size();i++)
		delete meshes[i];
}

void meshLoader::draw(unsigned int programId,bool wireframe)
{
	for(int i=0;i<meshes.size();i++)
		meshes[i]->draw(programId,wireframe);	
}

std::vector<Mesh*>& meshLoader::getMeshes()
{
	return meshes;
}
