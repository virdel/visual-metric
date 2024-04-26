

#include "stb/stb_image.h"

#include "GL_Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include<iostream>

#include"glm/glm.hpp"
#include"glm/gtc/matrix_transform.hpp"
#include"glm/gtc/type_ptr.hpp"
	
void GL_Model::Draw(GL_shader& shader) {
	for (int i = 0; i < meshes.size(); i++) {
		meshes[i].Draw(shader);
	}
} 

void GL_Model::loadModel(std::string path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs| aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode, scene);
}

void GL_Model::processNode(aiNode* node, const aiScene* scene) {
	for (int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	for (int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

GL_Mesh GL_Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int>indices;
	std::vector<Texture> textures;
	Material mat;
	bool useKn = false, useKd = false, useKs = false;

	for (int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		if (mesh->HasNormals()) {
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}
		if (mesh->HasTangentsAndBitangents()) {
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.tangent =glm::normalize( vector);

			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.bitangent = glm::normalize(vector);
		}



		if (mesh->mTextureCoords[0]) {
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else {
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}

		//std::cout << "vertex normal" << vertex.Normal.x << " " << vertex.Normal.y << " " << vertex.Normal.z << " " << "vertex tangent" << vertex.tangent.x << " " << vertex.tangent.y << " " << vertex.tangent.z << " " << "vertex bitangent" << vertex.bitangent.x << " " << vertex.bitangent.y << " " << vertex.bitangent.z << "N tangent dot "<<glm::dot(vertex.Normal,vertex.tangent)<< "N bitangent dot "<< glm::dot(vertex.Normal, vertex.bitangent)<<"      " <<glm::normalize(glm::cross(vertex.tangent, vertex.bitangent)).x<<" "<< glm::normalize(glm::cross(vertex.tangent, vertex.bitangent)).y<<" "<< glm::normalize(glm::cross(vertex.tangent, vertex.bitangent)).z << std::endl;
		vertices.emplace_back(vertex);
	}
	for (int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}
	
	if (mesh->mMaterialIndex >= 0) {
		aiString str;
		
		
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		material->Get(AI_MATKEY_NAME, str);
		std::cout << str.C_Str() << std::endl;
		aiColor3D color;

		if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_AMBIENT, color)) {
			mat.ka = glm::vec4(color.r, color.g, color.b, 1.0);
		}
		else {
			mat.ka = glm::vec4(1.0);
		}
		if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
			mat.kd = glm::vec4(color.r, color.g, color.b, 1.0);
		}
		else {
			mat.kd = glm::vec4(1.0);
		}
		if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_SPECULAR, color)) {
			mat.ks = glm::vec4(color.r, color.g, color.b, 1.0);
		}
		else {
			mat.ks = glm::vec4(1.0);
		}

		float shininess;
		if (AI_SUCCESS == material->Get(AI_MATKEY_SHININESS, shininess)) {
			mat.shininess = shininess;
		}
		else {
			mat.shininess = 32;
		}


		//��������ͼ
		if (material->GetTextureCount(aiTextureType_DIFFUSE)>0) {
			useKd = true;
			std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse",useKd);
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		}
		//specular��ͼ
		if (material->GetTextureCount(aiTextureType_SPECULAR)> 0) {
			useKs = true;
			std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular",useKs);
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		//������ͼ
		if (material->GetTextureCount(aiTextureType_HEIGHT)> 0) {
			useKn = true;
			std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal",useKn);
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		}

	}
	return GL_Mesh(vertices, indices, textures,mat,useKd,useKs,useKn);

}

std::vector<Texture>  GL_Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName,bool & use) {
	std::vector<Texture> textures;
	std::cout << mat->GetTextureCount(type) << std::endl;
	for (int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);

		std::cout << str.C_Str() << std::endl;
		bool skip = false;
		for (int j = 0; j < textures_loaded.size(); j++) {
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str())==0) {
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip) {
			Texture texture;
			std::cout << directory<<std::endl;
			texture.id = TextureFromFile(str.C_Str(), directory,use);
			texture.type = typeName;
			texture.path = str.C_Str();
			std::cout << texture.path << std::endl;
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}

	}
	return textures;
}

unsigned int TextureFromFile(const char* path, const std::string& directory,bool & use, bool gamma) {
	std::string filename = std::string(path);
	filename = directory + "/" + filename;

	//������������
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;

	//��תy���ȡͼ������
	stbi_set_flip_vertically_on_load(false);
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

	if (data) {
		GLenum format=GL_RED;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;
		//��������Ԫ
		glBindTexture(GL_TEXTURE_2D, textureID);
		//�����Դ棬��������
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
		use = true;
	}
	else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
		use = false;
	}
	return textureID;
}

void  GL_Model::getAABB(glm::vec3& max, glm::vec3& min) {
	meshes[0].getAABB(max, min);
	for (int i = 0; i < meshes.size(); i++) {
		glm::vec3 mesh_max, mesh_min;
		meshes[i].getAABB(mesh_max, mesh_min);
		max=glm::max(max, mesh_max);
		min = glm::min(min, mesh_min);
	}
	auto add=max+min;
	center=glm::vec3((max+min))/2.0f;
	diagnal_length = glm::length(max - min);
}

float GL_Model::getDiagnalLength() {
	return diagnal_length;
}

//��������ͼ
unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;

	stbi_set_flip_vertically_on_load(false);
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}
	
skybox::skybox(GL_shader shader,GL_camera& camera,const unsigned int width, const unsigned int height,float s) : shader(shader),camera(camera), SCR_WIDTH(width), SCR_HEIGHT(height),scale(s){

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	std::vector<std::string> faces = {
	"E:/code/learn_opengl/learn_opengl/resources/skybox/right.jpg",
	"E:/code/learn_opengl/learn_opengl/resources/skybox/left.jpg",
	"E:/code/learn_opengl/learn_opengl/resources/skybox/top.jpg",
	"E:/code/learn_opengl/learn_opengl/resources/skybox/bottom.jpg",
	"E:/code/learn_opengl/learn_opengl/resources/skybox/front.jpg",
	"E:/code/learn_opengl/learn_opengl/resources/skybox/back.jpg",

	};
	unsigned int cubemapTexture = loadCubemap(faces);

	VAO = skyboxVAO;
	texture = cubemapTexture;

	
}

void skybox::draw() {
	
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content

	shader.use();
	glm::mat4 model = glm::mat4(1.0f);
	model=glm::scale(model, glm::vec3(scale, scale,scale));

	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f,(float)(scale*3.0f));
	shader.setMat4f("model", model);
	shader.setMat4f("view", view);
	shader.setMat4f("projection", projection);
	// skybox cube
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP,texture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default


}