#version 460

layout(std140) uniform Global {
    mat4 projection;
    mat4 modelView;
    mat4 inverseProjection;
    mat4 inverseModelView;
    vec2 screenResolution;
    float tickDelta;
};

in vec3 pos;

out float alpha;

void main() {
    vec4 renderPosition = modelView * vec4(pos, 1.0);
	gl_Position = projection * renderPosition;
	alpha = 0.2 + 0.4 * smoothstep(2.0, 0.0, length(renderPosition));
}