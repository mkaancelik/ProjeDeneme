#version 330 core
out vec4 FragColor;

in vec3 FragPos_World;
in vec3 Normal_World;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;    // Light position in world space
uniform vec3 viewPos;     // Camera position in world space

// Spotlight properties (example)
uniform vec3 spotLightPos;
uniform vec3 spotLightDir;
uniform float spotLightCutOff; // cosine of the angle
uniform float spotLightOuterCutOff;
uniform vec3 spotLightColor;
uniform bool spotLightOn;


void main()
{
    vec3 finalColor = vec3(0.0);

    // Ambient light
    float ambientStrength = 0.15; // Main ambient light
    vec3 ambient = ambientStrength * lightColor;
    finalColor += ambient * objectColor;

    // Diffuse lighting (main light)
    vec3 norm = normalize(Normal_World);
    vec3 lightDir = normalize(lightPos - FragPos_World);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    finalColor += diffuse * objectColor;

    // Specular lighting (main light)
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos_World);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    finalColor += specular * objectColor;


    // Spotlight (if active)
    if (spotLightOn) {
        vec3 lightDirSpot = normalize(spotLightPos - FragPos_World);
        float theta = dot(lightDirSpot, normalize(-spotLightDir)); // -spotLightDir because spotLightDir points from the light
        float epsilon = spotLightCutOff - spotLightOuterCutOff;
        float intensity = clamp((theta - spotLightOuterCutOff) / epsilon, 0.0, 1.0);

        if (theta > spotLightOuterCutOff) { // or spotLightCutOff for hard edge
             // Ambient for spotlight (can be different)
            vec3 ambientSpot = 0.05f * spotLightColor;
            finalColor += ambientSpot * objectColor * intensity;

            // Diffuse for spotlight
            float diffSpot = max(dot(norm, lightDirSpot), 0.0);
            vec3 diffuseSpot = diffSpot * spotLightColor;
            finalColor += diffuseSpot * objectColor * intensity;
            
            // Specular for spotlight
            vec3 reflectDirSpot = reflect(-lightDirSpot, norm);
            float specSpot = pow(max(dot(viewDir, reflectDirSpot), 0.0), 16); // smaller exponent for softer highlight
            vec3 specularSpot = specularStrength * specSpot * spotLightColor;
            finalColor += specularSpot * objectColor * intensity;
        }
    }
    
    FragColor = vec4(finalColor, 1.0);
}