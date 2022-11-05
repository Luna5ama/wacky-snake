#version 460

uniform vec2 mouse;
uniform vec2 resolution;

in vec2 fragCoord;

out vec4 fragColor;

const float PI = 3.14159;

const int STAR_COUNT = 100;
const vec3 STAR_COLOR = vec3(1.0);

const vec3 SUN_DIR = vec3(0.0, 0.0, 1.0);
const vec3 SUN_COLOR = vec3(0.3, 0.21, 0.165);


float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void point(vec2 pos, float size) {
    float d = distance(fragCoord, pos);
    fragColor.rgb += smoothstep(size, 0.0, d) * STAR_COLOR;
}

void stars() {
    for (int i = 1; i < STAR_COUNT; i++) {
        float sx = rand(vec2(i, -i));
        float sy = rand(vec2(-sx, sx));
        float size = rand(vec2(sx, sy)) ;
        point(vec2(sx * 2.0 - mouse.x, sy * 2.0 - mouse.y), size * size * 2.0 + 1.0);
    }
}

vec3 calcDir(vec2 rotation) {
    float xzLen = cos(rotation.y);
    return vec3(xzLen * sin(-rotation.x), sin(rotation.y), xzLen * cos(rotation.x));	
}

void main() {
    fragColor = vec4(0.0, 0.0, 0.0, 1.0);
    
    float aspect = resolution.x / resolution.y;
    
    vec2 rotation = vec2(mouse.x * 2.0 - 1.0, mouse.y - 0.5) * PI;
    vec3 dir = calcDir(rotation);
    vec2 relativeRotation = vec2((fragCoord.x - 0.5) * aspect, fragCoord.y - 0.5) * PI + rotation;
    relativeRotation = clamp(relativeRotation, vec2(-PI, -0.5 * PI), vec2(PI, 0.5 * PI));
    vec3 relativeDir = calcDir(relativeRotation);
    
    vec2 res = 1.0 / resolution;
    
    stars();
    
    float flare = dot(dir, SUN_DIR);
    fragColor.rgb += flare * 0.5 * SUN_COLOR;
    
    float sun = max(distance(relativeDir, SUN_DIR), 0.0);
    fragColor.rgb += 0.4 / sun * SUN_COLOR;
}