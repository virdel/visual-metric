#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include "GL_Mesh.h"
#include<vector>
#include"GL_shader.h"

GL_Mesh::GL_Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures,Material mat, bool  useKd, bool  useKs, bool useKn) {
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	this->mats = mat;
	this->useKn = useKn;
	this->useKd = useKd;
	this->useKs = useKs;
	setupMesh();

}
void GL_Mesh::setupMesh() {


	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &uniformBlockIndex);


	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO); //将创建的缓冲绑定到GL_ARRAY_BUFFER上
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);//将顶点数据复制到缓冲的内存中

	//绑定数据	
	glBindBuffer(GL_UNIFORM_BUFFER, uniformBlockIndex);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(mats), (void*)(&mats), GL_STATIC_DRAW);

	//EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,Normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

	glBindVertexArray(0);
}

void GL_Mesh::Draw(GL_shader shader) {
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	for (int i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		std::string number;
		std::string name= textures[i].type;
		if (name == "texture_diffuse") {
			number = std::to_string(diffuseNr++);
		}
		else if (name == "texture_specular") {
			number = std::to_string(specularNr++);
		}
		shader.setInt(("material." + name + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}
	shader.setBool("material.useKd", useKd);
	shader.setBool("material.useKs", useKs);
	shader.setBool("material.useKn", useKn);
	shader.setVec4("mat.kd", mats.kd);
	shader.setVec4("mat.ks", mats.ks);
	shader.setVec4("mat.ka", mats.ka);



	glBindVertexArray(VAO);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uniformBlockIndex, 0, sizeof(Material));
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

void GL_Mesh::getAABB(glm::vec3& max, glm::vec3& min) {
	max = vertices[0].Position;
	min = vertices[0].Position;

	for (int i = 0; i < vertices.size(); i++) {
		max = glm::max(max, vertices[i].Position);
		min = glm::min(min, vertices[i].Position);
	}
}