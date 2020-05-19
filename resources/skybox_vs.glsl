#version 330

in vec3 Vertex;

uniform mat4 projection;
uniform mat4 view;

out vec3 frag_pos_vs;

void main() {
    frag_pos_vs = Vertex;

    mat4 rot_view = mat4(mat3(view));
    vec4 clip_pos = projection * rot_view * vec4(Vertex, 1.0);

    gl_Position = clip_pos.xyzw;
}
