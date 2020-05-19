#version 330

in vec3 frag_pos_vs;

uniform sampler2D equirectagular_map;

out vec4 result_fs;

vec2 sample_spherical_map(vec3 v) {
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= vec2(0.159, 0.3183);
    uv += 0.5;
    return uv;
}

void main() {
    vec2 uv = sample_spherical_map(normalize(frag_pos_vs));
    vec3 color = texture(equirectagular_map, uv).rgb;
    result_fs = vec4(color, 1.0);
}
