#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;
uniform mat4 matModel;
uniform mat4 matView;

// Output vertex attributes (to fragment shader)
out vec3 fragPosition;
out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragNormal;

out float fragAffine;

vec4 snap(vec4 vertex, vec2 resolution){
    vec4 result = vertex;
    result.xyz = vertex.xyz / vertex.w;
    result.xy = floor(resolution * result.xy) / resolution;
    result.xyz *= vertex.w;
    return result;
}

void main() {
    // Send vertex attributes to fragment shader
    fragPosition = vec3(matModel*vec4(vertexPosition, 1.0f));

    fragColor = vertexColor;
    
    mat3 normalMatrix = transpose(inverse(mat3(matModel)));
    fragNormal = normalize(normalMatrix*vertexNormal);

    // Calculate uv position simulating affine texture mapping
    gl_Position = snap(mvp*vec4(vertexPosition, 1.0), vec2(320.0/2.0, 224.0/2.0));
    // fragTexCoord = vertexTexCoord; 

    vec3 vertex_mv = vec3(matView * vec4(fragPosition, 1.0));
    float dist = length(vertex_mv) * 2.0;
    float affine = dist + ((gl_Position.w * 8.0)/dist)*0.5;

    fragTexCoord = vertexTexCoord * affine;
    fragAffine = affine; 
}
