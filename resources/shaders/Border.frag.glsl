#version 460

in vec4 color;
in vec3 rawPos;

out vec4 fragColor;

void main() {
	fragColor = color;
	fragColor.a += 0.5 * smoothstep(100.0, 215.0, length(rawPos));
}