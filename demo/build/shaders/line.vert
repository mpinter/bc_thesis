#version 130

in vec3 vertex;

uniform mat4 uniMVP;

void main() {
	gl_Position=uniMVP*vec4(vertex,1.0);
}