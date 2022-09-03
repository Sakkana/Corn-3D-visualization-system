#version 330 core

struct Material {
//    vec3 ambient;
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};
uniform Material material;

struct Light {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;


out vec4 FragColor;
in vec2 TexCoods;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    //环境光
//    float ambientStrength = 0.1;
    vec3  diffuseTexColor =  vec3(texture( material.texture_diffuse1, TexCoods));
    vec3  specularTexColor =  vec3(texture( material.texture_specular1, TexCoods));

    vec3 ambient = diffuseTexColor*light.ambient;
    // diffuse
    //单位法线向量
    vec3 norm = normalize(Normal);
    //指向光源的单位向量
    //而light.direction是指向像素点的，所以加个符号反向
    vec3 lightDir = normalize(-light.direction);
    //diff只是比例,如果小于0就取0，所以取了个max
    float diff = max(dot(norm, lightDir), 0.0);
    //还要用diff乘以光的颜色
    vec3 diffuse = diff * diffuseTexColor * light.diffuse;

    // specular
//    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular =  spec * light.specular * specularTexColor;

//    vec3 result = (ambient + diffuse + specular) * objectColor;
    //我们不再直接乘以objectColor，而是分项分别相乘
    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0);

}
