#version 460

layout(std140) uniform Global {
    mat4 projection;
    mat4 modelView;
    mat4 inverseProjection;
    mat4 inverseModelView;
    vec2 screenResolution;
    float tickDelta;
};

layout(location = 0) in vec3 pos;

out vec4 color;

void main() {
    vec4 renderPosition = modelView * vec4(pos, 1.0);
	gl_Position = projection * renderPosition;
    color = vec4(pos / 256.0 + 0.5, 0.2 + 0.4 * smoothstep(2.0, 0.0, length(renderPosition)));
}