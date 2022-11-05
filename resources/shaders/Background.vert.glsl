#version 460

in vec2 pos;

out vec2 fragCoord;

void main() {
	gl_Position = vec4(fragCoord, 1.0, 1.0);
	fragCoord = pos * 0.5 + 0.25;
}