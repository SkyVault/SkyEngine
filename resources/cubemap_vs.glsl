#version 330

in vec3 Vertex;

uniform mat4 projection;
uniform mat4 view;

out vec3 frag_pos_vs;

void main() {
    frag_pos_vs = Vertex;

    gl_Position = projection * view * vec4(Vertex, 1.0);
}
