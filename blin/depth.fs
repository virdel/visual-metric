#version 330 core
//���߿ռ�
out vec4 FragColor;
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

in mat3 TBN;
//���߿ռ�

void main(){

	float normalized_depth = (gl_FragCoord.z + 1.0) / 2.0; // 转换到 [0.0, 1.0]
	//���depthֵ
	FragColor=vec4(vec3(0.0), 1.0);
}