#version 460

layout(std140) uniform Global {
    mat4 projection;
    mat4 modelView;
    mat4 inverseProjection;
    mat4 inverseModelView;
    vec2 screenResolution;
    float tickDelta;
};

in vec4 color;
in vec3 rawPos;
in vec2 fragCoord;

out vec4 fragColor;

const float PI = 3.1415926;

float rand3D(vec3 co) {
    return fract(sin(dot(co.xyz ,vec3(12.9898,78.233,144.7272))) * 43758.5453);
}

float dotNoise(vec3 coord, float fractionalMaxDotSize, float dDensity) {
    float intX = coord.x - fract(coord.x);
    float fracX = coord.x - intX;

    float intY = coord.y - fract(coord.y);
    float fracY = coord.y - intY;

    float intZ = coord.z - fract(coord.z);
    float fracZ = coord.z - intZ;

    if (rand3D(vec3(intX + 1.0, intY + 1.0, intZ + 1.0)) > dDensity) {
        return 0.0;
    }

    float xoffset = (rand3D(vec3(intX, intY, intZ - 1.0)) - 0.5);
    float yoffset = (rand3D(vec3(intX + 1.0, intY, intZ + 1.0)) - 0.5);
    float zoffset = (rand3D(vec3(intX - 1.0, intY + 1.0, intZ)) - 0.5);
    float dotSize = 0.5 * fractionalMaxDotSize * max(0.25, rand3D(vec3(intX, intY + 1.0, intZ)));

    vec3 truePos = vec3(
        0.5 + xoffset * (1.0 - 2.0 * dotSize), 
        0.5 + yoffset * (1.0 - 2.0 * dotSize),
        0.5 + zoffset * (1.0 - 2.0 * dotSize)
    );

    float distance = length(truePos - vec3(fracX, fracY, fracZ));

    return 1.0 - smoothstep(0.3 * dotSize, 1.0 * dotSize, distance);
}

void main() {
	fragColor = color;
	fragColor.a += 0.5 * smoothstep(100.0, 215.0, length(rawPos));
	fragColor.rgb *= fragColor.a;
	fragColor.a = 1.0;
	fragColor.rgb += smoothstep(0.9, 1.0, dotNoise(rawPos / 2.0, 2.0, 0.1));
}