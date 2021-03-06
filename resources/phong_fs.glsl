#version 330

// Input vertex attributes (from vertex shader)
in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

in float fragAffine;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

#define     MAX_LIGHTS             10 
#define     LIGHT_DIRECTIONAL       0
#define     LIGHT_POINT             1

struct Sun { // Directional light
    vec3 direction; 
    vec3 ambient;
    vec3 diffuse;
};

struct MaterialProperty {
    vec3 color;
    int useSampler;
    sampler2D sampler;
};

struct Light {
    int enabled;
    int type;
    vec3 position;
    vec3 target;
    vec4 color;
};

// Input lighting values
uniform Light lights[MAX_LIGHTS];
uniform Sun sun;

uniform vec3 viewPos;

vec3 calculate_sun(Sun sun, vec3 normal, vec3 viewDir){
    vec3 lightDir = normalize(-sun.direction); 
    float diff = max(dot(normal, lightDir), 0.0);
    return sun.ambient + sun.diffuse * diff;
}

void main()
{
    // Texel color fetching from texture sampler
    vec4 texelColor = texture(texture0, fragTexCoord/fragAffine);
    vec3 lightDot = vec3(0.0);
    vec3 normal = normalize(fragNormal);
    vec3 viewD = normalize(viewPos - fragPosition);
    vec3 specular = vec3(0.0);

    if (texelColor.a < 0.1) discard;

    float constant = 1.0; 
    float linear = 0.09;
    float quadratic = 0.032;

    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        if (lights[i].enabled == 1)
        {
            vec3 light = vec3(0.0);
            
            if (lights[i].type == LIGHT_DIRECTIONAL) {
                light = -normalize(lights[i].target - lights[i].position);
            }
            
            if (lights[i].type == LIGHT_POINT) {
                light = normalize(lights[i].position - fragPosition);
            }

            float dist = length(lights[i].position - fragPosition);
            
            float atten = 1.0 / (constant + linear * dist + quadratic * (dist * dist));

            float NdotL = max(dot(normal, light), 0.0);
            lightDot += lights[i].color.rgb*NdotL*atten;

            float specCo = 0.0;
            if (NdotL > 0.0) specCo = pow(max(0.0, dot(viewD, reflect(-(light), normal))), 16); // 16 refers to shine
            specular += specCo*atten*lights[i].color.rgb;
        }
    }

    vec3 sun_color = calculate_sun(sun, normal, viewD); 
    vec4 result = vec4(0, 0, 0, 1);

    result = vec4(texelColor.rgb, 1.0) * colDiffuse; 
    result *= vec4(sun_color, 1.0) + vec4(lightDot, 1.0) + vec4(specular, 1.0);
    result.a = colDiffuse.a;

    finalColor = result;

    // // Gamma correction
    // finalColor = pow(finalColor, vec4(1.0/2.0)); 

    // Fog
    float dist = length(viewPos - fragPosition);

    const vec4 fogColor = vec4(0.0, 0.0, 0.0, 0.0);

    float fogDensity = 0.05;
    float fogFactor = 1.0/exp((dist*fogDensity*0.02)*(dist*fogDensity));

    fogFactor = clamp(fogFactor, 0.0, 1.0); 
    finalColor = mix(fogColor, finalColor, fogFactor);
}
