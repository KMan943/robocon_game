#version 460 core
out vec4 FragColor;
in vec3 Normal;
uniform vec3 objectColor;

void main() {
    // Basic Directional Lighting
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    float diff = max(dot(normalize(Normal), lightDir), 0.3); // 0.3 is ambient
    FragColor = vec4(objectColor * diff, 1.0);
}