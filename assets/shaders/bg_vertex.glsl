#version 460 core

out vec3 WorldPos;

uniform mat4 invViewProj;

void main() {
    // Generate full screen triangle using gl_VertexID
    float x = -1.0 + float((gl_VertexID & 1) << 2);
    float y = -1.0 + float((gl_VertexID & 2) << 1);
    
    vec4 ndcPos = vec4(x, y, 1.0, 1.0); // At far clip plane
    gl_Position = ndcPos;

    vec4 worldVec = invViewProj * ndcPos;
    WorldPos = worldVec.xyz / worldVec.w;
}
