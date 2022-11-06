#version 460

layout(std140) uniform Global {
    mat4 projection;
    mat4 modelView;
    mat4 inverseProjection;
    mat4 inverseModelView;
    vec2 screenResolution;
    float tickDelta;
};

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNormal;
layout(location = 2) in vec4 vertColor;

out vec3 normal;
out vec4 color;

void main() {
    gl_Position = projection * modelView * vec4(vertPos, 1.0);
	normal = normalize(mat3(modelView) * vertNormal);
	color = vertColor;
}