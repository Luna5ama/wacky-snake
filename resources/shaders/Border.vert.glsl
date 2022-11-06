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

out vec2 fragCoord;
out vec3 rawPos;
out vec4 color;

void main() {
    vec4 renderPosition = modelView * vec4(pos, 1.0);
	gl_Position = projection * renderPosition;
    rawPos = pos;
    vec3 temp = pos / 256.0 + 0.5;
    color = vec4(1.0 - temp.r, temp.g, 0.5 * temp.b + 0.5, 0.4 * smoothstep(2.0, 0.0, length(renderPosition)));
	fragCoord = gl_Position.xy / gl_Position.w;
}