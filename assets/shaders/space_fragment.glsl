#version 460 core
out vec4 FragColor;
uniform float uTime;
uniform vec2 uResolution;

float hash(vec2 p) {
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return fract(p.x * p.y);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

float fbm(vec2 p) {
    float v = 0.0;
    float a = 0.5;
    for (int i = 0; i < 5; i++) {
        v += a * noise(p);
        p *= 2.0;
        a *= 0.5;
    }
    return v;
}

void main() {
    vec2 uv = (gl_FragCoord.xy * 2.0 - uResolution.xy) / min(uResolution.x, uResolution.y);
    
    // Slow rotation
    float angle = uTime * 0.05;
    mat2 rot = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
    uv *= rot;

    // Nebula Layer
    float n1 = fbm(uv * 0.8 + uTime * 0.02);
    float n2 = fbm(uv * 1.2 - uTime * 0.01);
    
    vec3 nebulaColor = vec3(0.1, 0.0, 0.2) * n1; // Deep purple
    nebulaColor += vec3(0.0, 0.1, 0.3) * n2;   // Blue
    nebulaColor += vec3(0.3, 0.1, 0.2) * pow(fbm(uv * 2.0 + 10.0), 3.0); // Magenta highlights

    // Galaxy Core
    float dist = length(uv);
    float core = exp(-dist * 4.0);
    vec3 coreColor = vec3(0.9, 0.8, 0.6) * core;

    // Starfield Layer
    vec2 starUV = uv * 50.0;
    float stars = pow(hash(floor(starUV)), 20.0);
    stars *= step(0.9, hash(starUV + 123.45)); // Flicker/Variation
    
    vec3 finalColor = nebulaColor + coreColor + vec3(stars);
    
    // Vignette
    finalColor *= smoothstep(2.0, 0.5, dist);

    FragColor = vec4(finalColor, 1.0);
}
