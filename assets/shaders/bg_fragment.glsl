#version 460 core

out vec4 FragColor;

in vec3 WorldPos;
uniform vec3 cameraPos;

void main() {
    vec3 viewDir = normalize(WorldPos - cameraPos);
    
    // Soft gradient mapping viewDir.y from horizon to zenith
    float t = clamp(viewDir.y, 0.0, 1.0);
    
    // Rich atmospheric scattering representation
    vec3 skyBottom = vec3(0.55, 0.75, 0.95);  // Bright azure / misty cyan horizon
    vec3 skyTop = vec3(0.1, 0.25, 0.6);       // Deep space blue zenith
    vec3 groundColor = vec3(0.1, 0.1, 0.12);  // Dark ground color when looking down

    vec3 finalColor = mix(skyBottom, skyTop, t);
    
    // If looking down (y < 0), fade into ground color
    if (viewDir.y < 0.0) {
        float g = clamp(-viewDir.y * 5.0, 0.0, 1.0); // Quick fade down
        finalColor = mix(skyBottom, groundColor, g);
    }
    
    FragColor = vec4(finalColor, 1.0);
}
