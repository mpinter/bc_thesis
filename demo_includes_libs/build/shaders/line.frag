#version 130

in vec3 outColor;

out vec4 fragColor;

uniform vec3 uniColor;

void main() {
	fragColor=vec4(uniColor,1.0);
}