#version 330

in vec3 frag_pos_vs;

uniform samplerCube environment_map;

out vec4 result_fs;

void main() {
    vec3 color = texture(environment_map, frag_pos_vs.xyz).rgb;

    // Gamma correction
    //color = color / (color + vec3(1.0));
    //color = pow(color, vec3(1.0/2.2));

    result_fs = vec4(color, 1.0);
}
