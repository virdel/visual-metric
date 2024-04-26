#pragma once
#include<vector>
#include "GL_Mesh.h"
#include<string>
#include"GL_shader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include"GL_camera.h"


class GL_Model
{
public:
	std::vector<GL_Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;
	float diagnal_length;
	glm::vec3 center;
	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	GL_Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typename,bool & use);
public:
	GL_Model(const char* path) {
		loadModel(path);
		glm::vec3 max;
		glm::vec3 min;
		getAABB(max,min);
	}
	void Draw(GL_shader& shader);
	
	void getAABB(glm::vec3& max, glm::vec3& min);

	float getDiagnalLength();
};

class skybox {
public:
	float scale;

	GL_shader shader;
	GL_camera& camera;
	unsigned int VAO, texture;
	const unsigned int SCR_WIDTH, SCR_HEIGHT;
	skybox(GL_shader shader, GL_camera& camera, const unsigned int width, const unsigned int height,float s);
	void draw();

	void setScale(float s) {
		scale = s;
	}

};

unsigned int TextureFromFile(const char* path, const std::string& directory,bool & use, bool gamma=false);
