#pragma once
#include<glad/glad.h>
#include<string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class GL_shader

{
public:
	unsigned int ID;
	GL_shader(const char* vertexShaderPath, const char* fragmentShaderPath);
	void use();

	//uniform ¹¤¾ßº¯Êý
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec3(const std::string& name, glm::vec3 value) const;
	void setVec3(const std::string& name,float x, float y, float z ) const;
	void setVec4(const std::string& name, glm::vec4 value) const;
	void setVec4(const std::string& name, float x, float y, float z,float d) const;
	void setMat4f(const std::string& name, glm::mat4 value) const;



};

