#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec4 Ambient;
in vec4 Diffuse;
in vec4 Specular;
in mat3 TBN;
//���߿ռ�

out vec4 FragColor;

struct Material {
	bool useKd;
	bool useKs;
	bool useKn;
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_normal1;
    float shininess;
}; 

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform vec3 viewPos;
uniform Light light;  

void main(){
	
	//������
	vec3 ambient,diffuse,specular;


	//��������
	if(material.useKd){
		ambient=texture(material.texture_diffuse1,TexCoord).xyz*light.ambient;
	}else{
		ambient=vec3(Ambient)*light.ambient;
	}
	//diffuse
	vec3 norm=normalize(Normal);
	if (gl_FrontFacing == false) {
    // 当前片段位于多边形背面
    	norm = -norm; // 反转法线方向（假设已经计算出正面的法线）
	}

	if(false){
		norm=vec3(texture(material.texture_normal1,TexCoord));
		norm=norm*2.0-1.0;
		norm=normalize(TBN*norm);
	}

	vec3 lightdir=normalize(light.position-FragPos);
	float diff=max(dot(norm,lightdir),0.0);


	if(material.useKd){
		diffuse=diff*vec3(texture(material.texture_diffuse1,TexCoord))*light.diffuse;
	}else{
		diffuse=diff*vec3(Diffuse)*light.diffuse;
	}
	//specular
	vec3 viewDir=normalize(viewPos-FragPos);
	vec3 halfwaydir=normalize(lightdir+viewDir);
	//shininess Ĭ��32
	float spec= pow(max(dot(norm,halfwaydir),0.0),32);
	if(material.useKs){
		specular=(spec*vec3(texture(material.texture_specular1,TexCoord)))*light.specular;
	}else{
		specular=spec*vec3(Specular)*light.specular;
	}

	vec3 result=ambient+diffuse+specular;
	FragColor =vec4(diffuse,1.0);

}