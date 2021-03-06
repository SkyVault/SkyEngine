#include "rlights.h"

Light CreateLight(int type, Vector3 position, Vector3 target, Color color,
                  Shader shader) {
    Light light = {0};

    if (lightsCount < MAX_LIGHTS) {
        light.enabled = true;
        light.type = type;
        light.position = position;
        light.target = target;
        light.color = color;

        // TODO: Below code doesn't look good to me,
        // it assumes a specific shader naming and structure
        // Probably this implementation could be improved
        char enabledName[32] = "lights[x].enabled\0";
        char typeName[32] = "lights[x].type\0";
        char posName[32] = "lights[x].position\0";
        char targetName[32] = "lights[x].target\0";
        char colorName[32] = "lights[x].color\0";
        enabledName[7] = '0' + lightsCount;
        typeName[7] = '0' + lightsCount;
        posName[7] = '0' + lightsCount;
        targetName[7] = '0' + lightsCount;
        colorName[7] = '0' + lightsCount;

        light.enabledLoc = GetShaderLocation(shader, enabledName);
        light.typeLoc = GetShaderLocation(shader, typeName);
        light.posLoc = GetShaderLocation(shader, posName);
        light.targetLoc = GetShaderLocation(shader, targetName);
        light.colorLoc = GetShaderLocation(shader, colorName);

        UpdateLightValues(shader, light);

        lightsCount++;
    }

    return light;
}

// Send light properties to shader
// NOTE: Light shader locations should be available
void UpdateLightValues(Shader shader, Light light) {
    // Send to shader light enabled state and type
    SetShaderValue(shader, light.enabledLoc, &light.enabled, UNIFORM_INT);
    SetShaderValue(shader, light.typeLoc, &light.type, UNIFORM_INT);

    // Send to shader light position values
    float position[3] = {light.position.x, light.position.y, light.position.z};
    SetShaderValue(shader, light.posLoc, position, UNIFORM_VEC3);

    // Send to shader light target position values
    float target[3] = {light.target.x, light.target.y, light.target.z};
    SetShaderValue(shader, light.targetLoc, target, UNIFORM_VEC3);

    // Send to shader light color values
    float color[4] = {
        (float)light.color.r / (float)255, (float)light.color.g / (float)255,
        (float)light.color.b / (float)255, (float)light.color.a / (float)255};
    SetShaderValue(shader, light.colorLoc, color, UNIFORM_VEC4);
}

Sun CreateSun(Shader shader, Vector3 direction, Color ambient, Color diffuse) {
    Sun result = (Sun){
        .direction = direction,
        .ambient = ambient,
        .diffuse = diffuse,

        .directionLoc = GetShaderLocation(shader, "sun.direction"),
        .ambientLoc = GetShaderLocation(shader, "sun.ambient"),
        .diffuseLoc = GetShaderLocation(shader, "sun.diffuse"),
    };

    UpdateSunValue(shader, result);

    return result;
}

void UpdateSunValue(Shader shader, Sun sun) {
    float direction[3] = {sun.direction.x, sun.direction.y, sun.direction.z};

    float ambient[3] = {(float)sun.ambient.r / 255.0f,
                        (float)sun.ambient.g / 255.0f,
                        (float)sun.ambient.b / 255.0f};

    float diffuse[3] = {(float)sun.diffuse.r / 255.0f,
                        (float)sun.diffuse.g / 255.0f,
                        (float)sun.diffuse.b / 255.0f};

    SetShaderValue(shader, sun.directionLoc, direction, UNIFORM_VEC3);
    SetShaderValue(shader, sun.ambientLoc, ambient, UNIFORM_VEC3);
    SetShaderValue(shader, sun.diffuseLoc, diffuse, UNIFORM_VEC3);
}