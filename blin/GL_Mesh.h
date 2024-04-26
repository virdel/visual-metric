#pragma once
#include"GL_shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include<vector>
#include<string>


struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;

};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

struct Material {
    glm::vec4 ka;
    glm::vec4 kd;
    glm::vec4 ks;
    float shininess;
};

class GL_Mesh
{
private:
    unsigned int VAO, VBO, EBO;
    bool useKn, useKd, useKs;
    float shininess;
    void setupMesh();

public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int > indices;
    std::vector<Texture> textures;
    Material mats;
    unsigned int uniformBlockIndex;

    GL_Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures,Material mat,bool  useKa,bool  useKd,bool useKs);
    void Draw(GL_shader shader);

    void getAABB(glm::vec3& max, glm::vec3& min);


};

