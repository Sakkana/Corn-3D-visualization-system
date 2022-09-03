#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTextCoords;


out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoods;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {

    TexCoods = aTextCoords;
    //    Normal = vec3( model * vec4(aNormal, 1.0));
    //这里使用了法线矩阵
    Normal = mat3(transpose(inverse(model))) * aNormal;

    FragPos = vec3( model * vec4(aPos, 1.0));
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
