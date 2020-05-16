#version 330

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

out vec4 result;

struct Material {
    sampler2D sample;
    vec4 diffuse; 
    float shininess;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 diffuse;
    vec3 specular;
};

#define MAX_NUMBER_OF_POINT_LIGHTS (4)

uniform PointLight point_lights[MAX_NUMBER_OF_POINT_LIGHTS]; 
uniform vec3 view_pos; //done
uniform int num_point_lights = MAX_NUMBER_OF_POINT_LIGHTS;
uniform Material material;

vec3 calc_point_light(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_dir){
    vec3 light_dir = normalize(light.position - frag_pos); 
    float diff = max(dot(normal, light_dir), 0.0);
    vec3 reflect_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);

    float dist = length(light.position - frag_pos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

    vec3 ambient = vec3(0.2, 0.2, 0.2) * vec3(texture(material.sample, fragTexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.sample, fragTexCoord));
    vec3 specular = light.specular * spec;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

void main(){
    vec3 norm = normalize(fragNormal);
    vec3 view_dir = normalize(view_pos - fragPosition);

    vec3 color = vec3(0, 0, 0);

    for (int i = 0; i < num_point_lights; i++)
        color += calc_point_light(point_lights[i], norm, fragPosition, view_dir);

    result = vec4(color , 1);
}