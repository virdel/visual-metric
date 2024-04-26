#version 330 core
layout (location =0) in vec3 aPos;
layout (location=1 ) in vec3 aNor;
layout (location=2 ) in vec2 Tex;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

uniform mat4 model;
uniform mat4 view;
uniform vec3 viewPos;
uniform mat4 projection;

out vec3 FragPos;
out vec2 TexCoord;

out vec3 Normal;
out mat3 TBN;

void main(){
	gl_Position =projection* view*model*vec4(aPos, 1.0);

	//纹理坐标
	TexCoord=Tex;

	Normal=vec3(model*vec4(aNor,0.0));
	vec3 T=normalize(vec3(model*vec4(tangent,0.0)));
	vec3 B=normalize(vec3(model*vec4(bitangent,0.0)));
	vec3 N=normalize(vec3(model*vec4(aNor,0.0)));
	TBN=mat3(T,B,N);
	FragPos=vec3(model*vec4(aPos.x, aPos.y, aPos.z, 1.0));
}
