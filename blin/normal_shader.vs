//另外一种法线贴图映射方法，将TBN矩阵传给fs,将法向贴图的法线映射到模型空间

#version 330 core
layout (location =0) in vec3 aPos;
layout (location=1 ) in vec3 aNor;
layout (location=2 ) in vec2 Tex;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;  
uniform mat4 model;
uniform mat4 view;
uniform vec3 viewPos;
uniform mat4 projection;
uniform bool blin;
uniform Mat{
	vec4 aAmbient;
	vec4 aDiffuse;
	vec4 aSpecular;
};


out vec3 FragPos;
out vec2 TexCoord;
out vec4 Ambient;
out vec4 Diffuse;
out vec4 Specular;
out vec3 Normal;
out mat3 TBN;

void main(){
	gl_Position =projection* view*model*vec4(aPos, 1.0);

	//纹理坐标
	TexCoord=Tex;
	Ambient=aAmbient;
	Diffuse=aDiffuse;
	Specular=aSpecular;
	Normal=vec3(model*vec4(aNor,0.0));
	vec3 T=normalize(vec3(model*vec4(tangent,0.0)));
	vec3 B=normalize(vec3(model*vec4(bitangent,0.0)));
	vec3 N=normalize(vec3(model*vec4(aNor,0.0)));
	TBN=mat3(T,B,N);
	FragPos=vec3(model*vec4(aPos.x, aPos.y, aPos.z, 1.0));
}
