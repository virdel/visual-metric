#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D screenTexture1;
uniform sampler2D screenTexture2;
uniform sampler2D depth1;
uniform sampler2D depth2;

void main()
{ 
        //默认模式
    vec4 color3 = texture(screenTexture1, TexCoords);
    vec4 color4= texture(screenTexture2, TexCoords);

    vec4 depthvalue1= texture(depth1, TexCoords);
    vec4 depthvalue2= texture(depth2, TexCoords);
    
    FragColor=color4;

}